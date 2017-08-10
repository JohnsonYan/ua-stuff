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
    const unsigned char *packet = NULL; // Received raw data
    arphdr_t *arpheader = NULL;

    memset(errbuf, 0, PCAP_ERRBUF_SIZE);

    if (argc != 2) {
        printf("USAGE: arpsniffer <interface>\n");
        exit(1);
    }
    // open network device for packet capture
    descr = pcap_open_live(argv[1], MAXBYTES2CAPTURE, 0, 512, errbuf);
    // look up info from the capture device
    pcap_lookupnet(argv[1], &netaddr, &mask, errbuf);
    // compiles the fileter expression into a BPF filter program
    pcap_compile(descr, &filter, "arp", 1, mask);
    // load the filter program into the packet capture device
    pcap_setfilter(descr, &filter);

    while (1) {
        if ((packet = pcap_next(descr, &pkthdr)) == NULL)
            continue; // 出现错误或者没抓到包，返回NULL, 那就跳过这次循环, 防止出现空指针 
        arpheader = (struct arphdr *)(packet+14); // point to the arp header,前面14bytes是以太网首部

        printf("\n\nReceived Packet Size: %d bytes\n", pkthdr.len);
        
        printf("Hardware type: %s\n", (ntohs(arpheader->htype) == 1) ? "Ethernet":"Unknown");
        printf("Protocol type: %s\n", (ntohs(arpheader->ptype) == 0x0800) ? "IPv4":"Unknown");
        printf("Operation: %s\n", (ntohs(arpheader->oper) == ARP_REQUEST) ? "ARP Request":"ARP Reply");

        // if is Ethernet and IPv4, print packet contents
        if (ntohs(arpheader->htype) == 1 && ntohs(arpheader->ptype) == 0x0800) {
            printf("Sender MAC: ");
            for (i = 0; i < 6; i++) printf("%02x:", arpheader->sha[i]);
            printf("\nSender IP: ");
            for (i = 0; i < 4; i++) printf("%d.", arpheader->spa[i]);
            printf("\nTarget MAC: ");
            for (i = 0; i < 6; i++) printf("%02x:", arpheader->tha[i]);
            printf("\nTarget IP: ");
            for (i = 0; i < 4; i++) printf("%d.", arpheader->tpa[i]);
            printf("\n");
        }
    }

    return 0;
}
