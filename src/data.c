#include "data.h"


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



Function* function_new(char* name, char* desc, char* ret){
	Function* f = malloc(sizeof(Function));
	f->args_count = 0;
	f->name = name;
	f->description = desc;
	f->returnType = ret;
	return f;
}

Argument* argument_new(char* type, char* name){
	Argument* a = malloc(sizeof(Argument));
	a->type = type;
	a->name = name;
	return a;
}


bool checkSupportedExtension(const char* ext){
	const char* supported[] = {".c", ".h"};
	int size = sizeof(supported) / sizeof(supported[0]);
	for (int i=0; i<size; ++i){
		if (strcasecmp(ext, supported[i])==0){
			return true;
		}
	}
	return false;
}


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
	char* l = readline(f);
	Module* m = project_get_module(p, "default");
	while(l!=NULL){
		//If is a module name
		if(strlen(l)>3 && l[0]=='/' && l[1]=='/' && l[2]=='/' && l[3]=='='){
			//Update the current module
			m = project_get_module(p, l+4);

		//If is a function description
		}else if(strlen(l)>3 && l[0]=='/' && l[1]=='/' && l[2]=='/' && l[3]=='~'){
			//Parse the docstring
			char* fun_desc = strdup(l+4); //+4 removes the prefix

			//Readline until you find something
			l = readline(f);
			while(l!=NULL && strlen(trim(l))==0){
				l = readline(f);}
			char* fullLine = l;

			//Get return and name
			int step = charsUntil(l, 1, '(');
			if (step==0){
				printf("[!!!] Skipping docstring \"%s\" and declaration \"%s\" because is not a valid.\n", fun_desc, fullLine);fflush(stdout);
				continue;
			}

			char* returnAndName = trimndup(l, step);
			int sep = charsUntilLast(returnAndName, 2, ' ', '\t');
			char* fun_ret = trimndup(returnAndName, sep);
			char* fun_name = trimdup(returnAndName + sep);
			l += step + 1;

			//Check if arguments are properly declared
			int closingArgStep = charsUntil(l, 1, ')');
			if (closingArgStep==0 && ((strlen(l)>0 && l[0] != ')') || strlen(l)==0)) {
				printf("[!!!] Skipping docstring \"%s\" and declaration \"%s\" because is not a valid.\n", fun_desc, fullLine);fflush(stdout);
				continue;
			}

			//Function declaration should be valid here, so create the object
			Function* fun = function_new(fun_name, fun_desc, fun_ret);
			module_add_function(m, fun);

			//Get arguments
			int i=0;
			int size=strlen(l);
			while(i<size && l[i] != ')'){
				int step = charsUntil(l, 2, ',', ')');
				char* argString = trimndup(l, step);

				//Get argument type
				int sep = charsUntilLast(argString, 2, ' ', '\t');
				char* a_type = trimndup(l, sep);

				//Get argument name
				argString += sep;
				char* a_name = trimdup(argString);

				//Add the argument to the function
				Argument* a = argument_new(a_type, a_name);
				function_add_argument(fun, a);

				if (l[step]==')') break;
				l += step + 1;
				i = 0;
				size -= step;
			}


		}

		l = readline(f);
	}
}


Module* module_new(char* name){
	Module* m = malloc(sizeof(Module));
	m->name = name;
	m->functions_count = 0;
	return m;
}


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




void project_add_module(Project* p, Module* m){
	p->modules[p->modules_count] = m;
	p->modules_count++;
}


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

void module_add_function(Module* m, Function* f){
	m->functions[m->functions_count] = f;
	m->functions_count++;
}

Function* module_get_function(Module* m, char* name){
	for (int i=0; i<m->functions_count; ++i){
		if(strcmp(m->functions[i]->name, name) == 0){
			return m->functions[i];
		}
	}
	return NULL;
}

void function_add_argument(Function* f, Argument* a){
	f->args[f->args_count] = a;
	f->args_count++;
}

Argument* function_get_argument(Function* f, char* name){
	for (int i=0; i<f->args_count; ++i){
		if(strcmp(f->args[i]->name, name) == 0){
			return f->args[i];
		}
	}
	return NULL;
}




