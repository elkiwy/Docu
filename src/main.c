#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>

#include "utils.h"
#include "renderers.h"
#include "data.h"







////////////////////////////////////////////////////////////////////////
// Parser

void getFiles(Project* proj, const char* folder){
	DIR *dir;
    struct dirent* entry;
    if (!(dir = opendir(folder))) return;

	//While I read items
    while ((entry = readdir(dir)) != NULL) {
		//If it is a directory
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
				//Avoid unwanted entries
                continue;
			}else{
				//Get inside the directory
				char path[1024];
				sprintf(path, "%s/%s", folder, entry->d_name);
				getFiles(proj, path);
			}
		//If it is a normal file
        } else {
			//Add the file to the files list
			proj->files[proj->files_count] = formatString("%s/%s", 2, folder, entry->d_name);
			proj->files_count++;
        }
    }
    closedir(dir);
}

char* trimLeft(char* s){
	int i=0;
	while(s[i] == ' ' || s[i] == '\t'){i++;}
	return s+i;
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




char* readline(FILE* f){
	//Read it
	if (feof(f)){return NULL;}
	char* s = malloc(sizeof(char)*(MAX_LINE_LENGTH));
	if(fgets(s, MAX_LINE_LENGTH, f)==NULL){return NULL;}

	//Trim the result and remove the newline
	s = trimLeft(s);
	if (s[strlen(s)-1]=='\n') s[strlen(s)-1] = '\0';
	return s;
}

void readfile(Project* p, char* path){
	FILE* f = fopen(path, "r");
	if (f == NULL){
		printf("Failed to open %s for read\n", path);fflush(stdout);
		return;
	}else{
		char* l = readline(f);
		while(l!=NULL){
			//Trim the string
			if(strlen(l)>3 && l[0]=='/' && l[1]=='/' && l[2]=='/' && l[3]=='~'){
				//Parse the docstring
				//printf("\n\nFound docstring:\"%s\"\n", l);
				Function* fun = malloc(sizeof(Function));
				fun->args_count = 0;
				fun->description = strdup(l+4); //+4 removes the prefix

				//Parse the declaration
				l = readline(f);
				//printf("declaration line:\"%s\"\n", l);

				//Get return type
				int step = charsUntil(l, 2, ' ', '\t');
				fun->returnType = strndup(l, step);
				l += step + 1;

				//Get function name
				step = charsUntil(l, 3, ' ', '\t', '(');
				fun->name = strndup(l, step);
				l += step + 1;

				//Get arguments
				int i=0;
				int size=strlen(l);
				while(i<size && l[i] != ')'){
					Argument* a = malloc(sizeof(Argument));
					int step = charsUntil(l, 2, ',', ')');
					char* argString = strndup(l, step);

					//Get argument type
					int sep = charsUntilLast(argString, 2, ' ', '\t');
					a->type = trimLeft(strndup(l, sep));

					//Get argument name
					argString += sep;
					a->name = strdup(trimLeft(argString));

					fun->args[fun->args_count] = a;
					fun->args_count++;

					if (l[step]==')') break;
					l += step + 1;
					i = 0;
					size -= step;
				}

				render_function_debug(fun);
			}

			l = readline(f);
		}
	}
}


Project* project_new(char* name, char* rootFolder){
	Project* p = malloc(sizeof(Project));
	p->files_count = 0;
	p->root = rootFolder;
	p->name = name;
	getFiles(p, rootFolder);

	for (int i=0; i<p->files_count; ++i){
		readfile(p, p->files[i]);
	}

	return p;
}





int main(int argc, char** argv){
	if (argc<2){
		printf("Please specify a project root folder.\n");
		return 0;
	}

	Project* p = project_new("test", argv[1]);
	render_project_debug(p);
	return 0;
}
