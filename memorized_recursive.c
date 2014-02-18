//Let's consider an infinite sequence A defined as follows:
//* Ai = 1 for all i <= 0;
//* * Ai = A[i/p]-x + A[i/q]-y for all i >= 1, where [x] denotes the floor function of x. （see Notes）
//* You will be given n, p, q, x and y. Return the n-th element of A （index is 0-based）.
//* Constraints
//* - n will be between 0 and 10^13, inclusive.
//*   - p and q will both be between 2 and 10^9, inclusive.
//*   - x and y will both be between 0 and 10^9, inclusive.

#include<stdio.h>
#include<stdlib.h>
int calc(int* m, int n, int p, int q, int x, int y){
    if(n <= 0)
        return 1;
    else if(m[n] != 0){
        return m[n];
    }
    else
        m[n] = calc(m, n/p-x, p, q, x, y) + calc(m, n/p-x, p, q, x, y);
        return m[n];
}
int main(){
    int *m, *k;
    int n=100000000, p=2, q=3, x=4, y=1;

    m = k = (int*)malloc(sizeof(int)*n);
    for(; k != &m[n]; k++)
        *k = 0;

    printf("%d\n", calc(m, n, p, q, x, y));

    return 0;
}
