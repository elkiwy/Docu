#include "renderers.h"

////////////////////////////////////////////////////////////////////////
// Debug


void render_argument_debug(Argument* a){
	printf("      arg type: \"%s\", arg name: \"%s\"\n", a->type, a->name);
}

void render_function_debug(Function* f){
	printf("    function name: \"%s\", return: \"%s\"\n", f->name, f->returnType);
	printf("    =>%s:%d\n", f->file, f->line);
	printf("    =>%s\n", f->description);
	for (int i=0; i<f->args_count; ++i){
		render_argument_debug(f->args[i]);
	}
	printf("\n");
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

////////////////////////////////////////////////////////////////////////
// Org


void render_argument_org(FILE* f, Argument* a){
	fprintf(f, "     - *%s* => /%s/\n", a->name, a->type);
}

void render_function_org(FILE* f, Function* fun){
	fprintf(f, "*** %s => %s\n", fun->name, fun->returnType);
	fprintf(f, "    %s\n", fun->description);
	fputs("    Arguments:\n", f);
	for (int i=0; i<fun->args_count; ++i){
		render_argument_org(f, fun->args[i]);
	}
}

void render_module_org(FILE* f, Module* m){
	fprintf(f, "** Module: %s\n", m->name);
	for (int i=0; i<m->functions_count; ++i){
		render_function_org(f, m->functions[i]);
	}
}


void render_project_org(Project* p, const char* path){
	//Create the file
	FILE* f = fopen(path, "w");
	if (f==NULL){printf("Couln't open file %s for org export\n", path);return;}

	fprintf(f, "* %s API Reference\n", p->name);

	for (int i=0; i<p->modules_count; ++i){
		render_module_org(f, p->modules[i]);
	}

	//Close file
	printf("Exported documentation to %s\n", path);
	fclose(f);
}

