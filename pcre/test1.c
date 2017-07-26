#include <stdio.h>
#include <pcre.h>
#include <string.h>

#define OVECCOUNT 30

/* if enclosed in [], spaces are allowed */
#define CONFIG_PCRE_SRCDST "(" \
                            "[\\[\\]A-z0-9\\.\\:_\\$\\!\\-,\\/]+" \
                           "|" \
                            "\\[[\\[\\]A-z0-9\\.\\:_\\$\\!\\-,\\/\\s]+\\]"\
                           ")"

/* if enclosed in [], spaces are allowed */
#define CONFIG_PCRE_PORT   "([\\[\\]\\:A-z0-9_\\$\\!,\\s]+)"

/* format: action space(s) protocol spaces(s) src space(s) sp spaces(s) dir spaces(s) dst spaces(s) dp spaces(s) options */
#define CONFIG_PCRE "^([A-z]+)\\s+([A-z0-9\\-]+)\\s+" \
                    CONFIG_PCRE_SRCDST \
                    "\\s+"\
                    CONFIG_PCRE_PORT \
                    "\\s+(-\\>|\\<\\>|\\<\\-)\\s+" \
                    CONFIG_PCRE_SRCDST \
                    "\\s+" \
                    CONFIG_PCRE_PORT \
                    "(?:\\s+\\((.*)?(?:\\s*)\\))?(?:(?:\\s*)\\n)?\\s*$"
#define OPTION_PARTS 3
#define OPTION_PCRE "^\\s*([A-z_0-9-\\.]+)(?:\\s*\\:\\s*(.*)(?<!\\\\))?\\s*;\\s*(?:\\s*(.*))?\\s*$"


int match(char *src, const char *pattern)
{
    pcre *re;
    const char *error;
    int erroffset;
    int ovector[OVECCOUNT]; // 输出参数， 用来返回匹配位置偏移量的数组
    int rc, i;
    printf("String: %s\n", src);
    printf("Pattern: %s\n", pattern);
    
    // re是编译好的正则表达式的pcre内部表示结构
    re = pcre_compile(pattern, 0, &error, &erroffset, NULL);
    if (re == NULL) {
        printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
        return 1;
    }
    
    // 执行正则匹配，成功返回非负数， 没有匹配返回负值
    rc = pcre_exec(re, NULL, src, strlen(src), 0, 0, ovector, OVECCOUNT);
    if (rc < 0) {
        if (rc == PCRE_ERROR_NOMATCH)
            printf("No match...\n");
        else
            printf("Matching error %d\n", rc);
        pcre_free(re);
        return 2;
    }
    
    printf("\n Matched ...\n\n");
    for (i = 0; i < rc; i++) {
        char *substring_start = src + ovector[2*i];
        int substring_length = ovector[2*i+1] - ovector[2*i];
        // %.*s 表示带长度的字符串输出,其后面格式为str_len, str
        printf("$%2d: %.*s\n", i, substring_length, substring_start);
    }

    printf("--------------------------------------------\n");

    pcre_free(re);
    
    return 0;
    
}


int main()
{

    //match("msg:\"ET CNC Shadowserver Reported CnC Server IP group 23\"; reference:url,doc.emergingthreats.net/bin/view/Main/BotCC; reference:url,www.shadowserver.org; threshold: type limit, track by_src, seconds 3", OPTION_PCRE);
    match("alert ip [162.245.124.0/22,162.250.32.0/21,162.254.72.0/21,163.47.19.0/24,163.50.0.0/16,163.53.247.0/24,163.59.0.0/16,163.250.0.0/16,163.254.0.0/16,164.6.0.0/16,164.60.0.0/16,164.79.0.0/16,164.137.0.0/16,165.102.0.0/16,165.192.0.0/16,165.205.0.0/16,165.209.0.0/16,166.117.0.0/16,167.74.0.0/18,167.87.0.0/16] any -> $HOME_NET any (msg:\"ET DROP Spamhaus DROP Listed Traffic Inbound group 15\"; reference:url,www.spamhaus.org/drop/drop.lasso; threshold: type limit, track by_src, seconds 3600, count 1; classtype:misc-attack; flowbits:set,ET.Evil; flowbits:set,ET.DROPIP; sid:2400014; rev:2608;)", CONFIG_PCRE);
    match("msg:\"ET     DROP Spamhaus DROP Listed Traffic Inbound group 15\"; reference:url,www.spamhaus.org/drop/drop.lasso; threshold: type limit, track by_src, seconds 3600, count 1; classtype:mis    c-attack; flowbits:set,ET.Evil; flowbits:set,ET.DROPIP; sid:2400014; rev:2608;", OPTION_PCRE); 
    return 0;
}
