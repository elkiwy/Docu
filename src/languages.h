#pragma once

#include "minimap.h"
#include "data.h"

#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include "utils.h"


#define KEY_EXTENSIONS		"extensions"
#define KEY_FUNC_NAME		"functionName"
#define KEY_FUNC_NAME_G     "functionNameGroup"
#define KEY_FUNC_RETURN		"returnType"
#define KEY_FUNC_RETURN_G	"returnTypeGroup"
#define KEY_FUNC_ARGS		"args"
#define KEY_FUNC_ARGS_G		"argsGroup"
#define KEY_FUNC_ARG_SEP	"argSeparator"
#define KEY_FUNC_ARG_TYPE	"argType"
#define KEY_FUNC_ARG_NAME	"argName"



extern const char _binary_lang_c_doculang_start[];
extern const char _binary_lang_c_doculang_end[];
extern const char _binary_lang_gisp_doculang_start[];
extern const char _binary_lang_gisp_doculang_end[];


typedef struct LinkedFile{
	char* start;
	char* end;
	char* name;
} LinkedFile;

typedef struct Args{
	char* types[32];
	char* names[32];
	int count;
} Args;


void parse_every_languages_config(Map** languagesBuffer);


void lang_get_function_name(Map* lang, const char* src, char* result);
void lang_get_function_returnType(Map* lang, const char* src, char* result);
void lang_get_function_args(Map* lang, const char* src, Args* result);

void lang_fill_function_data(Map* lang, const char* src, Function* function);

Map* lang_get_language_for_file(Map** languages, char* filename);
