#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>

#include "data.h"
#include "renderers.h"






int main(int argc, char** argv){

	char* input  = NULL;
	char* output = NULL;
	char* style  = NULL;
	char* mode   = NULL;

	for (int i = 1; i < argc ; i++) {
		if (argv[i][0] == '-'){
			switch (argv[i][1]) {
			case 'o': output = argv[i+1]; break;
			case 'i': input = argv[i+1]; break;
			case 's': style = argv[i+1]; break;
			case 'm': mode = argv[i+1]; break;
			default:
				fprintf(stderr, "Usage: %s [-i projectRootFolder] [-m DEBUG/ORG/HTML] [-o outputFile] [-s cssFile]\n", argv[0]);
				exit(EXIT_FAILURE);
			}
		}
	}   

	if (input==NULL || mode==NULL){
		fprintf(stderr, "Please specify a input [-i rootFolder] and a mode [-m DEBUG/ORG/HTML]\n");
		exit(EXIT_FAILURE);
	}


	Project* p = project_new("test", input);
	if(strcmp(mode, "DEBUG")==0){
		render_project_debug(p);
	}else if(strcmp(mode, "ORG")==0){
		render_project_org(p, "test_output.org");
	}else if(strcmp(mode, "HTML")==0){
		render_project_html(p, "test_output.html", style);
	}else{
		fprintf(stderr, "Invalid mode '%s' must be one of: DEBUG, ORG, or HTML.\n", mode);
		exit(EXIT_FAILURE);
	}
	return 0;
}
