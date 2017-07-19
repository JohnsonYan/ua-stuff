#ifndef GENERATE_URL_H
#define GENERATE_URL_H

void get_rand_str(const char *metalist, char *str, int str_len, unsigned int seed);
void generate_one_url(char *url, unsigned int seed);
void generate_one_keyword(char *keyword, unsigned int seed);
void create_url_file(const char* filename, const int n);
void create_keyword_file(const char* filename, const int n);

#endif
