#include <stdio.h>
#include <pcre.h>
#include <string.h>

#define OVCOUNT 30

#define BROWSER_CHROME "(?:Chrome|CrMo|CriOS)\\/([0-9\\.]*)"
#define OS_WINDOWS "Windows NT ([0-9\\.]*)"
#define KERNAL_APPLEWEBKIT "AppleWebKit\\/([0-9\\.]*)"

typedef struct {
    char browser[64];
    char kernal[64];
    char os[64];
}Info;

int match(char *src, const char *pattern, char *value)
{
    pcre *re;
    const char *error;
    int erroffset;
    int ov[OVCOUNT]; // 输出参数， 用来返回匹配位置偏移量的数组
    int rc, i;

    // re是编译好的正则表达式的pcre内部表示结构
    re = pcre_compile(pattern, 0, &error, &erroffset, NULL);
    if (re == NULL) {
        printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
        return 1;
    }
    
    // 执行正则匹配，成功返回非负数， 没有匹配返回负值
    rc = pcre_exec(re, NULL, src, strlen(src), 0, 0, ov, OVCOUNT);
    if (rc < 0) {
        if (rc == PCRE_ERROR_NOMATCH)
            printf("No match...\n");
        else
            printf("Matching error %d\n", rc);
        pcre_free(re);
        return 2;
    }
    
    char *result = src + ov[0];
    int result_len = ov[1] - ov[0];
    strncpy(value, result, result_len);
    value[result_len] = '\0'; 

    /* 如果有子串就通过这个循环打印出来
    for (i = 0; i < rc; i++) {
        printf("%d\t", rc);
        char *substring_start = src + ovector[2*i];
        int substring_length = ovector[2*i+1] - ovector[2*i];
        // %.*s 表示带长度的字符串输出,其后面格式为str_len, str
        printf("$%2d: %.*s\n", i, substring_length, substring_start);
    }
    */

    pcre_free(re);
    
    return 0;
}


int main()
{
    Info info;

    char *ua = "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.101 Safari/537.36";
    printf("UA: %s\n", ua);

    match(ua, BROWSER_CHROME, info.browser);
    match(ua, KERNAL_APPLEWEBKIT, info.kernal);
    match(ua, OS_WINDOWS, info.os);

    printf("浏览器: %s\n", info.browser);
    printf("浏览器内核: %s\n", info.kernal);
    printf("操作系统: %s\n", info.os);

    return 0;
}
