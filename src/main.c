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
	if (argc<2){
		printf("Please specify a project root folder.\n");
		return 0;
	}

	Project* p = project_new("test", argv[1]);
	render_project_debug(p);
	render_project_org(p, "test_output.org");
	return 0;
}
