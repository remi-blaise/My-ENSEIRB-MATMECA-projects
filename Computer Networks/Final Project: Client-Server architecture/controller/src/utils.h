#ifndef UTILS_H
#define UTILS_H

#define GRN   "\x1B[92m"
#define BLEUE "\x1B[96m"
#define RED   "\x1B[91m"
#define RESET "\x1B[0m"

typedef struct elem {
    void * value;
    struct elem * next;
} elem_t;


void regex_error(int result);
/**
 * Return 0 if succeed.
 * If succeed, you have to call free_params to free memory.
 */
int regex_match(char * message, char * regex, char ** params, int params_length);
void free_params(char ** params, int params_length);

#endif
