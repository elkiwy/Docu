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



////////////////////////////////////////////////////////////////////////
// HTML

void render_argument_html(FILE* f, Argument* a){
	fprintf(f, "<div class='docu_argument'>");
	fprintf(f, "<span class='docu_argument_type'>%s</span>", a->type);
	fprintf(f, "<span class='docu_argument_separator'></span>");
	fprintf(f, "<span class='docu_argument_name'>%s</span>", a->name);
	fprintf(f, "</div>");
}

void render_function_html(FILE* f, Function* fun){
	fprintf(f, "<div class='docu_function'>");
	fprintf(f, "<span class='docu_function_return'>%s</span>", fun->returnType);
	fprintf(f, "<span class='docu_function_name'>%s</span>", fun->name);
	fprintf(f, "<span class='docu_function_desc'>%s</span>", fun->description);
	fprintf(f, "<ul>");
	for (int i=0; i<fun->args_count; ++i){
		fprintf(f, "<li>");
		render_argument_html(f, fun->args[i]);
		fprintf(f, "</li>");
	}
	fprintf(f, "</ul>");
	fprintf(f, "</div>");
}

void render_module_html(FILE* f, Module* m){
	fprintf(f, "<h2>Module: %s</h2>", m->name);
	fprintf(f, "<ul>");
	for (int i=0; i<m->functions_count; ++i){
		fprintf(f, "<li>");
		render_function_html(f, m->functions[i]);
		fprintf(f, "</li>");
	}
	fprintf(f, "</ul>");
}

void render_css(FILE* f, const char* stylesheet){
	FILE* fp = fopen(stylesheet, "r");	
	if (fp==NULL){printf("Couln't open style file %s\n", stylesheet);return;}


	int c = fgetc(fp);
	while(c!=EOF){
		fputc(c, f);
		c = fgetc(fp);
	}

	fclose(fp);
}

void render_project_html(Project* p, const char* path, const char* stylesheet){
	//Create the file
	FILE* f = fopen(path, "w");
	if (f==NULL){printf("Couln't open file %s for html export\n", path);return;}

	fputs("<!DOCTYPE html><html lang='en'>", f);
	fputs("<head>", f);
	fputs("<style>", f);
	render_css(f, stylesheet);
	fputs("</style>", f);
	fputs("</head>", f);
	fputs("<body>", f);
	fputs("<main>", f);

	fputs("<div class='docu_main'>", f);
	fprintf(f, "<h1>%s API Reference</h1>", p->name);
	for (int i=0; i<p->modules_count; ++i){
		render_module_html(f, p->modules[i]);
	}
	fputs("</div>", f);

	fputs("</main>", f);
	fputs("</body>", f);
	fputs("</html>", f);

	//Close file
	printf("Exported documentation to %s\n", path);
	fclose(f);
}

