#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>

#include "data.h"
#include "renderers.h"
#include "minimap.h"
#include "languages.h"
#include "reader.h"






int main(int argc, char** argv){
	//Parse the input
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

	//Check required inputs
	if (input==NULL || mode==NULL){
		fprintf(stderr, "Please specify a input [-i rootFolder] and a mode [-m DEBUG/ORG/HTML]\n");
		exit(EXIT_FAILURE);
	}

	//Prepare all the languages
	Map* languages[128];
	for(int i=0;i<128; ++i){languages[i] = NULL;}
	parse_every_languages_config(languages);

	//Creat the project
	Project* p = project_new("test");

	//Read all the files
	read_project(input, languages, p);
	for(int i=0; i<128; ++i){
		if(languages[i] != NULL){
			map_free(languages[i]);
			languages[i] = NULL;
		}
	}

	//Render the project
	if(strcmp(mode, "DEBUG")==0){
		render_project_debug(p);
	}else if(strcmp(mode, "ORG")==0){
		if(output==NULL) output = "test_output.org";
		render_project_org(p, output);
	}else if(strcmp(mode, "HTML")==0){
		if(output==NULL) output = "test_output.html";
		render_project_html(p, output, style);
	}else{
		fprintf(stderr, "Invalid mode '%s' must be one of: DEBUG, ORG, or HTML.\n", mode);
		exit(EXIT_FAILURE);
	}

	project_free(p);

	return 0;
}
