#include "generate_url.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define URL_LEN 256 /* URL最大长度  */
#define STR_LEN_MAX 20  /* 产生随机字符串的最大长度 */
#define STR_LEN_MIN 5  /* 产生随机字符串的最小长度 */


/* get_rand_str 函数根据传入的参数将 str_len 长度的随机字符串拼接到 url 后面
 * seed 是随机数生成器的种子
 * */
void get_rand_str(char *url,int str_len, unsigned int seed)
{
	/* 定义随机生成字符串表 */
	const char *str = "0123456789abcdefghijklmnopqrstuvwxyz";
	int i, len;
 	char temp[2] = {0};

 	len = strlen(str);
 	srand(seed); /* 使用初始化种子初始化随机数生成器，防止产生相同的url */

 	for(i = 0; i < str_len; i++) {
 		/* rand()%len 可随机返回0-35之间的整数, str[0-35]可随机得到其中的字符 */
  		sprintf(temp,"%c",str[(rand()%len)]);
  		strncat(url,temp,1); /* 将产生的随机的一个字符拼接到字符串s的后面 */
 	}
}

/* 生成一条随机的url 
 * seed 是随机数生成器的种子 
 * */
void generate_one_url(char *url, unsigned int seed)
{
    int str_len = 0;

    /* Init */
    srand(seed);
    sprintf(url, "wwww.");
    
    /* 产生[a-b)之间的数，则 (rand() % (a-b)) + b*/
    /* 这里保证了生成字符串长度不至于过小 */
    str_len = (rand() % (STR_LEN_MAX - STR_LEN_MIN)) + STR_LEN_MIN;
    get_rand_str(url, str_len, seed); /* 获得随机字符串并拼接到url后面 */

    strcat(url, ".com/");

    str_len = (rand() % (STR_LEN_MAX - STR_LEN_MIN)) + STR_LEN_MIN;
    get_rand_str(url, str_len, seed); /* 获得随机字符串并拼接到url后面 */
}

/* 将 n 条 url 写入文件 */
void write_file(const char* filename, const int n)
{
    char url[URL_LEN];
    int i;
    FILE *fp;
    
    fp = fopen(filename, "w+");

    for(i = 1; i <= n; i++) {
        generate_one_url(url, (unsigned int)i);
        fprintf(fp, "%s\n", url);
    }
    
    if(fclose(fp) != 0)
        printf("Error in closing file\n");
}



int main(int argc, const char* argv[])
{
    if(argc < 3) {
        printf("format:\nprogram filename url-number");
        exit(0);
    }

    const int num = atoi(argv[argc-1]); /* 将要生成的url数目转换成整数 */

    write_file(argv[argc-2], num);

 	return 0;
}
