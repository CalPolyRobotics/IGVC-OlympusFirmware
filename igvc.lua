-------------------------------------------------------------------------------
--
-- author: Hadriel Kaplan <hadriel@128technology.com>
-- Copyright (c) 2015, Hadriel Kaplan
-- This code is in the Public Domain, or the BSD (3 clause) license
-- if Public Domain does not apply in your country.
--
-- Version: 1.0
-- Modified by CalPoly Robotics Club
--
-------------------------------------------------------------------------------
-- To use this dissector in wireshark, run the following commands
-- mkdir -p ~/.local/lib/wireshark/plugins
-- cp igvc.lua ~/.local/lib/wireshark/plugsins/

----------------------------------------
local debug_level = {
    DISABLED = 0,
    LEVEL_1  = 1,
    LEVEL_2  = 2
}

----------------------------------------
-- set this DEBUG to debug_level.LEVEL_1 to enable printing debug_level info
-- set it to debug_level.LEVEL_2 to enable really verbose printing
-- set it to debug_level.DISABLED to disable debug printing
-- note: this will be overridden by user's preference settings
local DEBUG = debug_level.LEVEL_1

-- a table of our default settings - these can be changed by changing
-- the preferences through the GUI or command-line; the Lua-side of that
-- preference handling is at the end of this script file
local default_settings =
{
    debug_level  = DEBUG,
    enabled      = true, -- whether this dissector is enabled or not
    port         = 9485, -- default TCP port number for FPM
    max_msg_len  = 1500, -- max length of FPM message
    subdissect   = true, -- whether to call sub-dissector or not
    subdiss_type = wtap.NETLINK, -- the encap we get the subdissector for
}


local dprint = function() end
local dprint2 = function() end
local function resetDebugLevel()
    if default_settings.debug_level > debug_level.DISABLED then
        dprint = function(...)
            info(table.concat({"Lua: ", ...}," "))
        end

        if default_settings.debug_level > debug_level.LEVEL_1 then
            dprint2 = dprint
        end
    else
        dprint = function() end
        dprint2 = dprint
    end
end
-- call it now
resetDebugLevel()


--------------------------------------------------------------------------------
-- creates a Proto object, but doesn't register it yet
local igvc_proto = Proto("igvc", "IGVC Header")


----------------------------------------
-- a function to convert tables of enumerated types to value-string tables
-- i.e., from { "name" = number } to { number = "name" }
local function makeValString(enumTable)
    local t = {}
    for name,num in pairs(enumTable) do
        t[num] = name
    end
    return t
end

local msgtype = 
{
    GET_STATUS               = 0,
    GET_STATUS_R             = 1,
    GET_SPEED                = 2,
    GET_SPEED_R              = 3,
    GET_STEERING             = 4,
    GET_STEERING_R           = 5, 
    GET_SONAR                = 6,
    GET_SONAR_R              = 7,
    SET_TURN_SIGNAL          = 8,
    SET_TURN_SIGNAL_R        = 9,
    SET_HEADLIGHTS           = 10,
    SET_HEADLIGHTS_R         = 11,
    SET_MISC_LIGHTS          = 12,
    SET_MISC_LIGHTS_R        = 13,
    GET_FNR                  = 14,
    GET_FNR_R                = 15,
    GET_AUTO_MAN_CONTROL     = 16,
    GET_AUTO_MAN_CONTROL_R   = 17,
    SET_FNR                  = 18,
    SET_FNR_R                = 19,
    SET_STEERING             = 20,
    SET_STEERING_R           = 21,
    SET_BRAKE                = 22,
    SET_BRAKE_R              = 23,
    UNUSED                   = 24,
    UNUSED_R                 = 25,
    SET_SPEED                = 26,
    SET_SPEED_R              = 27,
    SET_LEDS                 = 28,
    SET_LEDS_R               = 29,
    GET_POWER                = 30,
    GET_POWER_R              = 31,
    KILL                     = 32,
    KILL_R                   = 33
}

local msgtype_valstr = makeValString(msgtype)


----------------------------------------
-- a table of all of our Protocol's fields
local hdr_fields =
{
    start1    = ProtoField.uint8 ("igvc.start1", "Start Byte 1", base.HEX);   
    start2    = ProtoField.uint8 ("igvc.start2", "Start Byte 2", base.HEX);
    mtype     = ProtoField.uint8 ("igvc.mtype", "Message Type", base.HEX, msgtype_valstr);
    seq       = ProtoField.uint8 ("igvc.seq", "Sequence Number", base.DEC);
    len       = ProtoField.uint8 ("igvc.len", "Message Length", base.DEC);
    cksum     = ProtoField.uint8 ("igvc.cksum", "Checksum", base.HEX);
}

-- register the ProtoFields
igvc_proto.fields = hdr_fields

dprint2("igvc_proto ProtoFields registered")


-- due to a bug in older (prior to 1.12) wireshark versions, we need to keep newly created
-- Tvb's for longer than the duration of the dissect function (see bug 10888)
-- this bug only affects dissectors that create new Tvb's, which is not that common
-- but this FPM dissector happens to do it in order to create the fake SLL header
-- to pass on to the Netlink dissector
local tvbs = {}

---------------------------------------
-- This function will be invoked by Wireshark during initialization, such as
-- at program start and loading a new file
function igvc_proto.init()
    -- reset the save Tvbs
    tvbs = {}
end


-- this is the size of the FPM message header (4 bytes) and the minimum FPM
-- message size we need to figure out how much the rest of the Netlink message
-- will be
local IGVC_MSG_HDR_LEN = 5

-- some forward "declarations" of helper functions we use in the dissector
local dissectFPM

-- this holds the plain "data" Dissector, in case we can't dissect it as igvc
local data = Dissector.get("data")


--------------------------------------------------------------------------------
-- The following creates the callback function for the dissector.
-- It's the same as doing "fpm_proto.dissector = function (tvbuf,pkt,root)"
-- The 'tvbuf' is a Tvb object, 'pktinfo' is a Pinfo object, and 'root' is a TreeItem object.
-- Whenever Wireshark dissects a packet that our Proto is hooked into, it will call
-- this function and pass it these arguments for the packet it's dissecting.
function igvc_proto.dissector(tvbuf, pktinfo, root)
    dprint2("igvc_proto.dissector called")
    -- reset the save Tvbs
    tvbs = {}

    -- get the length of the packet buffer (Tvb).
    local pktlen = tvbuf:len()

    local bytes_consumed = 0

    -- we do this in a while loop, because there could be multiple FPM messages
    -- inside a single TCP segment, and thus in the same tvbuf - but our
    -- igvc_proto.dissector() will only be called once per TCP segment, so we
    -- need to do this loop to dissect each IGVC message in it
    while bytes_consumed < pktlen do

        -- We're going to call our "dissect()" function, which is defined
        -- later in this script file. The dissect() function returns the
        -- length of the IGVC message it dissected as a positive number, or if
        -- it's a negative number then it's the number of additional bytes it
        -- needs if the Tvb doesn't have them all. If it returns a 0, it's a
        -- dissection error.
        local result = dissectIGVC(tvbuf, pktinfo, root, bytes_consumed)

        if result > 0 then
            -- we successfully processed an IGVC message, of 'result' length
            bytes_consumed = bytes_consumed + result
            -- go again on another while loop
        elseif result == 0 then
            -- If the result is 0, then it means we hit an error of some kind,
            -- so return 0. Returning 0 tells Wireshark this packet is not for
            -- us, and it will try heuristic dissectors or the plain "data"
            -- one, which is what should happen in this case.
            return 0
        else
            -- we need more bytes, so set the desegment_offset to what we
            -- already consumed, and the desegment_len to how many more
            -- are needed
            pktinfo.desegment_offset = bytes_consumed

            -- invert the negative result so it's a positive number
            result = -result

            pktinfo.desegment_len = result

            -- even though we need more bytes, this packet is for us, so we
            -- tell wireshark all of its bytes are for us by returning the
            -- number of Tvb bytes we "successfully processed", namely the
            -- length of the Tvb
            return pktlen
        end        
    end

    -- In a TCP dissector, you can either return nothing, or return the number of
    -- bytes of the tvbuf that belong to this protocol, which is what we do here.
    -- Do NOT return the number 0, or else Wireshark will interpret that to mean
    -- this packet did not belong to your protocol, and will try to dissect it
    -- with other protocol dissectors (such as heuristic ones)
    return bytes_consumed
end


----------------------------------------
-- The following is a local function used for dissecting our IGVC messages
-- inside the TCP segment using the desegment_offset/desegment_len method.
-- It's a separate function because we run over TCP and thus might need to
-- parse multiple messages in a single segment/packet. So we invoke this
-- function only dissects one IGVC message and we invoke it in a while loop
-- from the Proto's main disector function.
--
-- This function is passed in the original Tvb, Pinfo, and TreeItem from the Proto's
-- dissector function, as well as the offset in the Tvb that this function should
-- start dissecting from.
--
-- This function returns the length of the IGVC message it dissected as a
-- positive number, or as a negative number the number of additional bytes it
-- needs if the Tvb doesn't have them all, or a 0 for error.
--
dissectIGVC = function (tvbuf, pktinfo, root, offset)
    dprint2("IGVC dissect function called")

    local msglen = tvbuf:len() - offset

    -- check if capture was only capturing partial packet size
    if msglen ~= tvbuf:reported_length_remaining(offset) then
        -- captured packets are being sliced/cut-off, so don't try to desegment/reassemble
        dprint2("Captured packet was shorter than original, can't reassemble")
        return 0
    end

    if msglen < IGVC_MSG_HDR_LEN then
        -- we need more bytes, so tell the main dissector function that we
        -- didn't dissect anything, and we need an unknown number of more
        -- bytes (which is what "DESEGMENT_ONE_MORE_SEGMENT" is used for)
        dprint2("Need more bytes to figure out FPM length field")
        -- return as a negative number
        return -DESEGMENT_ONE_MORE_SEGMENT
    end

    -- set the protocol column to show our protocol name
    pktinfo.cols.protocol:set("IGVC")

    -- set the INFO column too, but only if we haven't already set it before
    -- for this frame/packet, because this function can be called multiple
    -- times per packet/Tvb
    if string.find(tostring(pktinfo.cols.info), "^IGVC") == nil then
        pktinfo.cols.info:set("IGVC")
    end

    local len = tvbuf:range(offset + 4, 1):uint()

    -- We start by adding our protocol to the dissection display tree.
    local tree = root:add(igvc_proto, tvbuf:range(offset, len))

    local start1 = tvbuf:range(offset, 1):uint()
    local start2 = tvbuf:range(offset + 1, 1):uint()
    local mtype = tvbuf:range(offset + 2, 1):uint()
    local seq = tvbuf:range(offset + 3, 1):uint()
    local cksum = tvbuf:range(offset + len - 1, 1):uint()

    tree:add(hdr_fields.start1, start1)
    tree:add(hdr_fields.start2, start2)
    tree:add(hdr_fields.mtype, mtype)
    tree:add(hdr_fields.seq, seq)
    tree:add(hdr_fields.len, len)
    tree:add(hdr_fields.cksum, cksum)

    -- Run data on info between headers, but don't include checksum
    tvbs[#tvbs+1] = tvbuf(offset + IGVC_MSG_HDR_LEN, len - IGVC_MSG_HDR_LEN - 1):tvb()
    data:call(tvbs[#tvbs], pktinfo, root)

    return len
end
--------------------------------------------------------------------------------
-- We want to have our protocol dissection invoked for a specific TCP port,
-- so get the TCP dissector table and add our protocol to it.
local function enableDissector()
    -- using DissectorTable:set() removes existing dissector(s), whereas the
    -- DissectorTable:add() one adds ours before any existing ones, but
    -- leaves the other ones alone, which is better
    DissectorTable.get("tcp.port"):add(default_settings.port, igvc_proto)
end
-- call it now, because we're enabled by default
enableDissector()

local function disableDissector()
    DissectorTable.get("tcp.port"):remove(default_settings.port, igvc_proto)
end


--------------------------------------------------------------------------------
-- preferences handling stuff
--------------------------------------------------------------------------------

local debug_pref_enum = {
    { 1,  "Disabled", debug_level.DISABLED },
    { 2,  "Level 1",  debug_level.LEVEL_1  },
    { 3,  "Level 2",  debug_level.LEVEL_2  },
}

----------------------------------------
-- register our preferences
igvc_proto.prefs.enabled     = Pref.bool("Dissector enabled", default_settings.enabled,
                                        "Whether the FPM dissector is enabled or not")

igvc_proto.prefs.subdissect  = Pref.bool("Enable sub-dissectors", default_settings.subdissect,
                                        "Whether the FPM packet's content" ..
                                        " should be dissected or not")

igvc_proto.prefs.debug       = Pref.enum("Debug", default_settings.debug_level,
                                        "The debug printing level", debug_pref_enum)

----------------------------------------
-- the function for handling preferences being changed
function igvc_proto.prefs_changed()
    dprint2("prefs_changed called")

    default_settings.subdissect  = igvc_proto.prefs.subdissect

    default_settings.debug_level = igvc_proto.prefs.debug
    resetDebugLevel()

    if default_settings.enabled ~= igvc_proto.prefs.enabled then
        default_settings.enabled = igvc_proto.prefs.enabled
        if default_settings.enabled then
            enableDissector()
        else
            disableDissector()
        end
        -- have to reload the capture file for this type of change
        reload()
    end

end

dprint2("pcapfile Prefs registered")
