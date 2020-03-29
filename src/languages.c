#include "languages.h"

///=Languages handling

int getGroupNumber(Map* m, char* key){
	char* groupStr = map_get(m, key);
	//printf("group number '%s' '%s'\n", groupStr, key);fflush(stdout);
	int c = (int)groupStr[0];
	int zero = '0';
	return c - zero;
}


///~Extract the function name from a function declaration src. Outputs it into result buffer.
void lang_get_function_name(Map* lang, const char* src, char* result){
	char* regex = map_get(lang, KEY_FUNC_NAME);
	int group = getGroupNumber(lang, KEY_FUNC_NAME_G);
	regex_get(src, regex, result, group);
}

///~Extract the function return type from a function declaration src. Outputs it into result buffer.
void lang_get_function_returnType(Map* lang, const char* src, char* result){
	char* regex = map_get(lang, KEY_FUNC_RETURN);
	int group = getGroupNumber(lang, KEY_FUNC_RETURN_G);
	regex_get(src, regex, result, group);
}

///~Extract the function args from a function declaration src. Outputs it into result struct.
void lang_get_function_args(Map* lang, const char* src, Args* result){
	//Initialize counter
	result->count = 0;

	//Extract the arg string
	char* regex = map_get(lang, KEY_FUNC_ARGS);
	int group = getGroupNumber(lang, KEY_FUNC_ARGS_G);
	char args[1024];
	regex_get(src, regex, args, group);

	//Get the regex for arguments
	char* regex_arg_name  = map_get(lang, KEY_FUNC_ARG_NAME);
	char* regex_arg_type  = map_get(lang, KEY_FUNC_ARG_TYPE);
	char* separatorStr    = map_get(lang, KEY_FUNC_ARG_SEP);
	char separator        = separatorStr[0];

	//Read all the arg string and extract arguments
	int i = 0;
	int pos = 0;
	char arg[256] = "";
	char c = ' ';
	while(c!='\0'){
		//Read next char
		c = args[pos];

		//If it's a separator or I reached the end
		if (c == separator || c=='\0'){
			//Parse the current arg
			char aName[256];
			char aType[256];
			regex_get(arg, regex_arg_name, aName, 1);
			regex_get(arg, regex_arg_type, aType, 1);
			if (strlen(trim(aName))>0){
				result->types[result->count] = strdup(trim(aType));
				result->names[result->count] = strdup(trim(aName));
				result->count += 1;
			}
			
			//...and reset for the next
			i = 0;
			for(int j=0;j<256;++j){arg[j]=0;}
		//Else I'm still reading charactrs
		}else{
			//Fill the temp arg
			arg[i] = c;
			i++;
		}
		pos++; 
	}
}

///~Extract all the information from a declaration src and fill the function struct.
void lang_fill_function_data(Map* lang, const char* src, Function* function){
	//Create a temp buffer
	char buff[1024];
	Args a;

	//Read the return and put it into the function
	lang_get_function_returnType(lang, src, buff);
	function->returnType = strdup(buff); 

	//Read the name and put it into the function
	lang_get_function_name(lang, src, buff);
	function->name = strdup(buff); 

	//Read the arguments and put them into the function
	lang_get_function_args(lang, src, &a);
	for (int i=0; i<a.count; i++){
		Argument* arg = argument_new(a.types[i], a.names[i]);
		function_add_argument(function, arg);
	}
}

///~Retrive the first extension from a extensions list comma separated. Return the position of the next extension
int getExtension(char* extList, char* ext){
	int i=0;
	while(extList[i]!='\0'){
		//If i reached the end of the extension close it and return the pos
		if(extList[i]==','){
			ext[i] = '\0';
			return i+1;
		}
		//Else keep adding characters
		ext[i] = extList[i];
		i++;
	}
	//I reached the last extension in the list
	ext[i] = '\0';
	return i;
}

///~Selects the right language for the filename
Map* lang_get_language_for_file(Map** languages, char* filename){
	//Grab the extension
	char* fileExt = filename + charsUntilLast(filename, 1, '.') + 1;

	//Cycle all the languages
	int i=0;
	Map* lang = languages[i];
	while(lang!=NULL){
		//Get the supported extensions for this language
		char* supportedList = map_get(lang, KEY_EXTENSIONS);

		//Cycle the supported extensions
		char ext[32];
		int pos = getExtension(supportedList, ext);
		while(pos != 0){
			//If i found it
			if(strcasecmp(trim(ext), fileExt)==0){
				//Return
				return lang;
			}

			//Else keep searching other extensions
			supportedList += pos;
			pos = getExtension(supportedList, ext);
		}

		//Keep searching in other languages
		i++;
		lang = languages[i];
	}
	return NULL;
}

///~Reads a couple of key and val from a file f. Returns 1 if success, 0 if nothing found.
int readKeyVal(FILE* f, char* key, char* val){
	if (feof(f)){return 0;}
	char c = fgetc(f);
	int i = 0;
	while(c!='='){
		if (c=='\0' || feof(f)){return 0;}
		key[i] = c;
		c = fgetc(f);
		i++;
	}
	key[i] = '\0';

	if (c!='='){return 0;}
	c = fgetc(f);

	i = 0;
	while(c!='\0' && c!='\n'){
		val[i] = c;
		c = fgetc(f);
		i++;
	}
	val[i] = '\0';
	return 1;
}

///~Extract the language configuration from the file at path
Map* parse_lang_config(const char* path){
	printf("Parsing lang config file : '%s'\n", path);

	//Init the map and expected keys
	Map* lang = map_new();
	const char* keys[] = {KEY_EXTENSIONS, KEY_FUNC_NAME, KEY_FUNC_RETURN, KEY_FUNC_ARGS, KEY_FUNC_ARG_SEP, KEY_FUNC_ARG_NAME, KEY_FUNC_ARG_TYPE, KEY_FUNC_NAME_G, KEY_FUNC_RETURN_G, KEY_FUNC_ARGS_G };
	int nkeys = sizeof(keys)/sizeof(keys[0]);

	//Retrieve data from the config file
	FILE* f = fopen(path, "r");
	rewind(f);
	char keyArr[256]; char valArr[256];
	char* key = &keyArr[0]; char* val = &valArr[0];
	int n = readKeyVal(f, key, val);
	key = trim(key); val = trim(val);

	while(n > 0){
		//Check if the key read is valid
		bool found = false;
		for (int i=0; i<nkeys; ++i){
			if(strcmp(keys[i], key)==0){found = true; break;}
		}
		if (found==false){
			printf("Key '%s' in file '%s' is invalid.\n", key, path);
			exit(1);
		}

		//Insert into the map
		val = val+1;
		val[strlen(val)-1] = '\0';
		map_insert(lang, strdup(key), strdup(val));

		//Continue the cycle
		n = readKeyVal(f, key, val);
		key = trim(key); val = trim(val);
	}
	fclose(f);
	return lang;
}

///~Extract all the languages configuration into the language array languagesBuffer.
void parse_every_languages_config(Map** languagesBuffer){
	DIR *dir;
    struct dirent* entry;
	const char* folder = "lang";
    if (!(dir = opendir(folder))){
		printf("No /lang folder!\n");	
		exit(1);
	}

	//While I read items
	int i = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_DIR) {
			//Parse the config file
			char* filename = formatString("%s/%s", 2, folder, entry->d_name);
			languagesBuffer[i] = parse_lang_config(filename);
			i++;
			free(filename);
        }
    }
    closedir(dir);
}



