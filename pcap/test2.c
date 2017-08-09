#include <stdio.h>
#include <pcap.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
typedef struct macaddr
{
    unsigned short a[3];
    unsigned short b[3];
    unsigned short ether_type;
};

void getPacket(u_char *arg, const struct pcap_pkthdr *pkthdr, const u_char * packet)
{
    struct macaddr *ethhdr = (struct macaddr *)packet;
    unsigned short *mac;
    printf("\n-----ethernet protocol(physical layer)-----\n");
    printf("MAC source Address:\n");
    mac = ethhdr->a;
    printf("%04x:%04x:%04x\n", ntohs(*mac), ntohs(*(mac+1)), ntohs(*(mac+2)));
    printf("MAC destination Address:\n");
    mac = ethhdr->b;
    printf("%04x:%04x:%04x\n", ntohs(*mac), ntohs(*(mac+1)), ntohs(*(mac+2)));
    printf("protocol: %04x\n", ntohs(ethhdr->ether_type));
    /*
       int *id = (int *)arg;

       printf("id: %d\n", ++(*id));
       printf("Packet length: %d\n", pkthdr->len);
       printf("Number of bytes: %d\n", pkthdr->caplen);
       printf("Recieved time: %s", ctime((const time_t *)&pkthdr->ts.tv_sec));

       int i;
       for (i = 0; i < pkthdr->len; ++i) {
       printf(" %02x", packet[i]);
       if ((i+1)%16 == 0)
       printf("\n");
       }
       printf("\n\n");
       */
}

/**************************************************************************** 
 * 函数名称: str_to_hex 
 * 函数功能: 字符串转换为十六进制 
 * 输入参数: string 字符串 cbuf 十六进制 len 字符串的长度。 
 * 输出参数: 无 
 * *****************************************************************************/   
static int str_to_hex(char *string, unsigned char *cbuf, int len)  
{  
    unsigned char high, low;  
    int idx, ii=0;  
    for (idx=0; idx<len; idx+=2)   
    {  
        high = string[idx];  
        low = string[idx+1];  

        if(high>='0' && high<='9')  
            high = high-'0';  
        else if(high>='A' && high<='F')  
            high = high - 'A' + 10;  
        else if(high>='a' && high<='f')  
            high = high - 'a' + 10;  
        else  
            return -1;  

        if(low>='0' && low<='9')  
            low = low-'0';  
        else if(low>='A' && low<='F')  
            low = low - 'A' + 10;  
        else if(low>='a' && low<='f')  
            low = low - 'a' + 10;  
        else  
            return -1;  

        cbuf[ii++] = high<<4 | low;  
    }  
    return 0;  
}  

int main(int argc, const char *argv[]) 
{
    if (argc != 2) {
        fprintf(stdout, "[format:\n\t program pcapfile\n");
        exit(0);
    }

    //unsigned short c[3];
    unsigned short *local;
    unsigned char cbuf[20];


    char errbuf[PCAP_ERRBUF_SIZE]; 
    FILE * fp;
    fp = fopen("mac.list", "r");
    char buf[15] = {'0'};
    fgets(buf, 20, fp);
    printf("%s\n", buf);
    str_to_hex(buf, cbuf,12); 
    
    local = &cbuf[0];
    printf("使用htons:%04x:%04x:%04x\n", htons(*local), htons(*(local+1)), htons(*(local+2)));
    printf("没有使用htons:%04x:%04x:%04x\n", *local, *(local+1), *(local+2));

    int i;
    printf("cbuf:");
    for (i=0;i<6;i += 2) {
        printf("%02x%02x", cbuf[i], cbuf[i+1]);
    }
    printf("\n\n");
    for (i=0;i<6;i += 1) {
        printf("%x\n", &cbuf[i]);
    }
    printf("\n\n");

    local = (unsigned short *)buf;
    printf("%04x:%04x:%04x\n", *local, *(local+1), *(local+2));

    // open pcap file ,waiting packets
    pcap_t * device = pcap_open_offline(argv[1], errbuf);
    if (!device) {
        printf("error: pcap_open_offline(): %s\n", errbuf);
        exit(1);
    }

    // read 10 packets 
    int id = 0;
    pcap_loop(device, 1, getPacket, (u_char *)&id);

    pcap_close(device);

    return 0;
}
