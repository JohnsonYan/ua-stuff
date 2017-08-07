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
// byte_jump
#define PARSE_REGEX_BYTE_JUMP  "^\\s*" \
                     "([^\\s,]+\\s*,\\s*[^\\s,]+)" \
                     "(?:\\s*,\\s*((?:multiplier|post_offset)\\s+[^\\s,]+|[^\\s,]+))?" \
                     "(?:\\s*,\\s*((?:multiplier|post_offset)\\s+[^\\s,]+|[^\\s,]+))?" \
                     "(?:\\s*,\\s*((?:multiplier|post_offset)\\s+[^\\s,]+|[^\\s,]+))?" \
                     "(?:\\s*,\\s*((?:multiplier|post_offset)\\s+[^\\s,]+|[^\\s,]+))?" \
                     "(?:\\s*,\\s*((?:multiplier|post_offset)\\s+[^\\s,]+|[^\\s,]+))?" \
                     "(?:\\s*,\\s*((?:multiplier|post_offset)\\s+[^\\s,]+|[^\\s,]+))?" \
                     "(?:\\s*,\\s*((?:multiplier|post_offset)\\s+[^\\s,]+|[^\\s,]+))?" \
                     "(?:\\s*,\\s*((?:multiplier|post_offset)\\s+[^\\s,]+|[^\\s,]+))?" \
                     "(?:\\s*,\\s*((?:multiplier|post_offset)\\s+[^\\s,]+|[^\\s,]+))?" \
                     "\\s*$"
// byte_extract
#define PARSE_REGEX_BYTE_EXTRACT "^"                                                  \
    "\\s*([0-9]+)\\s*"                                                   \
    ",\\s*(-?[0-9]+)\\s*"                                               \
    ",\\s*([^\\s,]+)\\s*"                                                \
    "(?:(?:,\\s*([^\\s,]+)\\s*)|(?:,\\s*([^\\s,]+)\\s+([^\\s,]+)\\s*))?" \
    "(?:(?:,\\s*([^\\s,]+)\\s*)|(?:,\\s*([^\\s,]+)\\s+([^\\s,]+)\\s*))?" \
    "(?:(?:,\\s*([^\\s,]+)\\s*)|(?:,\\s*([^\\s,]+)\\s+([^\\s,]+)\\s*))?" \
    "(?:(?:,\\s*([^\\s,]+)\\s*)|(?:,\\s*([^\\s,]+)\\s+([^\\s,]+)\\s*))?" \
    "(?:(?:,\\s*([^\\s,]+)\\s*)|(?:,\\s*([^\\s,]+)\\s+([^\\s,]+)\\s*))?" \
    "$"
// byte_test
#define PARSE_REGEX_BYTE_TEST  "^\\s*" \
                     "([^\\s,]+)" \
                     "\\s*,\\s*(\\!?)\\s*([^\\s,]*)" \
                     "\\s*,\\s*([^\\s,]+)" \
                     "\\s*,\\s*([^\\s,]+)" \
                     "(?:\\s*,\\s*([^\\s,]+))?" \
                     "(?:\\s*,\\s*([^\\s,]+))?" \
                     "(?:\\s*,\\s*([^\\s,]+))?" \
                     "(?:\\s*,\\s*([^\\s,]+))?" \
                     "(?:\\s*,\\s*([^\\s,]+))?" \
                     "\\s*$"
// filestore
#define PARSE_REGEX_FILESTORE  "^\\s*([A-z_]+)\\s*(?:,\\s*([A-z_]+))?\\s*(?:,\\s*([A-z_]+))?\\s*$"
// flags
#define PARSE_REGEX_FLAGS "^\\s*(?:([\\+\\*!]))?\\s*([SAPRFU120CE\\+\\*!]+)(?:\\s*,\\s*([SAPRFU12CE]+))?\\s*$"
// pcre
#define PARSE_CAPTURE_REGEX "\\(\\?P\\<([A-z]+)\\_([A-z0-9_]+)\\>"
#define PARSE_REGEX         "(?<!\\\\)/(.*(?<!(?<!\\\\)\\\\))/([^\"]*)"
// ssl_state
#define PARSE_REGEX1 "^\\s*(!?)([_a-zA-Z0-9]+)(.*)$"
#define PARSE_REGEX2 "^(?:\\s*[|,]\\s*(!?)([_a-zA-Z0-9]+))(.*)$"
// modbus
/**
 *  * \brief Regex for parsing the Modbus function string
 *   */
#define PARSE_REGEX_FUNCTION "^\\s*\"?\\s*function\\s*(!?[A-z0-9]+)(,\\s*subfunction\\s+(\\d+))?\\s*\"?\\s*$"
/**
 *  * \brief Regex for parsing the Modbus access string
 *   */
#define PARSE_REGEX_ACCESS "^\\s*\"?\\s*access\\s*(read|write)\\s*(discretes|coils|input|holding)?(,\\s*address\\s+([<>]?\\d+)(<>\\d+)?(,\\s*value\\s+([<>]?\\d+)(<>\\d+)?)?)?\\s*\"?\\s*$"
// classtype
#define DETECT_CLASSCONFIG_REGEX "^\\s*config\\s*classification\\s*:\\s*([a-zA-Z][a-zA-Z0-9-_]*)\\s*,\\s*(.+)\\s*,\\s*(\\d+)\\s*$"
// reference-util
#define SC_RCONF_REGEX "^\\s*config\\s+reference\\s*:\\s*([a-zA-Z][a-zA-Z0-9-_]*)\\s+(.+)\\s*$"

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

    //match("alert ip [162.245.124.0/22,162.250.32.0/21,162.254.72.0/21,163.47.19.0/24,163.50.0.0/16,163.53.247.0/24,163.59.0.0/16,163.250.0.0/16,163.254.0.0/16,164.6.0.0/16,164.60.0.0/16,164.79.0.0/16,164.137.0.0/16,165.102.0.0/16,165.192.0.0/16,165.205.0.0/16,165.209.0.0/16,166.117.0.0/16,167.74.0.0/18,167.87.0.0/16] any -> $HOME_NET any (msg:\"ET DROP Spamhaus DROP Listed Traffic Inbound group 15\"; reference:url,www.spamhaus.org/drop/drop.lasso; threshold: type limit, track by_src, seconds 3600, count 1; classtype:misc-attack; flowbits:set,ET.Evil; flowbits:set,ET.DROPIP; sid:2400014; rev:2608;)", CONFIG_PCRE);
    //match("msg:\"ET     DROP Spamhaus DROP Listed Traffic Inbound group 15\"; reference:url,www.spamhaus.org/drop/drop.lasso; threshold: type limit, track by_src, seconds 3600, count 1; classtype:mis    c-attack; flowbits:set,ET.Evil; flowbits:set,ET.DROPIP; sid:2400014; rev:2608;", OPTION_PCRE); 
    //match("bytes 1", "\\s*(bytes\\s+(\\d+),?)?"); // match bytes 1
    //match("offset 2", "\\s*(offset\\s+(\\d+),?)?"); // match offset 2
    //match("relative", "\\s*(\\w+)?"); // match relative
    //match("421441515.5124515", ".*?(\\d+)(\\.(\\d+))?");
    //match(" 4,0 , relative , little, string, dec, align, from_beginning , multiplier 2 , post_offset -16", PARSE_REGEX_BYTE_JUMP);
    //match("4, 2, one,  ralign 4, relative, little, multiplier 2, string hex", PARSE_REGEX_BYTE_EXTRACT);
    //match("4, >, 1, 0, string, hex", PARSE_REGEX_BYTE_TEST);
    //match("to_server , file, tx", PARSE_REGEX_FILESTORE);
    //match("!SAPRFU120, 12", PARSE_REGEX_FLAGS);
    //match("\"/User[-_]Agent[:]?\\sMozilla/H\"", PARSE_CAPTURE_REGEX);
    //match("\"/User[-_]Agent[:]?\\sMozilla/H\"", PARSE_REGEX);
    //match("server_hello, client_hello", PARSE_REGEX1);
    //match("|jfkaljflk", PARSE_REGEX2);
    //match("function 8, subfunction 4", PARSE_REGEX_FUNCTION);
    //match("access write, address 1500<>2000, value >2000", PARSE_REGEX_ACCESS);
    //match("config classification: not-suspicious,Not Suspicious Traffic,3\n", DETECT_CLASSCONFIG_REGEX);
    //match("config reference:aliyun www.taobao.com", SC_RCONF_REGEX);
    match("0479-4284991", "^0[0-9]{2,3}-[0-9]{7,8}$");
    return 0;
}
