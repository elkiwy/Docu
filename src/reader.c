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
char* nextUsefulLine(FILE* f, int* lineCount){
	char* l = readline(f, lineCount);
	while(l!=NULL && strlen(trim(l))==0){
		l = readline(f, lineCount);
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
char* parse_data_override(FILE* f, int* lineCount, char** name, char** ret, char* args[]){
	char* l = nextUsefulLine(f, lineCount);
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
		l = nextUsefulLine(f, lineCount);
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
	char* l = readline(f, &lineCount);
	Module* m = project_get_module(p, "default");
	while(l!=NULL){
		//If is a module name
		if(checkDocstringPrefix(l, '=')){
			//Update the current module
			m = project_get_module(p, l+4);

		//If is a function description
		}else if(checkDocstringPrefix(l, '~')){
			char* description = strdup(l+4); 
			l = nextUsefulLine(f, &lineCount);
			Function* function = function_new(NULL, description, NULL);
			function->file = filename;
			function->line = lineCount;
			lang_fill_function_data(lang, l, function);
			if(strlen(function->name)>0){
				module_add_function(m, function);
			}
		}

		l = readline(f, &lineCount);
	}
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









