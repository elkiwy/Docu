#pragma once

#define MAX_FILES 128
#define MAX_ARGS 16
#define MAX_FUNCTIONS 128
#define MAX_MODULES 128
#define MAX_LINE_LENGTH 256

typedef struct Argument{
	char* name;
	char* type;
} Argument;

typedef struct Function{
	char* name;
	char* description;
	char* returnType;
    Argument* args[MAX_ARGS];
    int args_count;
} Function;

typedef struct Module{
	char* name;
	char* description;
    Function* functions[MAX_FUNCTIONS];
    int functions_count;
} Module;

typedef struct Project{
	char* name;
	char* root;
	char* files[MAX_FILES];
	int files_count;
	Module* modules[MAX_MODULES];
	int modules_count;
} Project;






