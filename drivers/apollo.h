#ifndef __APOLLO_H__
#define __APOLLO_H__

typedef enum APOLLO_MSG {
    SET_TURN_SIGNAL =    0x00, 
    SET_HEADLIGHTS =     0x01, 
    SET_MISC_LIGHTS =    0x02, 
    GET_LIGHT_SENSOR =   0x03, 
    BOOTLOAD =           0x0A
} APOLLO_MSG;


#endif

