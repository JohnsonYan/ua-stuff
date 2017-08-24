#include <string.h>

#include <stdio.h> 
#include <stdlib.h>

/* HTTP Request header fields flags */
#define ACCEPT              (1l)     // 可接受的响应内容（Content-Types）类型
#define ACCEPT_CHARSET      (1l<<1)  // 可接受的字符集
#define ACCEPT_ENCODING     (1l<<2)  // 可接受的相应内容的编码方式
#define ACCEPT_LANGUAGE     (1l<<3)  // 可接受的响应内容语言列表
#define ACCEPT_DATETIME     (1l<<4)  // 可接受的按照时间来表示的版本
#define AUTHORIZATION       (1l<<5)  // 用于超文本传输协议的认证的认证信息
#define CACHE_CONTROL       (1l<<6)  // 用来指定当前的请求/回复中的，是否使用缓存机制
#define CONNECTION          (1l<<7)  // 该浏览器想要优先使用的连接类型 
#define COOKIE              (1l<<8)  // 由之前服务器通过Set-Cookie设置的一个HTTP协议Cookie 
#define CONTENT_LENGTH      (1l<<9)  // 以 八位字节数组（8位的字节） 表示的请求体的长度
#define CONTENT_MD5         (1l<<10) // 请求体的内容的二进制 MD5 散列值，以 Base64 编码的结果
#define CONTENT_TYPE        (1l<<11) // 请求体的 多媒体类型 （用于POST和PUT请求中）
#define DATE                (1l<<12) // 发送该消息的日期和时间
#define EXPECT              (1l<<13) // 表明客户端要求服务器做出特定的行为
#define FROM                (1l<<14) // 发起此请求的用户的邮件地址
#define HOST                (1l<<15) // 表示服务器的域名以及服务器所监听的端口号。如果所请求的端口是对应的服务的标准端口（80），则端口号可以省略。 
#define IF_MATCH            (1l<<16) // 仅当客户端提供的实体与服务器上对应的实体相匹配时，才进行对应的操作。主要用于像 PUT 这样的方法中，仅当从用户上次更新某个资源后，该资源未被修改的情况下，才更新该资源。 
#define IF_MODIFIED_SINCE   (1l<<17) // 允许在对应的资源未被修改的情况下返回304未修改
#define IF_NONE_MATCH       (1l<<18) // 允许在对应的内容未被修改的情况下返回304未修改（ 304 Not Modified ）
#define IF_RANGE            (1l<<19) // 如果该实体未被修改过，则向返回所缺少的那一个或多个部分。否则，返回整个新的实体 
#define IF_UNMODIFIED_SINCE (1l<<20) // 仅当该实体自某个特定时间以来未被修改的情况下，才发送回应。
#define MAX_FORWARDS        (1l<<21) // 限制该消息可被代理及网关转发的次数。
#define ORIGIN              (1l<<22) // 发起一个针对跨域资源共享的请求（该请求要求服务器在响应中加入一个Access-Control-Allow-Origin的消息头，表示访问控制所允许的来源）。
#define PRAGMA              (1l<<23) // 与具体的实现相关，这些字段可能在请求/回应链中的任何时候产生。 
#define PROXY_AUTHORIZATION (1l<<24) // 用于向代理进行认证的认证信息。
#define RANGE               (1l<<25) // 表示请求某个实体的一部分，字节偏移以0开始。 
#define REFERER             (1l<<26) // 表示浏览器所访问的前一个页面，可以认为是之前访问页面的链接将浏览器带到了当前页面。
#define TE                  (1l<<27) // 浏览器预期接受的传输时的编码方式：可使用回应协议头Transfer-Encoding中的值（还可以使用"trailers"表示数据传输时的分块方式）用来表示浏览器希望在最后一个大小为0的块之后还接收到一些额外的字段。
#define USER_AGENT          (1l<<28) // 浏览器的身份标识字符串
#define UPGRADE             (1l<<29) // 要求服务器升级到一个高版本协议。 
#define VIA                 (1l<<30) // 告诉服务器，这个请求是由哪些代理发出的。
#define WARNING             (1l<<31) // 一个一般性的警告，表示在实体内容体中可能存在错误。 
#define URL                 (1l<<32) // URL = HOST + PATH,新增字段的话请保证URL放在最后,代码里是通过len-1来访问的URL

#define TYPES_NUM 33
#define METHODS_NUM 8
#define MAX_BUFFER_SIZE 256

/* 保存http request header相关信息的结构体， flags用于提取特定的字段 */
typedef struct {
    unsigned char *l5;      // http报文内容
    int  l5_len;            // http报文长度
    unsigned long flags;    // 用于选择特定的字段         
}PKT_Info;

const char* types[]={
    "Accept:",              // id:0
    "Accept-Charset:",      // id:1
    "Accept-Encoding:",     // id:2
    "Accept-Language:",     // id:3
    "Accept-Datetime:",     // id:4
    "Authorization:",       // id:5
    "Cache-Control:",       // id:6
    "Connection:",          // id:7
    "Cookie:",              // id:8
    "Content-Length:",      // id:9
    "Content-MD5:",         // id:10
    "Content-Type:",        // id:11
    "Date:",                // id:12
    "Expect:",              // id:13
    "From:",                // id:14
    "Host:",                // id:15
    "If-Match:",            // id:16
    "If-Modified-Since:",   // id:17
    "If-None-Match:",       // id:18
    "If-Range:",            // id:19
    "If-Unmodified-Since:", // id:20
    "Max-Forwards:",        // id:21
    "Origin:",              // id:22
    "Pragma:",              // id:23
    "Proxy-Authorization:", // id:24
    "Range:",               // id:25
    "Referer:",             // id:26
    "TE:",                  // id:27
    "User-Agent:",          // id:28
    "Upgrade:",             // id:29
    "Via:",                 // id:30
    "Warning:",             // id:31
    "URL:"                  // id:32
};

// HTTP/1.0 定义了三种请求方法
// HTTP/1.1 新增了五种请求方法
const char* request_methods[] = {
    "GET", "POST", "HEAD",  
    "OPTIONS", "PUT", "DELETE", "TRACE", "CONNECT"
};


/* 功能：将HTTP Request 头部字段特定的值注册到results中 
* 参数说明：
*      pkt:包含http报文、flags的结构体
*      results:将flags中需要的字段保存在这里
*      res_len:results的个数
* 返回值：
*      1 成功
*      0 失败
*/
int get_http_header_info(PKT_Info *pkt, char ** results, int res_len)
{
    int i, j;
    char *header = (char *)pkt->l5; // 避免在字符串切割时破坏pkt本身的数据
    char *token = NULL; // 用于保存每一行数据的指针
    char *value = NULL;   // 用于保存每个字段后面的的数据的指针
    char fields_buffer[TYPES_NUM][MAX_BUFFER_SIZE] = {0}; // 用于临时保存取出来的数据
    char *url_token = NULL; // 用于保存分割url时的子串
    char *temp = NULL;
    
    /* ================解析到fields_buffer================ */

    // 使用strtok函数以 \r\n标志 将header分成一行一行的子串
    token = strtok(header, "\r\n"); // 获取到第一行子串
    temp = token;   // 保存第一行字符供之后拼接URL使用

    // 通过第一行请求方法判断是否是请求头，匹配请求方法则判定为请求头
    for(i = 0; i < METHODS_NUM; i++) {
        if(strncmp(token, request_methods[i], strlen(request_methods[i])) == 0)
            break;
        else if(i == (METHODS_NUM - 1))
            return 0;
    }
    
    while((token = strtok(NULL, "\r\n")) != NULL) {
        value = strstr(token, " "); // 注意到每个字段后面都有一个空格，故通过空格提取出后面的内容
    
        while(*value == 32) value++;    // 去掉多余空格
        
        for(i=0; i < (TYPES_NUM-1); i++) {
            if(strncmp(token, types[i], strlen(types[i])) == 0) {
                strncpy(fields_buffer[i], value, strlen(value));
            }
        }
    }

    /* ================拼接URL================ */
    url_token = strtok(temp, " ");  // 分割,提取子串
    url_token = strtok(NULL, " ");
    strncat(fields_buffer[TYPES_NUM-1], fields_buffer[15], MAX_BUFFER_SIZE);    // 索引15表示HOST字段, TYPES_NUM-1表示URL字段
    strncat(fields_buffer[TYPES_NUM-1], url_token, MAX_BUFFER_SIZE);            // 拼接URL 

    /* ================按flags传给results================ */
    i = 0;  // 初始化i，i指向results[i]
    for(j = 0; j < TYPES_NUM; j++) {
        if(pkt->flags & (1l<<j)) {
            // fields_buffer[]中的顺序很重要，应该和define的顺序一致
            strncpy(results[i], fields_buffer[j], strlen(fields_buffer[j]));
            // results[i]已存入数据，i自增1, 同时判断i是否超出长度范围  
            if(res_len < i++) {
                break;  
            }    
        }
    }
    
    return 1; 
}






/* ================TEST================ */

#define FLAGS ((1l<<33 )- 1) 
#define FLAGS_LEN 33
int main()
{
    // ---------------------------Init start------------------------------
    PKT_Info pkt_info;
    int lSize = 0, result = 0;
    FILE *fp;

    // ----------------------------READ--------------------------------
    if((fp = fopen("1.txt", "rb")) == NULL) {
        perror("open failed");
        exit(1);
    }

    /* 获取文件大小 */  
    fseek (fp , 0 , SEEK_END);  
    lSize = ftell (fp);  
    rewind (fp);  
  
    /* 分配内存存储整个文件 */   
    pkt_info.l5 = (unsigned char*) malloc (sizeof(unsigned char)*lSize);  
    if (pkt_info.l5 == NULL) {  
        perror("Memory error");   
        exit (2);  
    }  
  
    /* 将文件拷贝到pkt_info.l5中 */  
    result = fread (pkt_info.l5,1,lSize,fp);  
    if (result != lSize) {  
        perror("Reading error");
        exit (3);  
    }  
    /* 现在整个文件已经在pkt_info.l5中，可由标准输出打印内容 */  
    printf("[HTTP Request Header]\n\n%s\n", pkt_info.l5);   
  
    /* 关闭文件并释放内存 */  
    if(fclose(fp) != 0) {
        perror("close failed");
    }
    
    // ----------------------------END READ--------------------------------

    pkt_info.l5_len = lSize;
    pkt_info.flags = FLAGS;    

    char *results[FLAGS_LEN];
    int i;
    for(i = 0; i < FLAGS_LEN; i ++) {
         results[i] = (char *)malloc(sizeof(char) * MAX_BUFFER_SIZE);//将二维数组行地址赋值到对应的一维指针上。
    }
    
    // ---------------------------Init end------------------------------

    // ---------------------------Start parse------------------------------

    result = get_http_header_info(&pkt_info, results, FLAGS_LEN);
    
    if(result == 1)
        printf("[Get results OK]\n\n");
    else
        printf("[get_http_info() failed with code %d]\n", result);

    for(i=0;i<FLAGS_LEN;i++) printf("[%d]\t%s\n", i, results[i]);// test
    // ---------------------------End parse------------------------------
    
    free (pkt_info.l5);  
    for(i = 0; i < FLAGS_LEN; i ++) {
         free(results[i]);
    }

    return 0;
}
