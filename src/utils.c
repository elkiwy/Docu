
#include "utils.h"

char* formatString(char* format, int n, ...){
	va_list args;
	va_list args_copy;
	char* strings[n];
	int size = 0;
	va_start(args, n);
	va_copy(args_copy, args);
	for (int i=0; i<n; ++i){
		strings[i] = va_arg(args, char*);
		size += strlen(strings[i]);
	}
	size += strlen(format) + 1;
	char* buff = malloc(sizeof(char) * size);
	vsprintf(buff, format, args_copy);
	va_end(args);
	return buff;
}
