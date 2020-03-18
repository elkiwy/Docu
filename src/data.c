#include "data.h"

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
Project* project_new(char* name){
	Project* p = malloc(sizeof(Project));
	p->name = name;
	p->files_count = 0;
	p->modules_count = 0;
	project_add_module(p, module_new(DEFAULT_MODULE_NAME));
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


