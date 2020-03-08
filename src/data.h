#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include "utils.h"

#define MAX_FILES 128
#define MAX_ARGS 16
#define MAX_FUNCTIONS 128
#define MAX_MODULES 128
#define DEFAULT_MODULE_NAME "default"

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

    int line;
    char* file;
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




Project* project_new(char* name, char* rootFolder);
Module* module_new(char* name);

void project_add_module(Project* p, Module* m);
Module* project_get_module(Project* p, char* name);

void module_add_function(Module* m, Function* f);
Function* module_get_function(Module* m, char* name);

void function_add_argument(Function* f, Argument* a);
Argument* function_get_argument(Function* f, char* name);
