/* 本程序生成随机的url或者keyword，输出到文件，每个url/keyword占一行;
 *
 * metalist中保存所有可取的字符，如需增加或删除字符，请修改metalist;
 * 宏定义中URL_LEN/KEYWORD_LEN表示数组的大小,MAX/MIN表示随机生成字符串的长度范围;
 * 
 * */
#include "../inc/generate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define URL_LEN 256 /* URL最大长度  */
#define URL_LEN_MAX 20  /* 产生随机URL字符串的最大长度,修改时注意不要超过最大长度 */
#define URL_LEN_MIN 5  /* 产生随机URL字符串的最小长度 */
#define KEYWORD_LEN 30 /* KEYWORD最大长度 */
#define KEYWORD_LEN_MAX 15  /* 产生随机keyword字符串的最大长度,修改时注意不要超过最大长度 */
#define KEYWORD_LEN_MIN 3  /* 产生随机keyword字符串的最小长度 */
#define BUF_SIZE 10 

/* get_rand_str 函数根据传入的参数将 str_len 长度的随机字符串拼接到 str 后面
 * metalist 所有的随机字符
 * seed 随机数生成器的种子
 * */
void get_rand_str(const char* metalist, char *str, int str_len, unsigned int seed)
{
	int i, len;
 	char temp[2] = {0};

 	len = strlen(metalist);
 	srand(seed); /* 使用初始化种子初始化随机数生成器，防止产生相同的随机数 */

 	for(i = 0; i < str_len; i++) {
 		/* rand()%len 从metalist中随机取出一个字符 */
  		snprintf(temp, BUF_SIZE, "%c", metalist[(rand()%len)]);
  		strncat(str, temp, BUF_SIZE); /* 将产生的随机的一个字符拼接到字符串str的后面 */
 	}
}


/* 生成一条随机的url 
 * seed 是随机数生成器的种子 
 * */
void generate_one_url(char *url, unsigned int seed)
{
	/* 定义随机生成url可使用的字符 */
	const char *metalist = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int str_len = 0;

    /* Init */
    srand(seed);
    snprintf(url, BUF_SIZE, "www.");
    
    /* 产生[a-b)之间的数，则 (rand() % (a-b)) + b*/
    /* 这里保证了生成字符串长度不至于过小 */
    str_len = (rand() % (URL_LEN_MAX - URL_LEN_MIN)) + URL_LEN_MIN;
    get_rand_str(metalist, url, str_len, seed); /* 获得随机字符串并拼接到url后面 */

    strncat(url, ".com/", BUF_SIZE);

    str_len = (rand() % (URL_LEN_MAX - URL_LEN_MIN)) + URL_LEN_MIN;
    get_rand_str(metalist, url, str_len, seed); /* 获得随机字符串并拼接到url后面 */
}

/* 生成一条随机的keyword 
 * seed 是随机数生成器的种子 
 * */
void generate_one_keyword(char *keyword, unsigned int seed)
{
	/* 定义随机生成keyword可使用的字符 
     * 符号参考自:https://my.oschina.net/cnlw/blog/355234
     * */
    const char *metalist = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz/?;:@&=$-_.+!*\'\"(),%";
    int str_len = 0;

    /* Init */
    srand(seed);

    /* 产生[a-b)之间的数，则 (rand() % (a-b)) + b*/
    /* 这里保证了生成字符串长度不至于过小 */
    str_len = (rand() % (KEYWORD_LEN_MAX - KEYWORD_LEN_MIN)) + KEYWORD_LEN_MIN;
    get_rand_str(metalist, keyword, str_len, seed); /* 获得随机字符串并拼接到url后面 */

}

/* 生成url文件 */
void create_url_file(const char* filename, const int n)
{
    char url[URL_LEN];
    int i;
    FILE *fp;
    
    if((fp = fopen(filename, "w+")) == NULL) {
        fprintf(stderr, "Error in opening file: %s\n", filename);
        exit(1);
    }

    for(i = 1; i <= n; i++) {
        generate_one_url(url, (unsigned int)i);
        fprintf(fp, "%s\n", url);
    }
    
    if(fclose(fp) != 0) {
        fprintf(stderr, "Error in closing file: %s\n", filename);
    }
}

/* 生成keyword文件 */
void create_keyword_file(const char* filename, const int n)
{
    char keyword[KEYWORD_LEN];
    int i;
    FILE *fp;
    
    if((fp = fopen(filename, "w+")) == NULL) {
        fprintf(stderr, "Error in opening file: %s\n", filename);
        exit(1);
    }

    for(i = 1; i <= n; i++) {
        memset(keyword, 0, sizeof(keyword));
        generate_one_keyword(keyword, (unsigned int)i);
        fprintf(fp, "%s\n", keyword);
    }
    
    if(fclose(fp) != 0) {
        fprintf(stderr, "Error in closing file: %s\n", filename);
    }


}


int main(int argc, const char* argv[])
{
    if(argc != 4) {
        fprintf(stdout, "[format]:\n\tprogram option filename number\n");
        fprintf(stdout, "[option]:\n\turl\tgenerate url\n\tkeyword\tgenerate keyword\n");
        exit(0);
    }

    if(strcmp(argv[argc-3], "url") == 0) {
        const int num = atoi(argv[argc-1]); /* 将要生成的url数目转换成整数 */
        create_url_file(argv[argc-2], num);
    }
    else if(strcmp(argv[argc-3], "keyword") == 0) {
        const int num = atoi(argv[argc-1]); /* 将要生成的keyword数目转换成整数 */
        create_keyword_file(argv[argc-2], num);
    }
    else {
        fprintf(stdout, "can't find the option you input, try again.\n");
    }

 	return 0;
}
