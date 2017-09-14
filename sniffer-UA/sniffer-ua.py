#!/usr/bin/env python
# encoding: utf-8

import sys, getopt
import scapy_http.http as HTTP
from scapy.all import *


def sniffer(pkt):

    if HTTP.HTTPRequest in pkt:
        f = open("ua.txt", "a")
        payload = pkt[TCP].payload

        headers, body = str(payload).split("\r\n\r\n", 1)
        fields = headers.split("\r\n")

        for field in fields:
            if "User-Agent:" in field:
                f.write(field)
                f.write("\n")

        f.close()
    else:
        pass


def usage():
    print "OPTIONS:"
    print "\t-i --- eg: -i eth0"


def main():
    usage()
    opts, args = getopt.getopt(sys.argv[1:], "i:")
    device="eno16777736"
    for op, value in opts:
        if op == "-i":
            device = value
        else:
            sys.exit()
    print "Program start running"
    sniff(filter = 'tcp and (port 80 or port 8080)', prn = sniffer, iface = device)

main()



