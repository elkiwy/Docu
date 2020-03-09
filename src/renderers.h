#pragma once
#include <stdio.h>
#include <stdbool.h>
#include "data.h"

void render_project_debug(Project* p);
void render_project_org(Project* p, const char* path);
void render_project_html(Project* p, const char* path, const char* stylesheet);




