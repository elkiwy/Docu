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


	printf("Function names: \n");
	for (int i=0; i<p->modules_count; ++i){
		Module* m = p->modules[i];
		for (int j=0; j<m->functions_count; ++j){
			Function* fun = m->functions[j];
			printf("%s ", fun->name);
		}
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

bool isALimiter(char c){
	int nc = (int)c;
	if((nc >= 48 && nc<=57) // 0-9
	   || (nc >= 65 && nc<=90) // A-Z
	   || nc == 95 // _
	   || nc == 45 // - 
	   || (nc >= 97 && nc<=122)){ // a-z
		return false;
	}
	return true;
}


char* highlightArguments(Function* fun){
	char* buff = malloc(sizeof(char)*1024);
	buff[0] = '\0';

	char* desc = fun->description;
	char* p = desc;
	int n = 1;
	int desc_size = strlen(desc);

	//cycle all description
	for(int i=1; i<desc_size; ++i){
		//check for all the args name
		for(int j=0; j<fun->args_count; ++j){
			char* aName = fun->args[j]->name;
			if (strncmp(desc+i, aName, strlen(aName)) == 0
			&& isALimiter(desc[i-1]) && isALimiter(desc[i+strlen(aName)])){
				//Found one arg, insert everything until here into buff
				strncat(buff, p, n);

				//Insert the span
				strcat(buff, "<span class='docu_arg_in_desc'>");
				strcat(buff, aName);
				strcat(buff, "</span>");

				i += strlen(aName);
				p = desc + i;
				n = 0;
			}
		}
		n++;
	}

	strncat(buff, p, n);

	return buff;
}


void render_function_html(FILE* f, Function* fun){
	fprintf(f, "<div id=\"%s\" class='docu_function'>", fun->name);
	fprintf(f, "<span class='docu_function_return'>%s</span>", fun->returnType);
	fprintf(f, "<span class='docu_function_name'>%s</span>", fun->name);
	char* highlightedDesc = highlightArguments(fun);
	fprintf(f, "<span class='docu_function_desc'>%s</span>", highlightedDesc);
	free(highlightedDesc);
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
	if(m->functions_count>0){
		fprintf(f, "<li><h2 id=\"%s\">%s</h2></li>", m->name, m->name);
		fprintf(f, "<ul>");
		for (int i=0; i<m->functions_count; ++i){
			fprintf(f, "<li>");
			render_function_html(f, m->functions[i]);
			fprintf(f, "</li>");
		}
		fprintf(f, "</ul>");
	}
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

		//Render index
		fputs("<div class='docu_index'>", f);
		fputs("<h1>Index:</h2>", f);
		fputs("<ul>", f);
		for (int i=0; i<p->modules_count; ++i){
			Module* m = p->modules[i];
			if(m->functions_count>0){
				fprintf(f, "<li><a class=\"docu_index_module\" href=\"#%s\">%s</a></li>", m->name, m->name);
				fputs("<ul>", f);
				for (int j=0; j<m->functions_count; ++j){
					Function* fun = m->functions[j];
					fprintf(f, "<li><a class=\"docu_index_func\" href=\"#%s\">%s</a></li>", fun->name, fun->name);
				}
				fputs("</ul>", f);
			}
		}
		fputs("</ul>", f);
		fputs("</div>", f);

		//Render main body
		fputs("<h1>Documentation:</h2>", f);
		fputs("<ul>", f);
		for (int i=0; i<p->modules_count; ++i){
			render_module_html(f, p->modules[i]);
		}
		fputs("</ul>", f);

	fputs("</div>", f);

	fputs("</main>", f);
	fputs("</body>", f);
	fputs("</html>", f);

	//Close file
	printf("Exported documentation to %s\n", path);
	fclose(f);
}

