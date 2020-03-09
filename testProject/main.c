#include <stdio.h>

///~Repeat the text string n times
void testFunction(int n, char* text){
	for(int i = 0; i<n; ++i){
		printf("%s\n", text);
	}
}



///~This function has spaces between the docstring and declaration

	   
		

void testFunction(int n, char* text){
}


     ///~this function has whitespaces before the docstring
void testFunctionWhitespaces(int n){
}


///~No arguments
void testFunctionNoArgument(){
}

///~One arguments
void testFunctionSingleArgument(int n){
}

  
  
///~This docstring shouldn't work
asdjaklsdjasldjsal


///~This docstring shouldn't work
void asdjaklsdjasldjsal(



///~Test Docstring
///&overridenName
///#overriddenReturn
///@1char k
///@2char n
void asdjaklsdjasldjsal(int n){
}







int main(){
	testFunction(1, "ciao");
	return 0;
}







