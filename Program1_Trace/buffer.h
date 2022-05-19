#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <pcap/pcap.h>
#include <arpa/inet.h>
#include <bits/types.h>

#include "checksum.h"


#define HEADER_28_BYTE 14
#define SOURCE_ADDRESS 6
#define ETH_HEAD_TYPE 12
#define IP_HEADER_LEN 14


#define TOS 15
#define TOTAL_PDU_LEN 16
#define TTL 22
#define PROTOCOL 23
#define CHECKSUM 24

#define SENDER_IP 26
#define DEST_IP 30

#define ICMP_TYPE 34
#define ARP_OPCODE 20
#define ARP_SENDER_MAC 22
#define ARP_SENDER_IP 28
#define ARP_TARGET_MAC 32
#define ARP_TARGET_IP 38





#define SOURCE_PORT 34
#define DEST_PORT 36

#define SEQUENCE 38

#define ACK_NUM  42

#define FLAGS 47 

#define WINDOW 48



#endif