#ifndef STRINGS_H_
#define STRINGS_H_

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#define STRING_SIZE 256
#define LARGE_STRING_SIZE 1024
#define SMALL_STRING_SIZE 32

typedef struct SmallString{
	char value[SMALL_STRING_SIZE];

	operator char* () { 
	   return value; 
	}

	operator const char* () { 
	   return value; 
	}

}SmallString;

void String_log(SmallString);

void String_set(char *, const char *, int);

void String_append(char *, const char *);

void String_append_int(char *, int);

void String_append_float(char *, float);

void String_clearRange(char *, int, int);

#endif
