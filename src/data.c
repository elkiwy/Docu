#include "data.h"
Argument* argument_new(char* type, char* name);
Function* function_new(char* name, char* desc, char* ret);
Module* module_new(char* name);
Project* project_new(char* name, char* rootFolder);


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


///~Checks if the current extension is one of the supported ones
bool checkSupportedExtension(const char* ext){
	const char* supported[] = {".c", ".h"};
	int size = sizeof(supported) / sizeof(supported[0]);
	for (int i=0; i<size; ++i){
		if (strcasecmp(ext, supported[i])==0) return true;
	}
	return false;
}


///~Cycle all the next lines and skip all the blank ones
char* nextUsefulLine(FILE* f, int* lineCount){
	char* l = readline(f, lineCount);
	while(l!=NULL && strlen(trim(l))==0){
		l = readline(f, lineCount);
	}
	return l;
}


///////////////////////////////////////////////////////////////////////////////////
///=Generic parsing

///~Read all the docstring lines for information overriding
char* parse_data_override(FILE* f, int* lineCount, char** name, char** ret, char* args[]){
	char* l = nextUsefulLine(f, lineCount);
	while(l!=NULL && strncmp(l, "///", 3)==0){
		//Override name
		if (strncmp(l, "///&", 4)==0){
			*name = strdup(l+4);	
			
		//Override return
		}else if (strncmp(l, "///#", 4)==0){
			*ret = strdup(l+4);	

		//Override Args
		}else if (strncmp(l, "///@", 4)==0){
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



///////////////////////////////////////////////////////////////////////////////////
///=C Code parsing

///~Get the function return type from C declaration
char* get_function_return_c(char* line){
	int step = charsUntil(line, 1, '(');
	char* returnAndName = trimndup(line, step);
	int sep = charsUntilLast(returnAndName, 2, ' ', '\t');
	char* ret = trimndup(returnAndName, sep);
	free(returnAndName);
	return ret;
}


///~Get the function name from C declaration
char* get_function_name_c(char* line){
	int step = charsUntil(line, 1, '(');
	char* returnAndName = trimndup(line, step);
	int sep = charsUntilLast(returnAndName, 2, ' ', '\t');
	char* name = trimdup(returnAndName + sep);
	free(returnAndName);
	return name;
}


///~Get the function argument at index from C declaration
Argument* get_function_argument_c(char* line, int index){
	int step = charsUntil(line, 1, '(');
	char* args = line + step + 1;
	int i = 0;
	while(i<index){
		step = charsUntil(args, 1, ',');
		if (step==0) return NULL;
		args += (step + 1);
		i++;
	}

	step = charsUntil(args, 2, ',', ')');
	char* arg = strndup(args, step); 
	if (strlen(arg)==0)return NULL;
	printf("=arg '%s'\n", arg);fflush(stdout);
	int sep = charsUntilLast(arg, 2, ' ', '\t');
	char* a_type = trimndup(arg, sep);
	char* a_name = trimdup(arg + sep);
	return argument_new(a_type, a_name);
}


///~Override a single argument from its argString
void override_arg_c(Argument* a, char* argString){
	int sep = charsUntilLast(argString, 2, ' ', '\t');
	char* a_type = trimndup(argString, sep);
	char* a_name = trimdup(argString + sep);
	a->type = a_type;
	a->name = a_name;
}


///~Parse a docstring from a C file
void parse_docstring_c(Module* m, FILE* f, char* l, int* lineCount, char* filename){
	//Prepare for information overriding
	char* override_name   = NULL;
	char* override_return = NULL;
	char* override_args[MAX_ARGS];
	for (int i=0; i<MAX_ARGS; ++i){override_args[i] = NULL;}

	//Parse the docstring
	char* fun_desc = strdup(l+4); //+4 removes the prefix
	l = parse_data_override(f, lineCount, &override_name, &override_return, override_args);

	//Get return and name
	char* fun_ret = get_function_return_c(l);
	char* fun_name = get_function_name_c(l);
	if (strlen(fun_ret)==0 || strlen(fun_name)==0){return;}
	if (override_name!=NULL)fun_name = override_name;
	if (override_return!=NULL)fun_ret = override_return;
	Function* fun = function_new(fun_name, fun_desc, fun_ret);
	fun->line = *lineCount;
	fun->file = strdup(filename);
	module_add_function(m, fun);

	//Get arguments
	int ind =0;
	Argument* a = get_function_argument_c(l, ind);
	while(a!=NULL){
		if (override_args[ind]!=NULL){ override_arg_c(a, override_args[ind]);}
		function_add_argument(fun, a);
		ind++;
		a = get_function_argument_c(l, ind);
	}

	//Keep adding arguments if overriding
	while(override_args[ind]!=NULL){
		//Get argument type
		a = argument_new("", "");
		if (override_args[ind]!=NULL){ override_arg_c(a, override_args[ind]);}
		function_add_argument(fun, a);
		ind++;
	}
}



///////////////////////////////////////////////////////////////////////////////////
///=File reading

///~Reads a file and parse it creating all the documentation from his docstrings
void readfile(Project* p, char* path){
	//Check if the file extension is one of the supported ones
	char* fileExt = path + charsUntilLast(path, 1, '.');
	if(!checkSupportedExtension(fileExt)){
		printf("Skipping %s, extension not supported\n", path);
		return;
	}

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
		if(strlen(l)>3 && strncmp(l, "///=", 4)==0){
			//Update the current module
			m = project_get_module(p, l+4);

		//If is a function description
		}else if(strlen(l)>3 && strncmp(l, "///~", 4)==0){
			if(strcmp(fileExt, ".c") || strcmp(fileExt, ".h")){
				parse_docstring_c(m, f, l, &lineCount, filename);
			}else{
				//Other files
			}
		}

		l = readline(f, &lineCount);
	}
}



///////////////////////////////////////////////////////////////////////////////////
///=Data creation and manipulation

///~Initialize a new argument
Argument* argument_new(char* type, char* name){
	Argument* a = malloc(sizeof(Argument));
	a->type = type;
	a->name = name;
	return a;
}

///~Initialize a new function
Function* function_new(char* name, char* desc, char* ret){
	Function* f = malloc(sizeof(Function));
	f->args_count = 0;
	f->name = name;
	f->description = desc;
	f->returnType = ret;
	f->line = 0;
	f->file = "";
	return f;
}

///~Initialize a new module
Module* module_new(char* name){
	Module* m = malloc(sizeof(Module));
	m->name = name;
	m->functions_count = 0;
	return m;
}

///~Initialize a new Project
Project* project_new(char* name, char* rootFolder){
	//Initialize
	Project* p = malloc(sizeof(Project));
	p->files_count = 0;
	p->root = rootFolder;
	p->name = name;
	p->modules_count = 0;

	//Add default module
	project_add_module(p, module_new(DEFAULT_MODULE_NAME));

	//Add and parse files
	getFiles(p, rootFolder);
	for (int i=0; i<p->files_count; ++i){
		readfile(p, p->files[i]);
	}

	return p;
}

///~Add a module to a project
void project_add_module(Project* p, Module* m){
	p->modules[p->modules_count] = m;
	p->modules_count++;
}

///~Get a module from a project or create a new one if not found
Module* project_get_module(Project* p, char* name){
	for (int i=0; i<p->modules_count; ++i){
		if(strcmp(p->modules[i]->name, name) == 0){
			return p->modules[i];
		}
	}
	Module* newModule = module_new(name);
	project_add_module(p, newModule);
	return newModule;
}

///~Add a function to a module
void module_add_function(Module* m, Function* f){
	m->functions[m->functions_count] = f;
	m->functions_count++;
}

///~Get a function from a module
Function* module_get_function(Module* m, char* name){
	for (int i=0; i<m->functions_count; ++i){
		if(strcmp(m->functions[i]->name, name) == 0){
			return m->functions[i];
		}
	}
	return NULL;
}

///~Add a argument to a function
void function_add_argument(Function* f, Argument* a){
	f->args[f->args_count] = a;
	f->args_count++;
}

///~Get an argument from a function
Argument* function_get_argument(Function* f, char* name){
	for (int i=0; i<f->args_count; ++i){
		if(strcmp(f->args[i]->name, name) == 0){
			return f->args[i];
		}
	}
	return NULL;
}




