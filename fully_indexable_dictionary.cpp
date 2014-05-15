#include<iostream>
#include<stdlib.h>
#include<string.h>

using namespace std;

class FullyIndexableDictionary{
private:
    int _n, n, l, s, len_p;
    unsigned char *B;
    char *L, *S, *P;
public:
    ~FullyIndexableDictionary(){
        free(B);
        free(L);
        free(S);
        free(P);
    };
    FullyIndexableDictionary(const char* _B){
        //const char* _B = "10100000...
        // In Fullyindexabledictionary, we need to possess given bit sequence
        _n = strlen(_B);

        B = (unsigned char*)malloc( sizeof(unsigned char) * (_n >> 3) );
        for(int i=0; i<_n; ++i){
            if( _B[i] - '0' ){ B[ (i >> 3) ] |= (1 << (7 - (i & 0x07))); }
        }

        // let us consider a bit vector B the size of n = 256 (bits)
        n = 256;
        // the size l = lg^2(n) = 64 covered by L[i],
        // and an element of L can be stored in lg64 = 6 (bits)
        l = 64;
        // the size s = lg(n)/2 = 4 covered by S[i],
        // and an element of S can be stored in lg4 = 2 (bits)
        s = 4;
        // |P[0, 2^s)| = 16
        len_p = 1 << 4;

        L = (char*)malloc(sizeof(char) * (n/l));
        S = (char*)malloc(sizeof(char) * (n/s));
        P = (char*)malloc(sizeof(char) * len_p);

        // initialize L[0,n/l), S[0,n/s)
        L[0] = S[0] = 0;
        for(int i=0,j=0,k=0; i < _n; ++i){
            if( !( i % l ) ){
                ++j;
                L[j] = L[j-1];
            }

            if( !( i % s ) ){
                if( !( i % l ) ){ S[k] = 0; }
                ++k;
                S[k] = S[k-1];
            }

            if( B[(i >> 3)] & (1 << (7 - (i & 0x07))) ){
                ++L[j];
                ++S[k];
            }

            // DEBUG
//            if( i % s ){
//                printf("B[%d](%d)=%d\n", (i>>3), (7-(i & 0x07)), B[(i>>3)] & (1<<(7-(i&0x07))) );
//            }else{
//                printf("B[%d](%d)=%d, L[%d]=%d, S[%d]=%d\n", (i>>3), (7-(i & 0x07)), B[(i>>3)] & (1<<(7-(i&0x07))), j-1, (j>0?L[j-1]:0), k-1, (k>0?S[k-1]:0));
//            }
        }

        // initialize P[0,i)
        for(int i=0; i < (1 << s); ++i){
            P[i] = 0;
            for(int j=1, j_end = (i << sizeof(char)); j < j_end; j <<= 1){
                if( i & j ){ ++P[i]; }
            }
        }
    };

    int rank(int i){
        // create a mask; like "11111000", #0 is (s - i % s)
        unsigned char mask = 0xff << ( s - (i % s) );

        // extract a bit sequence stored in B;
        // if (i / 4) != (i / 8 * 2), for example i = 15
        // then, use lower bits the size of s
        unsigned char bitseq = B[(i >> 3)];
        if( (i >> 2) != ((i >> 3) << 1) ){
            bitseq <<= s;
            bitseq >>= s;
        }else{
            bitseq >>= s;
        }

        return L[( i / l )] + S[( i / s )] + P[ bitseq & mask ];
    };

    int select(int);
};

int main(){
    const char* B = "1010000000010100010100000000101010000000111000101111000000000000010001010010100001000001010100010001010001001001010000101000010001000101001010000100000101010001000101000100100101000010100001000100010100101000010000010101000100010100010010010100001010000100";
    FullyIndexableDictionary fid = FullyIndexableDictionary(B);
    for(int i=0; i<strlen(B); ++i){
        int r=0;
        for(int j=0; j<i; ++j){ if( B[j] - '0' ){ ++r; } }
        if(fid.rank(i) != r){
            printf("rank(B,%d):%d[ans:%d]\n", i, fid.rank(i), r);
        }
    }
}
