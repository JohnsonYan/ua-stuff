/* Simple Sniffer
 * To compile: gcc simplesniffer.c -o simplesniffer -lpcap
 */

#include <pcap.h>
#include <string.h>
#include <stdlib.h>

#define MAXBYTES2CAPTURE 2048

void processPacket(u_char *arg, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
    int i = 0, *counter = (int *)arg;

    printf("Packet Count: %d\n", ++(*counter));
    printf("Received Packet Size: %d\n", pkthdr->len);
    printf("Payload:\n");
    for (i = 0; i < pkthdr->len; i++) {
        printf("%02x ", *(packet+i));

        if ((i%16 == 0 && i!= 0) || i == pkthdr->len-1)
            printf("\n");
    }
    return;
}

int main()
{
    int i = 0, count = 0;
    pcap_t *descr = NULL;
    pcap_if_t *alldevsp, *d;
    char errbuf[PCAP_ERRBUF_SIZE], *devbuf[20]; // devbuf 存储所有可供选择的网卡名称
    memset(errbuf, 0, PCAP_ERRBUF_SIZE);

    // Get the name of the first device suitable for capture
    if (0 != pcap_findalldevs(&alldevsp, errbuf)) {
        printf("Error in findalldevs\n");
        return 1;
    }

    /* list the devs list, choose one
     * TODO: add detect for the user input
     */
    printf("Select a ethernet card for the sniffer\n");
    for (i = 0, d = alldevsp; d != NULL; d = d->next, i++) {
        printf("[%d] %s\n", i, d->name);
        devbuf[i] = d->name;
    }

    scanf("%d", &i);

    printf("Opening device %s\n", devbuf[i]);

    // Open device
    descr = pcap_open_live(devbuf[i], MAXBYTES2CAPTURE, 10, 512, errbuf);

    // Loop forever & call processPacket() for every received packet
    pcap_loop(descr, -1, processPacket, (u_char *)&count);

    return 0;
}
