#!/usr/bin/env python3

import socket
import struct
import sys
import datetime


HOST = '192.168.10.55'
PORT = 24
SIZE = 1024*256

F_DUMP = True
F_TDUMP = False

def read_net(host, port):

    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((host, port))
    except socket.error as e:
        print("Connection error", e)
        s.close()
        return

    nread = 0
    while True:
        data = s.recv(SIZE)
        #print('Len:', len(data))
        if len(data) == 0:
            break
        if F_DUMP:
            udata = struct.unpack(str(len(data))+'B', data)
            for i in range(0, len(udata), 1):
                if nread % 16 == 0:
                    print('\n{:06x} : '.format(nread), end='')
                print('{:02x} '.format(udata[i]), end='')
                nread = nread + 1

        if F_TDUMP:
            print(data)

        sys.stdout.flush()

    s.close()

def print_help():
        print(args[0], '-h <host name> -p <port number>')
        print(args[0], '-d : diable hex dump')
        print(args[0], '-t : Text dump')
        print(args[0], '-s <Buffer size>')

if __name__ == '__main__':
    args = sys.argv
    
    host_flag = False
    port_flag = False
    size_flag = False
    for argv in args:
        if argv == '--help':
            print_help()
            exit(0)
        if argv == '-h':
            host_flag = True
            continue
        if host_flag:
            HOST = argv
            host_flag = False
            continue
        if argv == '-p':
            port_flag = True
            continue
        if port_flag:
            PORT = argv
            port_flag = False
            continue
        if argv == '-s':
            size_flag = True
            continue
        if size_flag:
            SIZE = argv
            size_flag = False
            continue
        if argv == '-d':
            F_DUMP = False
            continue
        if argv == '-t':
            F_TDUMP = True
            continue

    print('Host:', HOST, 'Port:', PORT)
    read_net(HOST, int(PORT))
