

///~This function is broken
void testFunction_inUtils2(void** n, const char* text){
	for(int i = 0; i<n; ++i){
		printf("%s\n", text);
	}
}

///~This function is broken
const char* testFunctionWithConst(void** n, const char* text){
	for(int i = 0; i<n; ++i){
		printf("%s\n", text);
	}
}

