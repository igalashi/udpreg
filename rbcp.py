#!/usr/bin/env python

import socket
import struct
import sys


def rbcp_write1(host, port, addr, data):
    id   = 0x00
    len  = 0x1

    wmes = struct.pack('>4BIB',
        0xff, 0x80, id, len, addr, data)
    print repr(wmes)

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    print 'TO ', sock.gettimeout()
    sock.settimeout(3.0)
    print 'TO ', sock.gettimeout()
    sock.sendto(wmes, (host, port))

    try:
        rmes = sock.recv(128)

        print repr(rmes)
        rmes_t = struct.unpack('>4BIB', rmes)
        print rmes_t
        rval = rmes_t[5]
        
    except socket.timeout:
        print "RBCP Write Timeout", host, port
        rval = None

    sock.close()

    return rval

def rbcp_read(host, port, addr, len):
    id   = 0xaa

    wmes = struct.pack('>4BI',
        0xff, 0xc0, id, len, addr)
    print repr(wmes)

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(3.0)
    sock.sendto(wmes, (host, port))

    try:
        rmes = sock.recv(128+len)
        print repr(rmes)
        fmt = '>4BI' + str(len) + 'B'
        print fmt
        rmes_t = struct.unpack(fmt, rmes)
        print rmes_t
        rval = rmes_t[5:]
        print rval
    except socket.timeout:
        print "RBCP Read Timeout", host, port
        rval = None

    sock.close()

    return rval

class Rbcp:

    def __init__(self, host='192.168.10.16', port=4660):
        self.host = host
        self.port = port
        self.timeout = 3.0

    #def __del__(self):
    #    print 'Destruct Rbcp'

    def write1(self, addr, data):
        seq_id  = 0x00
        length  = 0x1

        wmes = struct.pack('>4BIB',
            0xff, 0x80, seq_id, length, addr, data)
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.settimeout(self.timeout)
        sock.sendto(wmes, (self.host, self.port))

        try:
            rmes = sock.recv(128)
            rmes_t = struct.unpack('>4BIB', rmes)
            rval = rmes_t[5]
        
        except socket.timeout:
            print "RBCP Write Timeout", self.host, self.port
            rval = None

        sock.close()
        return rval


    def read(self, addr, length):
        seq_id = 0xaa

        wmes = struct.pack('>4BI',
             0xff, 0xc0, seq_id, length, addr)
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.settimeout(self.timeout)
        sock.sendto(wmes, (self.host, self.port))

        try:
            rmes = sock.recv(128+length)
            fmt = '>4BI' + str(length) + 'B'
            rmes_t = struct.unpack(fmt, rmes)
            rval = rmes_t[5:]
        except socket.timeout:
            print "RBCP Read Timeout", self.host, self.port
            rval = ()

        sock.close()
        return rval


def main():

    host = '192.168.10.16'
    port = 4660

    #rbcp_write1(host, port, 0, 1)
    #rbcp_read(host, port, 0, 0x10)
    #rbcp_read(host, port, 0xe0100000, 0x10)

    #reg = Rbcp(host)
    #print reg.read(0xe0100000, 0x10)
    #print reg.host, reg.port, reg.timeout
    #reg.write1(0x00, 0)
    #print reg.read(0x00000000, 0x1)
    #reg.write1(0x00, 1)
    #print reg.read(0x00000000, 0x1)
    #print reg.host, reg.port, reg.timeout
    #del reg

    args = sys.argv
    #print args
    if len(args) == 1:
        print args[0], '-h <host name> -p <port number>'
        print args[0], '-r <address> <length>'
        print args[0], '-w <address> <length>'

    host_flag = False
    port_flag = False
    write_flag = 0
    read_flag = 0
    for argv in args:
        if argv == '-h':
            host_flag = True
            continue
        if host_flag:
            host = argv
            host_flag = False
            continue
        if argv == '-p':
            port_flag = True
            continue
        if port_flag:
            port = int(argv, 10)
            port_flag = False
            continue
        if argv == '-w':
            write_flag = 1
            continue
        if write_flag == 1:
            addr = int(argv, 16)
            write_flag = 2
            continue
        if write_flag  == 2 and isinstance(addr, int):
            data = int(argv, 16)
            write_flag = 9
            continue
        if argv == '-r':
            read_flag = 1
            continue
        if read_flag == 1:
            addr = int(argv, 16)
            read_flag = 2
            continue
        if read_flag == 2 and isinstance(addr, int):
            nread = int(argv, 16)
            read_flag = 9
            continue


    reg = Rbcp(host, port)
    print 'Host:', host, ' Port:',  port
    if write_flag == 9:
        print 'Write ', addr, ':', data, ', Echo back:', 
        print reg.write1(addr, data)
    if read_flag == 9:
        print 'Read ', addr, ':', nread
        rdata = reg.read(addr, nread)
        for i in range(len(rdata)):
            if (i != 0) and ((i % 16) == 0): print
            print '{:0>2x}'.format(rdata[i]),
        print

    del reg


if __name__ == '__main__':
    main()
