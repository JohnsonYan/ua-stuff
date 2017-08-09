/* Simple ARP Sniffer.
 * To compile: gcc arpsniffer.c -o arpsniffer -lpcap
 * Run as root.
 */

#include <pcap.h>
#include <stdlib.h>
#include <string.h>

// ARP Header, (assuming Ethernet + IPv4)
#define ARP_REQUEST 1
#define ARP_REPLY 2
typedef struct arphdr {
    u_int16_t htype; // Hardware Type
    u_int16_t ptype; // Protocol Type
    u_char hlen; // Hardware Address Length
    u_char plen; // Protocol Address Length
    u_int16_t oper; // Operation Code
    u_char sha[6]; // Sender hardware address
    u_char spa[4]; // Sender IP address
    u_char tha[6]; // Target hardware address
    u_char tpa[4]; // Target IP address
} arphdr_t;

#define MAXBYTES2CAPTURE 2048

int main(int argc, char *argv[])
{
    int i = 0;
    bpf_u_int32 netaddr = 0, mask = 0;
    struct bpf_program filter;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *descr = NULL;   // Network interface handler
    struct pcap_pkthdr pkthdr;  // Packet information (timestamp,size...)

}
