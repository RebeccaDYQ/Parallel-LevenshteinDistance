#include <mpi.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

inline int max ( int a, int b ) { return a > b ? a : b; }
inline int min ( int a, int b ) { return a < b ? a : b; }

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


void parallel_LD(char* str1, char* str2, int len1, int start_j, int end_j, int ** D, int pid, int pcount) {
	int left_value = 0;
	int upper_left_value = 0; 
	int i=0, j=0;
	for(i = 0 - pid; i<len1 + (pcount - pid - 1); i++) {
		if (i>=0 && i < len1) {
			if(pid == 0) {
				upper_left_value = left_value;
				left_value = i;
			}
			if(i==0) {
				for(j=start_j; j<end_j; j++){
					D[i][j-start_j] = j;
				}
			} else {
				for(j = start_j; j<end_j; j++) {
					int add_one = 1;
					if(str1[i-1] == str2[j-1]) {
						add_one = 0;
					}
					int local_j = j - start_j;
					if(j==start_j) {
						D[i][local_j] = min( min(D[i-1][local_j]+1, left_value+1), upper_left_value + add_one);
					} else {
						D[i][local_j] = min( min(D[i-1][local_j]+1, D[i][local_j-1]+1), D[i-1][local_j-1] + add_one);
					}
				}
			}

			if (pid < pcount - 1) {
				MPI_Send(&D[i][end_j-start_j-1], 1, MPI_INT, pid+1, 0, MPI_COMM_WORLD);
			}
		}
		
		if (i>=-1 && i < len1-1 && pid > 0) {
			upper_left_value = left_value;
			MPI_Recv(&left_value, 1, MPI_INT, pid-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		MPI_Barrier(MPI_COMM_WORLD);
	}
}

int main()
{	
	// MPI setup
	int pid = 0;
    int pcount = 1;
    
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &pcount);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    bool mpi_master = pid == 0;
    // printf("Hello! I am %d of %d\n", pid, pcount);

    int strlength1 = 100;
	int strlength2 = 10000;
	char* str1 = malloc(strlength1 * sizeof(char));
	char* str2 = malloc(strlength2 * sizeof(char));

	double seq_time = 0.0;
	int seq_ld;

	int trial_num = 100;
	int trial_i = 0; 
    
    // time seq 
    if (mpi_master) {
		// initialize the 2 strings
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

		printf("len1: %i, len2: %i\n", len1, len2);

		for (trial_i = 0; trial_i<trial_num; trial_i++) {
			// initialize memory
			int ** seq_D = calloc(len1, sizeof(int*));
			for (i = 0; i < len1; i++) {
				seq_D[i] = calloc(len2, sizeof(int));
			}
			// execution
			clock_t t = clock(); 
			seq_ld = seq_LevenshteinDistance(str1, str2, len1, len2, seq_D);
			seq_time += clock() - t;
			// free memory
			for (i = 0; i < len1; i++) {
				free(seq_D[i]);
			}
			free(seq_D);
		}
		printf("\nseq time took: %f \n", seq_time);
		printf("seq_LD: %i \n\n", seq_ld);
	} 

	MPI_Bcast(str1, strlength1, MPI_CHAR, 0, MPI_COMM_WORLD);
	MPI_Bcast(str2, strlength2, MPI_CHAR, 0, MPI_COMM_WORLD);

	double row_time = 0.0; 
	clock_t t;
	
	int len1 = strlength1;
	int start_j = pid * (strlength2 / pcount);
	if (pid == 0) {
		start_j = 1;
	}
	int end_j = (pid+1) * (strlength2 / pcount);
	if(pid == pcount - 1){
		end_j = strlength2;
	}

	for (trial_i = 0; trial_i<trial_num; trial_i++) {
		int i;
		int ** row_D = calloc(len1, sizeof(int*));
		for (i = 0; i < len1; i++) {
			row_D[i] = calloc(end_j - start_j, sizeof(int));
		}
		
		if(mpi_master)
			t = clock(); 

		parallel_LD(str1, str2, len1, start_j, end_j, row_D, pid, pcount);

		if(mpi_master)
			row_time += clock() - t;

		if(trial_i == 0 && pid == pcount - 1) {
			int row_ld = row_D[strlength1-1][end_j-start_j-1];
			printf("\nrow_LD: %i \n", row_ld);
		}
		// deallocate memory to clear everything before next trial
		for (i = 0; i < len1; i++) {
			free(row_D[i]);
		}
		free(row_D);
	}

	if(mpi_master) {
		printf("omp time took: %f \n", row_time);
		printf("row major speedup: %f \n", seq_time / row_time);
	}

	MPI_Finalize();
	return 0;
}