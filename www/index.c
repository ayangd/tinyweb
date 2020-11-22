#include <malloc.h>

const char* hello = "hello";
void load(const char* request, char** contentPtr, int* lengthPtr) {
	*contentPtr = (char*)malloc(5);
	*lengthPtr = 0;
	while (hello[*lengthPtr] != 0) {
		(*contentPtr)[*lengthPtr] = hello[*lengthPtr];
		(*lengthPtr)++;
	}
}