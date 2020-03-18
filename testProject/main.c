#include <stdio.h>

///~1 Repeat the text string n times
void testFunction(int n, char* text){
	for(int i = 0; i<n; ++i){
		printf("%s\n", text);
	}
}



///~2 This function has spaces between the docstring and declaration

	   
		

void testFunction(int n, char* text){
}


     ///~3 this function has whitespaces before the docstring
void testFunctionWhitespaces(int n){
}


///~4 No arguments
void testFunctionNoArgument(){
}

///~5 One arguments
void testFunctionSingleArgument(int n){
}

  
  
///~This docstring shouldn't work
asdjaklsdjasldjsal


///~6 This docstring shouldn't work
void asdjaklsdjasldjsal(



///~7 Test Docstring
void asdjaklsdjasldjsal(int n){
}







int main(){
	testFunction(1, "ciao");
	return 0;
}







