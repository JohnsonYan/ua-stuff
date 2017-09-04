#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//-----------通用类型定义-------------------
#define  UINT8     unsigned char
#define  INT8      char
#define  UINT16    unsigned short int
#define  INT16     short int
#define  UINT32    unsigned int
#define  INT32     int
#define  UINT64    unsigned long long int
#define  INT64     long long int
//---------------------------------------------------


int get_url_useragent(UINT8 *l5, UINT16 l5_len, UINT8 **user_agent, UINT16 *useragent_len, UINT8 *url, UINT8 url_len)
{  
    UINT8 *token1, *token2; // 解析用的中间变量
    UINT8 host_len, path_len, *ua, *host, *path;

    // 解析User-Agent
    token1 = strstr(l5, "User-Agent:");
    token2 = strstr(token1, "\r\n");
    ua = strstr(token1, " ");
    while(*ua == 32) ua++;    // 去掉多余空格
    *user_agent = ua;
    *useragent_len = token2 - ua;
    
    // 解析Host
    token1 = strstr(l5, "Host:");
    token2 = strstr(token1, "\r\n");
    host = strstr(token1, " ");
    while(*host == 32) host++;    // 去掉多余空格
    host_len = token2 - host;
    
    // 解析path
    token1 = strstr(l5, "GET ");
    token2 = strstr(token1, " HTTP");
    path = strstr(token1, " ");
    while(*path == 32) path++;    // 去掉多余空格
    path_len = token2 - path;

    // 拼接url
    snprintf(url, url_len, "%.*s%.*s", host_len, host, path_len, path);

    return 1;
}



/* ================TEST================ */

int main()
{
    // ---------------------------Init start------------------------------
    int lSize = 0, result = 0;
    FILE *fp;
    UINT8 *l5;
    UINT16 l5_len;

    UINT8 **user_agent = (UINT8 **)malloc(sizeof(UINT8 *));
    UINT16 useragent_len;
    UINT8 url[256];
    UINT8 url_len = 255;

    // ----------------------------READ--------------------------------
    if((fp = fopen("1.txt", "rb")) == NULL) {
        perror("open failed");
        exit(1);
    }

    /* 获取文件大小 */
    fseek (fp , 0 , SEEK_END);
    lSize = ftell (fp);
    l5_len = lSize;
    rewind (fp);

    /* 分配内存存储整个文件 */
    l5 = (unsigned char*) malloc (sizeof(unsigned char)*lSize);
    if (l5 == NULL) {
        perror("Memory error");
        exit (2);
    }

    /* 将文件拷贝到l5中 */
    result = fread (l5,1,lSize,fp);
    if (result != lSize) {
        perror("Reading error");
        exit (3);
    }
    /* 现在整个文件已经在l5中，可由标准输出打印内容 */
    //printf("[HTTP Request Header]\n\n%s\n", l5);

    /* 关闭文件并释放内存 */
    if(fclose(fp) != 0) {
        perror("close failed");
    }

    // ----------------------------END READ--------------------------------
    // ---------------------------Init end------------------------------


    // ---------------------------Start parse------------------------------
    result = get_url_useragent(l5, l5_len, user_agent, &useragent_len, url, url_len);

    if(result == 1)
        printf("[Get results OK]\n\n");
    else
        printf("[get_url_ua() failed with code %d]", result);

    printf("[User-Agent]\n%.*s\n", useragent_len, *user_agent);
    printf("[URL]\n%s\n", url);
    // ---------------------------End parse------------------------------

    free(l5);
    free(user_agent);

    return 0;
}
