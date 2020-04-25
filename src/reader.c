#include "reader.h"



///=File reader


///~Retrive all the files from a root directory and fill them into the project
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


///~Cycle all the next lines and skip all the blank ones
char* nextUsefulLine(char* linebuffer, FILE* f, int* lineCount){
	char* l = readline(linebuffer, f, lineCount);
	while(l!=NULL && strlen(trim(l))==0){
		l = readline(linebuffer, f, lineCount);
	}
	return l;
}



///~Checks if the first 4 symbols of line matches with the docstring with symbol
bool checkDocstringPrefix(const char* line, const char symbol){
	if(line==NULL){return false;}
	if(strlen(line)<4){return false;}
	char* prefixes[2];
	char p1[5] = {'/', '/', '/', '0', '\0'};
	char p2[5] = {';', ';', ';', '0', '\0'};
	prefixes[0] = p1;
	prefixes[1] = p2;
	int n = sizeof(prefixes)/sizeof(prefixes[0]);

	for(int i=0; i<n; ++i){
		//Adjust prefix to check the requested symbol
		prefixes[i][3] = symbol;

		//Check if match
		if(strncmp(line, prefixes[i], 4)==0) return true;
	}
	return false;
}


///~Read all the docstring lines for information overriding
char* parse_data_override(char* linebuffer, FILE* f, int* lineCount, char** name, char** ret, char* args[]){
	char* l = nextUsefulLine(linebuffer, f, lineCount);
	while(l!=NULL && (strncmp(l, "///", 3)==0 || strncmp(l, ";;;", 3)==0)){
		//Override name
		if (checkDocstringPrefix(l, '&')){
			*name = strdup(l+4);	
			
		//Override return
		}else if (checkDocstringPrefix(l, '#')){
			*ret = strdup(l+4);	

		//Override Args
		}else if (checkDocstringPrefix(l, '@')){
			//Convert one digit char into a number
			if (l[4]<48 || l[4]>57){
				printf("[!!!] Invalid argument overriding docstirng: \"%s\"", l);
			}else{
				int index = (int)l[4] - 48;
				args[index-1] = strdup(l+5);
			}
		//Unknown docstring
		}else{
			printf("[!] Unknown docstring : \"%s\"", l);
		}
		l = nextUsefulLine(linebuffer, f, lineCount);
	}
	return l;
}





///~Reads a file and parse it creating all the documentation from his docstrings
void readfile(Project* p, Map* lang, char* path){
	printf("Reading file %s\n", path);fflush(stdout);

	//Try to open the file
	FILE* f = fopen(path, "r");
	if (f == NULL){
		printf("Failed to open %s for read\n", path);fflush(stdout);
		return;
	}

	//Read it line by line
	int lineCount = 0;
	char* filename = path;// + charsUntilLast(path, 1, '/');

	char* lbuff = malloc(sizeof(char)*(MAX_LINE_LENGTH));
	char* l = readline(lbuff, f, &lineCount);
	Module* m = project_get_module(p, "default");
	while(l!=NULL){
		//If is a module name
		if(checkDocstringPrefix(l, '=')){
			//Update the current module
			m = project_get_module(p, l+4);

		//If is a function description with ONLY the docstrings
		}else if(checkDocstringPrefix(l, '+')){
			char* description = strdup(l+4); 

			//Loop through the overrides
			char* ret = NULL;
			char* name = NULL;
			char* args_name[32] = {NULL};
			char* args_type[32] = {NULL};
			int definition = 0;
			while(definition == 0){
				l = nextUsefulLine(lbuff, f, &lineCount);

				//Return type
				if (checkDocstringPrefix(l, '#')){
					ret = strdup(l+4);
				//Name
				}else if (checkDocstringPrefix(l, '&')){
					name = strdup(l+4);
				//argument name
				}else if (checkDocstringPrefix(l, '@')){
					int n = (int)l[4] - (int)'0' - 1;
					args_name[n] = strdup(l+5);
				//argument type
				}else if (checkDocstringPrefix(l, '!')){
					int n = (int)l[4] - (int)'0' - 1;
					args_type[n] = strdup(l+5);
				}else{
					definition = 1;
				}
			}

			printf("definition: %s\n", l);

			//Fill normal data
			Function* function = function_new(name, description, ret);
			function->file = filename;
			function->line = lineCount;

			//Replace overrides
			for(int i=0;i<32;++i){
				//Check if i have to create a new argument
				if((args_name[i] != NULL || args_type[i] != NULL)){
					Argument* arg = argument_new(args_type[i], args_name[i]);
					function_add_argument(function, arg);
				}
			}

			if(strlen(function->name)>0){
				module_add_function(m, function);
			}else{
				function_free(function);
			}

		//If is a function description
		}else if(checkDocstringPrefix(l, '~')){
			char* description = strdup(l+4); 

			//Loop through the overrides
			char* over_return = NULL;
			char* over_name = NULL;
			char* over_args_name[32] = {NULL};
			char* over_args_type[32] = {NULL};
			int definition = 0;
			while(definition == 0){
				l = nextUsefulLine(lbuff, f, &lineCount);

				//Return type
				if (checkDocstringPrefix(l, '#')){
					over_return = strdup(l+4);
				//Name
				}else if (checkDocstringPrefix(l, '&')){
					over_name = strdup(l+4);
				//argument name
				}else if (checkDocstringPrefix(l, '@')){
					int n = (int)l[4] - (int)'0' - 1;
					over_args_name[n] = strdup(l+5);
				//argument type
				}else if (checkDocstringPrefix(l, '!')){
					int n = (int)l[4] - (int)'0' - 1;
					over_args_type[n] = strdup(l+5);
				}else{
					definition = 1;
				}
			}
			printf("definition: %s\n", l);

			//Fill normal data
			Function* function = function_new(NULL, description, NULL);
			function->file = filename;
			function->line = lineCount;
			lang_fill_function_data(lang, l, function);

			//Replace overrides
			if(over_return){
				free(function->returnType);
				function->returnType = over_return;
			}
			if(over_name){
				free(function->name);
				function->name = over_name;
			}
			for(int i=0;i<32;++i){
				//Check if i have to create a new argument
				if((over_args_name[i] != NULL || over_args_type[i] != NULL) && function->args_count <= i){
					Argument* arg = argument_new(over_args_type[i], over_args_name[i]);
					function_add_argument(function, arg);
				//Else just override the data
				}else{
					if(over_args_name[i] != NULL){
						function->args[i]->name = strdup(over_args_name[i]);
						if(function->args_count<=i)function->args_count = i+1;
					}
					if(over_args_type[i] != NULL){
						function->args[i]->type = strdup(over_args_type[i]);
						if(function->args_count<=i)function->args_count = i+1;
					}
				}
			}


			if(strlen(function->name)>0){
				module_add_function(m, function);
			}else{
				function_free(function);
			}
		}

		l = readline(lbuff, f, &lineCount);
	}
	free(lbuff);
}


///~Read a whole project from its rootFolder and extract the info into the Project p.
void read_project(char* rootFolder, Map** languages, Project* p){
	p->root = rootFolder;

	//Add and parse files
	getFiles(p, rootFolder);
	for (int i=0; i<p->files_count; ++i){
		Map* lang = lang_get_language_for_file(languages, p->files[i]);
		if(lang!=NULL){readfile(p, lang, p->files[i]);}
	}
}









