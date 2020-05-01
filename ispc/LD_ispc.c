// DP table initialized beforehand
#define LEN1 1000
#define LEN2 1000
/*
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
*/
export void LD_row_init(uniform int len2, uniform int D[])
{
    foreach(i = 0 ... len2) {
        D[i] = i;     
    }
}


export void LD_diag_ispc( // uniform int str1[], uniform int str2[],
                         uniform int diag_start, uniform int start, uniform int end,
                         uniform int left, uniform int top_left,
                         uniform int D[])
{
    foreach(k = start ... end) { 
        // print ("k = %, equal %, add %\n", k, is_equal, add_one);
        D[diag_start+k] = min(min(D[left+k], D[left+k+1])+1, D[top_left+k]+D[diag_start+k]);
    }
}

// Coalesce gathers into loads: seg fault
export void LD_diag_compare(uniform int str1[], uniform int str2[],
                            uniform int len1, uniform int len2, uniform int shorter,
                            uniform int diag_starts[], uniform int diag_lens[], uniform int D[])
{
    for (uniform int i = 2; i < len1+len2-1; i++) {
        uniform int diag_start = diag_starts[i];
        uniform int diag_len = diag_lens[i];
        uniform int start = i < len1? 1: 0;
        uniform int end = i < len2? diag_len-1: diag_len;
        uniform int str1_start = min(i, len1-1)-1;
        uniform int str2_start = max(0, i-len1+1)-1;
        foreach(k = start ... end) {
            if (str1[str1_start-k] != str2[str2_start+k])
                D[diag_start+k] = 1;
        }
    }
}

export void LD_diag_init(uniform int len1, uniform int len2, uniform int shorter,
                         uniform int diag_starts[], uniform int D[])
{
    foreach(i = 0 ... len1) {
        D[diag_starts[i]] = i;
    }
    foreach(i = 0 ... len2) {
        D[diag_starts[i+1]-1] = i;
    }
        
}
export void LD_each_diag_compare(uniform int str1[], uniform int str2[],
                                 uniform int diag_start, uniform int start, uniform int end,
                                 uniform int str1_start, uniform int str2_start,
                                 uniform int D[])
{
    foreach(k = start ... end) {
        if (str1[str1_start-k] != str2[str2_start+k])
            D[diag_start+k] = 1;
    }
}

export void LD_row_compare(uniform int str1[], uniform int str2[],
                           uniform int len1, uniform int len2,
                           uniform int D[])
{
    for (uniform int i = 0; i < len1-1; i++) {
        foreach(j = 0 ... len2-1) {
            if (str1[i] != str2[j])
                D[(i+1)*len2+j+1] = 1;
        }
    }
}

export void LD_row_ispc(uniform int len1, uniform int len2,
                        uniform int D[])
{
    uniform int row_blocks = (len1-1+7)/8; // start from row 1
    for (uniform int row_block = 0; row_block < row_blocks; row_block++) {
        uniform int row_start = 1 + row_block*8;
        uniform int row_end = min(row_start+8, len1);
        // print ("Row block %, start %, end %\n", row_block, row_start, row_end);
        uniform int rows = row_end - row_start;
        for (uniform int col_start = 1; col_start < len2+rows-1; col_start++) {
            foreach(row_idx = 0 ... rows) {
            // foreach(i = row_start ... row_end) {
                int i = row_start + row_idx;
                int j = col_start - row_idx;
                // int j = col_start - (i-1)%8; 
                if (j >= 1 && j < len2) {
                    // print ("Compute %, %\n", i, j);
                    D[i*len2+j] = min(min(D[(i-1)*len2+j], D[i*len2+j-1])+1, D[(i-1)*len2+j-1] + D[i*len2+j]);       
                }
            }
        }
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
*/
