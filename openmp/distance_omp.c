#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

inline int max ( int a, int b ) { return a > b ? a : b; }
inline int min ( int a, int b ) { return a < b ? a : b; }

int LevenshteinDistance(char* str1, char* str2, int len1, int len2, int ** D) {
	int i, j;

	#pragma omp parallel
	{	
		#pragma omp for
		for (i = 0; i < len1; i++) {
			D[i][0] = i;
		}
		#pragma omp for
		for (j = 0; j < len2; j++) {
			D[0][j] = j;
		}
	}
	
	int row_block = 10; 
	#pragma omp parallel 
	{
		// printf("number of threads %i \n", omp_get_thread_num());
		int thread_id = omp_get_thread_num();
		int num_thread = omp_get_num_threads();
		int num_col = (len2-1) / num_thread;
		int col_s = 1 + num_col * thread_id;
		int col_e = col_s + num_col;
		if (thread_id == num_thread-1) {
			col_e = len2;
		}

		int row = 1 - thread_id;
		int col;
		while (row < len1 + (num_thread - thread_id - 1)) {
			// printf("thread_id: %i, row: %i \n", thread_id, row);
			if (row >= 1 && row < len1) {
				for (col = col_s; col < col_e; col++) {
					int add_one = 1;
					if(str1[row-1] == str2[col-1]) {
						add_one = 0;
					}
					D[row][col] = min( min(D[row-1][col]+1, D[row][col-1]+1), D[row-1][col-1] + add_one);
				}
			}
			row++;
			#pragma omp barrier 
		}
	}
	
	return D[len1-1][len2-1];
}

int seq_LevenshteinDistance(char* str1, char* str2, int len1, int len2, int ** D) {
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
	// initialize the 2 strings
	int strlength1 = 100;
	int strlength2 = 100000;
	char* str1 = malloc(strlength1 * sizeof(char));
	char* str2 = malloc(strlength2 * sizeof(char));
	int i;
	for (i=0; i<strlength1-1; i++) {
		str1[i] = 'a';	
	}
	for (i=0; i<strlength2-1; i++) {
		str2[i] = 'a' + i % 26;
	}
	str1[strlength1-1] = '\0';
	str2[strlength2-1] = '\0';

	// allocate memory
	int len1 = strlen(str1) + 1;
	int len2 = strlen(str2) + 1;
	
	// optimization: swith strings
	if (len1 > len2) {
		char* temp = str1;
		str1 = str2;
		str2 = temp;
		int templen = len1; 
		len1 = len2;
		len2 = templen;
	}
	printf("len1: %i, len2: %i\n", len1, len2);

	int ** D = calloc(len1, sizeof(int*));
	for (i = 0; i < len1; i++) {
		D[i] = calloc(len2, sizeof(int));
	}

	clock_t t = clock(); 
	int ld; 
	for (i = 0; i<100; i++) {
		ld = LevenshteinDistance(str1, str2, len1, len2, D);
	}
	t = clock() - t;
	double time_taken = ((double)t)/CLOCKS_PER_SEC; // in second


	clock_t st = clock();
	int seq_ld;
	for (i = 0; i<100; i++) {
		seq_ld = seq_LevenshteinDistance(str1, str2, len1, len2, D);
	}
	st = clock() - st;
	double seq_time_taken = ((double)st)/CLOCKS_PER_SEC; 

	printf("\nLD: %i, seq_LD: %i \n\n", ld, seq_ld);

	printf("omp time took: %f \n", time_taken);
	printf("seq time took: %f \n", seq_time_taken);

	return 0;
}