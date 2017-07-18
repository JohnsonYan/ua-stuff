#ifndef GENERATE_URL_H
#define GENERATE_URL_H

void write_file(const char* filename, const int n);
void get_rand_str(char *url, int str_len, unsigned int seed);
void generate_one_url(char *str, unsigned int seed);

#endif
