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

int main(){
	testFunction(1, "ciao");
	return 0;
}







