// DP table initialized beforehand
#define LEN1 1000
#define LEN2 1000
export void LD_ispc(uniform int str1[], uniform int str2[], 
                    uniform int start_i, uniform int start_j,
                    uniform int diag_len, uniform int D[LEN1][LEN2])
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

// prev1: start index of previous diagonal
export void LD_diag_ispc( // uniform int str1[], uniform int str2[],
                         uniform int len1, uniform int len2,
                         uniform int diag_start, uniform int start, uniform int end,
                         uniform int str1_start, uniform int str2_start, uniform int str_sub[],
                         uniform int left, uniform int top_left,
                         uniform int D[])
{
    // bool equal = equal[(str1_start-start)*(len2-1)+str2_start+start];
    // print ("Equal % \n", equal_idx);
    foreach(k = start ... end) { 
        // int str1_idx = str1_start-k;
        // int str2_idx = str2_start+k;
        // if (str1[str1_idx] == str2[str2_idx])
        int add_one = str_sub[(str1_start-k)*(len2-1)+str2_start+k];
        // print ("k = %, equal %, add %\n", k, is_equal, add_one);
        D[diag_start+k] = min(min(D[left+k]+1, D[left+k+1]+1), D[top_left+k] + add_one);
    }
}
/*
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
*/
#define BLOCK_LEN 1000
// Use tasking
task void LD_ispc_task(uniform int str1[], uniform int str2[],
                       uniform int start_i, uniform int start_j,
                       uniform int diag_len, uniform int D[LEN1][LEN2]) 
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
                              uniform int diag_len, uniform int D[LEN1][LEN2])
{
    uniform int taskCount = (diag_len + BLOCK_LEN - 1) / BLOCK_LEN;
    launch[taskCount] LD_ispc_task(str1, str2, start_i, start_j, diag_len, D);  
}

