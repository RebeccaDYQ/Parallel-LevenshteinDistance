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
	
	int row_block = 1; 
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

		int row = 1 - thread_id*row_block;
		int col;

		while (row < len1 + (num_thread - thread_id - 1)* row_block) {
			// printf("thread_id: %i, row: %i \n", thread_id, row);
			int inner_row_i = 0;
			for (inner_row_i = 0; inner_row_i < row_block; inner_row_i++) {
				int row_i = row + inner_row_i;
				if (row_i >= 1 && row_i < len1) {
					for (col = col_s; col < col_e; col++) {
						int add_one = 1;
						if(str1[row_i-1] == str2[col-1]) {
							add_one = 0;
						}
						D[row_i][col] = min( min(D[row_i-1][col]+1, D[row_i][col-1]+1), D[row_i-1][col-1] + add_one);
					}
				}
			}
			row += row_block;
			#pragma omp barrier 
		}
	}
	
	return D[len1-1][len2-1];
}


void diagonal_LD_element(char* str1, char* str2, int** D, int i, int j) {
	i++; j++; 
	int add_one = 1;
	if(str1[i-1] == str2[j-1]) {
		add_one = 0;
	}
	D[i][j] = min(min(D[i-1][j]+1, D[i][j-1]+1), D[i-1][j-1] + add_one);
	// printf("D[%i][%i] %i\n", i, j, D[i][j]);
}

int diagonal_LD(char* str1, char* str2, int len1, int len2, int ** D) {
	int i;

	#pragma omp parallel for 
	for (i = 0; i < len1; i++) {
		D[i][0] = i;
	}

	#pragma omp parallel for
	for (i = 0; i < len2; i++) {
		D[0][i] = i;
	}

	int diag_idx;
	len1--; len2--;

	int sequential_threshold = 10;
	// printf("len1: %i, len2: %i\n", len1, len2);
	for (diag_idx = 0; diag_idx < len1 + len2; diag_idx++) {
		if (diag_idx < sequential_threshold) {
			for (i = 0; i<sequential_threshold; i++) {
				int j;
				for (j=0; j<sequential_threshold-i; j++) {
					diagonal_LD_element(str1, str2, D, i, j);
				}
			}
			diag_idx = sequential_threshold - 1;
		} else if (diag_idx < len1) {
        	#pragma omp parallel for
	        for (i=0; i<diag_idx + 1; i++) {
	        	int j = diag_idx - i;
	        	diagonal_LD_element(str1, str2, D, i, j);
		    }
        } else if (diag_idx < len2) {
        	#pragma omp parallel for
        	for(i=0; i<len1; i++) {
        		int j = diag_idx - i;
        		diagonal_LD_element(str1, str2, D, i, j);
        	}
        } else if (diag_idx < len1 + len2 - sequential_threshold){
        	#pragma omp parallel for
        	for(i=diag_idx+1-len2; i<len1; i++) {
        		int j = diag_idx - i;
        		diagonal_LD_element(str1, str2, D, i, j);
        	}
        } else {
        	for (i = len1 - sequential_threshold; i<len1; i++) {
				int j;
				for (j=len2 - (i - len1 + sequential_threshold); j<len2; j++) {
					diagonal_LD_element(str1, str2, D, i, j);
				}
			}
			diag_idx = len1 + len2;
        }
    }

    return D[len1][len2];
}

// int diagonal_mem_LD(char* str1, char* str2, int len1, int len2, int *D) {
// 	// D is of size len1*len2
	
// }

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
	int trial_num = 100;
	int trial_i = 0; 
	int strlength1 = 100;
	int strlength2 = 10000;
	char* str1 = malloc(strlength1 * sizeof(char));
	char* str2 = malloc(strlength2 * sizeof(char));
	int i;
	for (i=0; i<strlength1-1; i++) {
		str1[i] = 'a' + rand() % 26;	
	}
	for (i=0; i<strlength2-1; i++) {
		str2[i] = 'a' + rand() % 26;
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

	// calling sequential LD
	double seq_time = 0.0, diag_time = 0.0, row_time = 0.0; 
	int seq_ld, diag_ld, row_ld; 
	
	for (trial_i = 0; trial_i<trial_num; trial_i++) {
		// initialize memory
		int ** seq_D = calloc(len1, sizeof(int*));
		for (i = 0; i < len1; i++) {
			seq_D[i] = calloc(len2, sizeof(int));
		}

		int ** row_D = calloc(len1, sizeof(int*));
		for (i = 0; i < len1; i++) {
			row_D[i] = calloc(len2, sizeof(int));
		}

		int ** diag_D = calloc(len1, sizeof(int*));
		for (i = 0; i < len1; i++) {
			diag_D[i] = calloc(len2, sizeof(int));
		}

		// execution
		clock_t t = clock(); 
		seq_ld = seq_LevenshteinDistance(str1, str2, len1, len2, seq_D);
		seq_time += clock() - t;
		
		t = clock(); 
		diag_ld = diagonal_LD(str1, str2, len1, len2, diag_D);
		diag_time += clock() - t;
		
		t = clock(); 
		row_ld = LevenshteinDistance(str1, str2, len1, len2, row_D);
		row_time += clock() - t;

		// deallocate memory to clear everything before next trial
		for (i = 0; i < len1; i++) {
			free(diag_D[i]);
		}
		free(diag_D);

		for (i = 0; i < len1; i++) {
			free(seq_D[i]);
		}
		free(seq_D);

		for (i = 0; i < len1; i++) {
			free(row_D[i]);
		}
		free(row_D);
	}

	printf("\nrow_LD: %i, seq_LD: %i, diag_ld: %i\n\n", row_ld, seq_ld, diag_ld);
	printf("omp time took: %f \n", row_time);
	printf("seq time took: %f \n", seq_time);
	printf("diagonal time took: %f \n\n", diag_time);
	printf("row major speedup: %f \n", seq_time / row_time);
	printf("diagonal major speedup: %f \n", seq_time / diag_time);

	return 0;
}