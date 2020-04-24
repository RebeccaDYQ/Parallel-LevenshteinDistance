#include <stdio.h>
#include <time.h>
#include "LD_ispc.h"

#define TRIALS 100
#define len1 1000
#define len2 1000

inline int min ( int a, int b ) { return a < b ? a : b; }
inline int max ( int a, int b ) { return a > b ? a : b; }

void display_table(int D[len1][len2]) {
    for (int i = 0; i < len1; i++) {
        for (int j = 0; j < len2; j++)
            printf("[%d][%d]: %d ", i, j, D[i][j]);
        printf("\n");
    }
}

int LD_int_seq(int str1[], int str2[], int D[len1][len2]) {
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
    // char str1[] = "hello";
    // char str2[] = "elloh";

    // allocate memory
    // int len1 = strlen(str1) + 1;
    // int len2 = strlen(str2) + 1;
    
    // static const int len1 = 100;
    // static const int len2 = 10000;
    // static int D[len1][len2];
    static int str1[len1-1];
    static int str2[len2-1];
    int i;
    for (i = 0; i < len1-1; i++) {
        str1[i] = 0;
    }
    for (i = 0; i < len2-1; i++) {
        str2[i] = 0 + i % 26;
    }

    printf("len1: %i, len2: %i\n", len1, len2);
    
    // static int D[MAXLEN][MAXLEN] = {0};
    
    // ISPC time
    clock_t t = clock(); 
    int diag_idx, diag_len, start_i, start_j;
    int shorter = min(len1, len2);
    int ld;
    for (int n = 0; n < TRIALS; n++) { 
    // Initialize table: moved to ispc kernel?
    static int D[len1][len2] = {0};
    
    for (i = 0; i < len1; i++)
        D[i][0] = i;
    for (i = 0; i < len2; i++)
        D[0][i] = i;
    
    // Loop over diagnoals  
    for (diag_idx = 3-len1; diag_idx < len2; diag_idx ++) {
        diag_len = min(shorter, len1+diag_idx);
        diag_len = min(diag_len, len2-diag_idx);
        if (diag_idx <= 0)    diag_len --;
        if (diag_idx+len1 <= len2)    diag_len --;

        start_i = min(diag_idx+len1-2, len1-1);
        start_j = max(1, diag_idx);

        ispc::LD_ispc(str1, str2, start_i, start_j, diag_len, D);
        /*
        diag_idx ++;
        int diag_len2 = min(shorter, len1+diag_idx);
        diag_len2 = min(diag_len, len2-diag_idx);
        if (diag_idx <= 0)    diag_len2 --;
        if (diag_idx+len1 <= len2)    diag_len2 --;
        int start_i2 = min(diag_idx+len1-2, len1-1);
        int start_j2 = max(1, diag_idx);
        ispc::LD_ispc_unroll(str1, str2, start_i, start_j, start_i2, start_j2, diag_len, diag_len2, D);
        */
        // ispc::LD_ispc_withtasks(str1, str2, start_i, start_j, diag_len, D);
        #ifdef DEBUG
        printf("Diag %d len %d start[%d][%d]\n", diag_idx, diag_len, start_i, start_j);
        display_table(D);
        #endif
    }
    ld = D[len1-1][len2-1];
    }
    t = clock() - t;
    double ispc_time = ((double)t)/CLOCKS_PER_SEC; // in second
    
    clock_t st = clock();
    static int D[len1][len2] = {0};
    int seq_ld;
    for (i = 0; i < TRIALS; i++) {
        seq_ld = LD_int_seq(str1, str2, D);
    }
    st = clock() - st;
    double seq_time = ((double)st)/CLOCKS_PER_SEC; 

    printf("\nLD: %i, seq_LD: %i \n\n", ld, seq_ld); 
    printf("ISPC time took: %f \n", ispc_time);
    printf("seq time took: %f \n", seq_time);
   
   return 0;
}
