#define MAXLEN 256

// DP table initialized beforehand
export void LD_ispc(uniform int str1[], uniform int str2[], 
                    uniform int start_i, uniform int start_j,
                    uniform int diag_len, uniform int D[MAXLEN][MAXLEN])
{
    foreach(k = 0 ... diag_len) {
        // Compute entry [start_i-k][start_j+k]
        int i = start_i-k, j = start_j+k;
        int add_one = 1;
        if (str1[i-1] == str2[j-1])
            add_one = 0;
        D[i][j] = min(min(D[i-1][j]+1, D[i][j-1]+1), D[i-1][j-1] + add_one);
    }
}

