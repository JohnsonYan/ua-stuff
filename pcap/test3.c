#include <stdio.h>
#include <pcap.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>

void getPacket(u_char *arg, const struct pcap_pkthdr *pkthdr, const u_char * packet)
{
    struct ether_header *ethhdr = (struct ether_header *)packet;
    unsigned char *mac;
    int *id = (int *)arg;

    printf("id: %d\n", ++(*id));
    printf("Packet length: %d\n", pkthdr->len);
    printf("Number of bytes: %d\n", pkthdr->caplen);
    printf("Recieved time: %s", ctime((const time_t *)&pkthdr->ts.tv_sec));

    // ----------------decode ether_layer----------------------
    printf("\n-----ethernet protocol(physical layer)-----\n");
    printf("MAC source Address:\n");
    mac = ethhdr->ether_dhost;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac, *(mac+1), *(mac+2), *(mac+3), *(mac+4), *(mac+5));
    printf("MAC destination Address:\n");
    mac = ethhdr->ether_shost;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", *mac, *(mac+1), *(mac+2), *(mac+3), *(mac+4), *(mac+5));
    printf("protocol: %04x\n", ntohs(ethhdr->ether_type));
    // ----------------decode ether_layer----------------------

    // ----------------raw data-------------------------------
    int i;
    printf("\n-----raw data-----\n");
    for (i = 0; i < pkthdr->len; ++i) {
        printf(" %02x", packet[i]);
        if ((i+1)%16 == 0)
            printf("\n");
    }
    // ----------------raw data-------------------------------
    printf("\n\n");
}


int main(int argc, const char *argv[]) 
{
    if (argc != 2) {
        fprintf(stdout, "[format:\n\t program pcapfile\n");
        exit(0);
    }

    char errbuf[PCAP_ERRBUF_SIZE]; 
    

    // open device ,waiting packets
    pcap_t * device = pcap_open_live(argv[1], 1518, 0, 0, errbuf);
    if (!device) {
        printf("error: pcap_open_live(): %s\n", errbuf);
        exit(1);
    }

    // read 10 packets 
    int id = 0;
    pcap_loop(device, 1, getPacket, (u_char *)&id);

    pcap_close(device);

    return 0;
}
