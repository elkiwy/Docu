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



///~Count how many character from the start of s to the first limit
int charsUntil(char* s, int n, ...){
	//Extracts the limits
	va_list args;
	char limits[n];
	va_start(args, n);
	for (int i=0; i<n; ++i){
		limits[i] = va_arg(args, int);
	}
	va_end(args);

	//Cycle the string until you find a limit or you reach the end
	int i=0;
	int size = strlen(s);
	while(i<size){
		for(int j=0; j<n; ++j){
			if (s[i] == limits[j]) return i;
		}
		i++;
	}
	return 0;
}


///~Count how many character from the start of s to the last limit
int charsUntilLast(char* s, int n, ...){
	//Extracts the limits
	va_list args;
	char limits[n];
	va_start(args, n);
	for (int i=0; i<n; ++i){
		limits[i] = va_arg(args, int);
	}
	va_end(args);

	//Cycle the string until you find a limit or you reach the end
	int size = strlen(s);
	int i=size-1;
	while(i>=0){
		for(int j=0; j<n; ++j){
			if (s[i] == limits[j]) return i;
		}
		i--;
	}
	return 0;
}



char* trimLeft(char* s){
	int i=0;
	while(s[i] == ' ' || s[i] == '\t'){i++;}
	return s+i;
}

char* trimRight(char* s){
	int i = strlen(s)-1;
	while(i>=0 && (s[i] == ' ' || s[i] == '\t' || s[i] == '\n')){
		s[i] = '\0';
		i--;
	}
	return s;
}

char* trimdup(char* s){
	char* news = strdup(s);
	return trimLeft(trimRight(news));
}

char* trimndup(char* s, int n){
	char* news = strndup(s, n);
	return trimLeft(trimRight(news));
}

char* trim(char* s){
	return trimLeft(trimRight(s));
}




char* readline(FILE* f){
	//Read it
	if (feof(f)){return NULL;}
	char* s = malloc(sizeof(char)*(MAX_LINE_LENGTH));
	if(fgets(s, MAX_LINE_LENGTH, f)==NULL){return NULL;}

	//Trim the result and remove the newline
	return trim(s);
}








