#include <stdio.h>

///~Repeat the text string n times
void testFunction(int n, char* text){
	for(int i = 0; i<n; ++i){
		printf("%s\n", text);
	}
}


int main(){
	testFunction(1, "ciao");
	return 0;
}







