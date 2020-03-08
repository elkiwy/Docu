#pragma once
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_LINE_LENGTH 256

char* formatString(char* format, int n, ...);

int charsUntil(char* s, int n, ...);
int charsUntilLast(char* s, int n, ...);

char* trimLeft(char* s);
char* trimRight(char* s);
char* trim(char* s);
char* trimdup(char* s);
char* trimndup(char* s, int n);

char* readline(FILE* f, int* lineCount);
