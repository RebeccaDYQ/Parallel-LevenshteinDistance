#include <stdio.h>
#include "LD_ispc.h"

#define MAXLEN 256
inline int min ( int a, int b ) { return a < b ? a : b; }
inline int max ( int a, int b ) { return a > b ? a : b; }

void display_table(int D[MAXLEN][MAXLEN], int len1, int len2) {
    for (int i = 0; i < len1; i++) {
        for (int j = 0; j < len2; j++)
            printf("[%d][%d]: %d ", i, j, D[i][j]);
        printf("\n");
    }
}

int main()
{
    // char str1[] = "hello";
    // char str2[] = "elloh";
    int str1[] = {1, 2, 3, 3, 4};
    int str2[] = {2, 3, 3, 4, 1};

    // allocate memory
    // int len1 = strlen(str1) + 1;
    // int len2 = strlen(str2) + 1;
    int len1 = sizeof(str1) / sizeof(str1[0]) + 1;
    int len2 = sizeof(str2) / sizeof(str2[0]) + 1;
    int D[MAXLEN][MAXLEN] = {0};
    int i;
    for (i = 0; i < len1; i++)
        D[i][0] = i;
    for (i = 0; i < len2; i++)
        D[0][i] = i;

    int diag_idx, diag_len, start_i, start_j;
    int shorter = min(len1, len2);
    // Loop over diagnoals
    for (diag_idx = 3-len2; diag_idx < len1; diag_idx++) {
        diag_len = min(shorter, len1+diag_idx);
        diag_len = min(diag_len, len2-diag_idx);
        if (diag_idx <= 0)
            diag_len -= 2;

        start_i = min(diag_idx+len1-2, len1-1);
        start_j = max(1, diag_idx);
        ispc::LD_ispc(str1, str2, start_i, start_j, diag_len, D);
        #ifdef DEBUG
        printf("Diag %d len %d start[%d][%d]\n", diag_idx, diag_len, start_i, start_j);
        display_table(D, len1, len2);
        #endif
    }
    printf("\nLD: %i\n\n", D[len1-1][len2-1]);
    return 0;
}
