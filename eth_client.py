import socket
import sys

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_address = ('192.168.0.10', 7)
sock.connect(server_address)

try:
    startBytes = chr(0xF0)+chr(0x5a)+chr(0x00)+chr(0x00)
    message = 'This is the message. It will be repeated.'
    crc8 = chr(162)
    startBytes += chr(len(startBytes + message + crc8) + 1) #add packet length
    data = startBytes + message + crc8
    sock.sendall(data)
    print 'sent "%s"' % message

    amount_received = 0
    amount_expected = len(message)

    while amount_received < amount_expected:
        data = sock.recv(1500)
        amount_received += len(data)
        print >>sys.stderr, 'received "%s"' % data

finally:
    print >>sys.stderr, 'closing socket'
    sock.close
