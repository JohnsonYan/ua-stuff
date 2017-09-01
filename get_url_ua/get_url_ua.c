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
    UINT8 *dup = strndup(l5, l5_len);   // 在内存中创建一个副本
    
    UINT8 *header = (UINT8 *)dup; // 避免在字符串切割时破坏pkt本身的数据
    UINT8 *token; // 用于保存每一行数据的指针
    UINT8 *value;   // 用于保存每个字段后面的的数据的指针
    UINT8 *url_token = NULL; // 用于保存分割url时的子串
    UINT8 *temp = NULL;
    UINT8 host[128];
    
    /* ================解析到fields_buffer================ */

    // 使用strtok函数以 \r\n标志 将header分成一行一行的子串
    token = strtok(header, "\r\n"); // 获取到第一行子串
    temp = token;   // 保存第一行字符供之后拼接URL使用

    while((token = strtok(NULL, "\r\n")) != NULL) {
        value = strstr(token, " "); // 注意到每个字段后面都有一个空格，故通过空格提取出后面的内容

        while(*value == 32) value++;    // 去掉多余空格

        if(strncmp(token, "User-Agent:", strlen("User-Agent:")) == 0) {
            *user_agent = value;    // 指针指向这个地址
            *useragent_len = strlen(value);
        }
        if(strncmp(token, "Host:", strlen("Host:")) == 0) {
            strncpy(host, value, strlen(value));
        }
    }

    /* ================拼接URL================ */
    url_token = strtok(temp, " ");  // 分割,提取子串
    url_token = strtok(NULL, " ");
    snprintf(url, url_len, "%s%s", host, url_token);
    
    free(dup);
    
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
        printf("[get_url_ua() failed with code %d]\n", result);

    printf("%s\n%s\n%d\n", *user_agent, url, useragent_len);
    // ---------------------------End parse------------------------------

    free(l5);
    free(user_agent);

    return 0;
}
