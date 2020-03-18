#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>

#include "minimap.h"
#include "languages.h"
#include "utils.h"
#include "data.h"






void read_project(char* rootFolder, Map** languages, Project* p);




