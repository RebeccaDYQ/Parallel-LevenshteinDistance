#include <stdio.h>
#include <stdlib.h>
#include <string.h>

inline int max ( int a, int b ) { return a > b ? a : b; }
inline int min ( int a, int b ) { return a < b ? a : b; }

int LevenshteinDistance(char* str1, char* str2, int len1, int len2, int ** D) {
	int i, j;

	for (i = 0; i < len1; i++) {
		D[i][0] = i;
	}
	for (j = 0; j < len2; j++) {
		D[0][j] = j;
	}
	
	for (i = 1; i < len1; i++) {
		for (j = 1; j < len2; j++) {
			int add_one = 1;
			if(str1[i-1] == str2[j-1]) {
				add_one = 0;
			}
			D[i][j] = min( min(D[i-1][j]+1, D[i][j-1]+1), D[i-1][j-1] + add_one);
		}
	}
	
	return D[len1-1][len2-1];
}


int main()
{
	char str1[] = "hello";
	char str2[] = "elloh";

	// allocate memory
	int len1 = strlen(str1) + 1;
	int len2 = strlen(str2) + 1;
	int i;
	int ** D = calloc(len1, sizeof(int*));
	for (i = 0; i < len1; i++) {
		D[i] = calloc(len2, sizeof(int));
	}

	int ld = LevenshteinDistance(str1, str2, len1, len2, D);

	printf("\nLD: %i\n\n", ld);
	return 0;
}