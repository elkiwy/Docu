#include "renderers.h"

////////////////////////////////////////////////////////////////////////
// Renderes


void render_argument_debug(Argument* a){
	printf("      arg type: \"%s\", arg name: \"%s\"\n", a->type, a->name);
}

void render_function_debug(Function* f){
	printf("    function name: \"%s\", return: \"%s\"\n", f->name, f->returnType);
	for (int i=0; i<f->args_count; ++i){
		render_argument_debug(f->args[i]);
	}
}

void render_module_debug(Module* m){
	printf("  module name: \"%s\"\n", m->name);
	for (int i=0; i<m->functions_count; ++i){
		render_function_debug(m->functions[i]);
	}
}

void render_project_debug(Project* p){
	printf("Rendering project %s\n", p->name);	
	for (int i=0; i<p->files_count; ++i){
		printf("file: %s\n", p->files[i]);
	}

	for (int i=0; i<p->modules_count; ++i){
		render_module_debug(p->modules[i]);
	}
}
