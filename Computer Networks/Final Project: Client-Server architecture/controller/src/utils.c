#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

#define DEBUG_REGEX 0
#define BUFFER_SIZE 50

void regex_error(int result) {
    if (result) {
        printf("ERROR in compiling regex\n");
        exit(EXIT_FAILURE);
    }
}

int regex_match(char * message, char * regex, char ** params, int params_length) {
    regex_t compiled;
    regmatch_t pmatch[10];
    int result;
    regex_error(regcomp(&compiled, regex, REG_EXTENDED));


    if (result = regexec(&compiled, message, params_length + 1, pmatch, 0) == 0) {
        for (int i = 0; i < params_length; ++i) {
            #if DEBUG_REGEX
            printf("  # Index: %d %d\n", pmatch[i + 1].rm_so, pmatch[i + 1].rm_eo);
            #endif
            params[i] = malloc(BUFFER_SIZE);
            sprintf(params[i], "%.*s", pmatch[i + 1].rm_eo - pmatch[i + 1].rm_so, message + pmatch[i + 1].rm_so);
        }

        #if DEBUG_REGEX
        printf("  # Match: %.*s\n  # Params:", pmatch[0].rm_eo - pmatch[0].rm_so, message + pmatch[0].rm_so);
        for (int i = 0; i < params_length; ++i) {
            printf(" %s", params[i]);
        }
        printf("\n");
        #endif
    }
    return result;
}

void free_params(char ** params, int params_length) {
    for (int i = 0; i < params_length; ++i) {
        free(params[i]);
    }
}

// inline function to swap two numbers
void swap(char *x, char *y) {
	char t = *x; *x = *y; *y = t;
}

// function to reverse buffer[i..j]
char* reverse(char *buffer, int i, int j)
{
	while (i < j)
		swap(&buffer[i++], &buffer[j--]);

	return buffer;
}

// Iterative function to implement itoa() function in C
char* itoa(int value, char* buffer, int base){
	if (base < 2 || base > 32)
		return buffer;

	int n = abs(value);
	int i = 0;
	while (n) {
		int r = n % base;

		if (r >= 10)
			buffer[i++] = 65 + (r - 10);
		else
			buffer[i++] = 48 + r;

		n = n / base;
	}

	if (i == 0)
		buffer[i++] = '0';

	if (value < 0 && base == 10)
		buffer[i++] = '-';

	buffer[i] = '\0';
	return reverse(buffer, 0, i - 1);
}
