#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "LD_ispc.h"
#include "../sequential/LD_seq.h"

#define TRIALS 100

inline int min ( int a, int b ) { return a < b ? a : b; }
inline int max ( int a, int b ) { return a > b ? a : b; }

void display_table(int **D, int len1, int len2) {
    for (int i = 0; i < len1; i++) {
        for (int j = 0; j < len2; j++)
            printf("[%d][%d]: %d ", i, j, D[i][j]);
        printf("\n");
    }
}


void display_diag_table(int D[], int diag_starts[], int diag_lens[], int len1, int len2) {
    for (int i = 0; i < len1+len2-1; i++) {
        for (int j = diag_starts[i]; j < diag_starts[i]+diag_lens[i]; j++) {
            printf("%d ", D[j]);
        }
        printf("\n");
    }
}

int LD_int_seq(int str1[], int str2[], int len1, int len2, int **D) {
    int i, j;
    for (i = 0; i < len1; i++)
        D[i][0] = i;
    for (j = 0; j < len2; j++)
        D[0][j] = j;
    
    for (i = 1; i < len1; i++) {
        for (j = 1; j < len2; j++) {
            int add_one = 1;
            if (str1[i-1] == str2[j-1])
                add_one = 0;
            D[i][j] = min( min(D[i-1][j]+1, D[i][j-1]+1), D[i-1][j-1] + add_one);
        }
    }
    return D[len1-1][len2-1];
}


int main()
{
    static const int strlength1 = 1000;
    static const int strlength2 = 1000;

    char* str1 = (char *)calloc(strlength1, sizeof(char));
    char* str2 = (char *)calloc(strlength2, sizeof(char));
    int i, j;
    for (i = 0; i < strlength1-1; i++) {
        str1[i] = 'a' + rand() % 26;
    }
    for (i = 0; i < strlength2-1; i++) {
        str2[i] = 'a' + rand() % 26;
    }
    str1[strlength1-1] = '\0';
    str2[strlength2-1] = '\0';

    static const int len1 = strlength1;
    static const int len2 = strlength2;
    
    printf("len1: %i, len2: %i\n", len1, len2);
    // printf("str1: %s, str2: %s\n", str1, str2);
    
    // Convert string to int array
    static int int_str1[strlength1];
    static int int_str2[strlength2];
    for (i = 0; i < strlength1-1; i++) {
        int_str1[i] = str1[i] - 'a';
    }
    for (i = 0; i < strlength2-1; i++) {
        int_str2[i] = str2[i] - 'a';
    }

    // ISPC time
    double seq_time = 0.0, ispc_time = 0.0;

    int shorter = min(len1, len2);
    int ld, seq_ld;
    // Track ispc time
    clock_t t = clock(); 
    for (int n = 0; n < TRIALS; n++) {
        /*
        static int D[len1][len2];
        for (i = 0; i < len1; i++)
            D[i][0] = i;
        for (i = 0; i < len2; i++)
            D[0][i] = i;
        // Loop over diagnoals  
        int diag_idx, diag_len, start_i, start_j;
        for (diag_idx = 3-len1; diag_idx < len2; diag_idx ++) {
            diag_len = min(shorter, len1+diag_idx);
            diag_len = min(diag_len, len2-diag_idx);
            if (diag_idx <= 0)    diag_len --;
            if (diag_idx+len1 <= len2)    diag_len --;
            start_i = min(diag_idx+len1-2, len1-1);
            start_j = max(1, diag_idx);
            // ispc::LD_ispc(int_str1, int_str2, start_i, start_j, diag_len, D);
            ispc::LD_ispc_withtasks(int_str1, int_str2, start_i, start_j, diag_len, D);   
        }
        ld = D[len1-1][len2-1];
        */
        
        // Store in diagonal major
        static int D[len1*len2];
        // static int str_sub[(len1-1)*(len2-1)];
        // Precompute all diagonal lengths and start points
        int *diag_lens = (int *)calloc(len1+len2-1, sizeof(int));
        int *diag_starts = (int *)calloc(len1+len2-1, sizeof(int));
        diag_starts[0] = 0;
        int diag_len, diag_start, start, end, prev1, prev2, str1_start, str2_start, left, top_left;
        for (i = 0; i < len1+len2-1; i++) {
            int diag_pos = i+1-len1;
            diag_len = min(shorter, len1+diag_pos);
            diag_len = min(diag_len, len2-diag_pos);
            diag_lens[i] = diag_len;
            if (i < len1+len2-2)
                diag_starts[i+1] = diag_starts[i] + diag_len;
        }
        
        // Initialize table
        for (i = 0; i < len1+len2-1; i++) {
            if (i <= len1-1)
                D[diag_starts[i]] = i;
            if (i < len2)
                D[diag_starts[i+1]-1] = i;
        }
        // display_diag_table(D, diag_starts, diag_lens, len1, len2);
        // Initialize substitution values
        for (i = 2; i < len1+len2-1; i++) {
            diag_len = diag_lens[i];
            diag_start = diag_starts[i];
            start = i < len1? 1: 0;
            end = i < len2? diag_len-1: diag_len;
            str1_start = min(i, len1-1)-1;
            str2_start = max(0, i-len1+1)-1;
            for (int k = start; k < end; k++) {
                if (int_str1[str1_start-k] != int_str2[str2_start+k])
                    D[diag_start+k] = 1;
            }
        }
        /*
        for (i = 0; i < len1-1; i++) {
            for (j = 0; j < len2-1; j++) {
                if (int_str1[i] != int_str2[j])
                    str_sub[i*(len2-1)+j] = 1;
                // printf("[%d][%d] %d", i, j, str_sub[i*(len2-1)+j]);
            }
            // printf("\n");
        }
        */

        // Loop over diagnoals: exclude top left and bottom right corners 
        for (i = 2; i < len1+len2-1; i++) {
            
            diag_len = diag_lens[i];
            diag_start = diag_starts[i];
            prev1 = diag_starts[i-1];
            prev2 = diag_starts[i-2];
            // printf("Diag %d len %d start %d\n", i, diag_len, diag_start);
        
            start = i < len1? 1: 0;
            end = i < len2? diag_len-1: diag_len;
            left = i < len1? prev1-1: prev1;
            top_left = i-1 < len1? prev2-start: prev2+1;
            // str1_start = min(i, len1-1)-1;
            // str2_start = max(0, i-len1+1)-1;
            // printf("Left %d top left %d str1 %d str2 %d\n", left, top_left, str1_start, str2_start);
            ispc::LD_diag_ispc(diag_start, start, end, left, top_left, D);
            // display_diag_table(D, diag_starts, diag_lens, len1, len2);
        }
        ld = D[len1*len2-1];
        free(diag_lens);
        free(diag_starts);
    }
    ispc_time = clock() - t;
    // Compare with sequential version
    clock_t st = clock();
    for (i = 0; i < TRIALS; i++) {
        int **seq_D = (int **)calloc(len1, sizeof(int*));
        for (j = 0; j < len1; j++) {
            seq_D[j] = (int *)calloc(len2, sizeof(int));
        }
    
        seq_ld = LevenshteinDistance(str1, str2, len1, len2, seq_D);
        // seq_ld = LD_int_seq(int_str1, int_str2, len1, len2, seq_D);
        
        for (j = 0; j < len1; j++) {
            free(seq_D[j]);
        }
        free(seq_D);
    }
    seq_time = clock() - st;
    // display_table(seq_D, len1, len2);
    
    printf("\nLD: %i, seq_LD: %i \n\n", ld, seq_ld); 
    printf("ISPC time took: %f s\n", ispc_time/CLOCKS_PER_SEC);
    printf("Seq time took: %f s\n", seq_time/CLOCKS_PER_SEC);
    printf("ISPC speedup: %f \n", seq_time / ispc_time);
    return 0;
}
