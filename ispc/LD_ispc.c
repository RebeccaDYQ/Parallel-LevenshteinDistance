#define MAXLEN 10000
#define MINLEN 1000

#define len1 1000
#define len2 1000
// DP table initialized beforehand
export void LD_ispc(uniform int str1[], uniform int str2[], 
                    uniform int start_i, uniform int start_j,
                    uniform int diag_len, uniform int D[len1][len2])
{
    foreach(k = 0 ... diag_len) {
        // Compute entry [start_i-k][start_j+k]
        int i = start_i - k, j = start_j + k;
        int add_one = 1;
        if (str1[i-1] == str2[j-1])
            add_one = 0;
        D[i][j] = min(min(D[i-1][j]+1, D[i][j-1]+1), D[i-1][j-1] + add_one);
        
    }
}

export void LD_ispc_unroll(uniform int str1[], uniform int str2[],
                           uniform int start_i1, uniform int start_j1,
                           uniform int start_i2, uniform int start_j2,
                           uniform int diag_len1, uniform int diag_len2,
                           uniform int D[len1][len2])
{
    int add_one;
    foreach(k = 0 ... diag_len1) {
        int i = start_i1 - k, j = start_j1 + k;
        add_one = 1;
        if (str1[i-1] == str2[j-1])
            add_one = 0;
        D[i][j] = min(min(D[i-1][j]+1, D[i][j-1]+1), D[i-1][j-1] + add_one);
    }
    foreach(k = 0 ... diag_len2) {
        int i = start_i2 - k, j = start_j2 + k;
        add_one = 1;
        if (str1[i-1] == str2[j-1])
            add_one = 0;
        D[i][j] = min(min(D[i-1][j]+1, D[i][j-1]+1), D[i-1][j-1] + add_one);
    }
}

#define BLOCK_LEN 1000
// Use tasking
task void LD_ispc_task(uniform int str1[], uniform int str2[],
                       uniform int start_i, uniform int start_j,
                       uniform int diag_len, uniform int D[len1][len2]) 
{
    uniform int kstart = taskIndex * BLOCK_LEN;
    uniform int kend = min(kstart + BLOCK_LEN, diag_len); 
    foreach(k = kstart ... kend) {
        int i = start_i - k, j = start_j + k;
        int add_one = 1;
        if (str1[i-1] == str2[j-1])
            add_one = 0;
        D[i][j] = min(min(D[i-1][j]+1, D[i][j-1]+1), D[i-1][j-1] + add_one);
    }
}

export void LD_ispc_withtasks(uniform int str1[], uniform int str2[],
                              uniform int start_i, uniform int start_j,
                              uniform int diag_len, uniform int D[len1][len2])
{
    uniform int taskCount = (diag_len + BLOCK_LEN - 1) / BLOCK_LEN;
    launch[taskCount] LD_ispc_task(str1, str2, start_i, start_j, diag_len, D);  
}
