#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<ctime>

#define TYPE_TO_BITS(X) (8 * sizeof(X))
#define DIVIDE8(X) ((X) >> 3)
#define MOD8(X) ((X) & 0x07)
#define LSB(X) ((X) & 0x01)
#define MAX(X,Y) ((X > Y) ? X : Y)

#define UB_ALPHABET_SIZE 10000
#define UB_TEXT_SIZE 10000

using namespace std;

typedef unsigned char block_b; /* DO NOT CHANGE THE SIZE OF B[i] */
typedef unsigned short block_s;
typedef unsigned int block_l;
typedef unsigned char block_p;

typedef unsigned char UCHAR;
typedef unsigned char UINT;
typedef unsigned short USHORT;
typedef unsigned long long ULL;

typedef size_t size_array;
typedef size_t size_bits;

class BitContainer{/*{{{*/
private:
    size_array len_B;
    size_bits bits_B;
    vector<block_b> B;
public:
    size_bits n, tail_idx;
    ~BitContainer(){
    };
    BitContainer(int _n):
        n(_n),
        tail_idx(0),
        bits_B(TYPE_TO_BITS(block_b))
    {
        len_B = (size_array)ceil( (int)n / (double)bits_B );
        B.resize( len_B, 0 );
    };
    BitContainer():
        n(UB_TEXT_SIZE),
        tail_idx(0),
        bits_B(TYPE_TO_BITS(block_b))
    {
        len_B = (size_array)ceil( (int)n / (double)bits_B );
        B.resize( len_B, 0 );
    };

    const char access(int i){
        return (B[ DIVIDE8(i) ] & ( 1 << MOD8(i) )) ? '1' : '0';
    };

    block_s set(int i, char b){
        if( i > tail_idx ){ tail_idx = i; }
        if( b - '0' > 0 ){
            return B[ DIVIDE8(i) ] |= 1 << MOD8(i);
        }else{
            return B[ DIVIDE8(i) ] &= ~( 1 << MOD8(i) );
        }
    };

    block_s append(char b){
        if( b - '0' > 0 ){
            B[ DIVIDE8(tail_idx) ] |= 1 << MOD8(tail_idx);
        }else{
            B[ DIVIDE8(tail_idx) ] &= ~( 1 << MOD8(tail_idx) );
        }
        return B[ DIVIDE8(tail_idx++) ];
    };
};/*}}}*/

class BitVector{/*{{{*/
private:
    size_array len_S, len_L, len_P, len_B;
    size_bits bits_B, bits_P, bits_S, bits_L, b, s, l;
    vector<block_b> B; /* in which a bit-vector is stored */
    vector<block_s> S; /* S: small blocks */
    vector<block_l> L; /* L: large blocks */
    vector<block_p> P; /* P: a dictionary of # of 1 in each bit-vector */
    void initialize(size_bits N){
        if( N == 0 ){
            printf("BitVector initialization error\n");
            exit(1);
        }
        n = N;
        int log2n = (int)ceil( log2( (int)n ) );
        s = (size_bits)MAX(1, ceil(log2n / 2.)); /* s = lg(n)/2 bits in B are covered by S[i] */
        l = (size_bits)MAX(1, (log2n * log2n)); /* l = lg^2(n) bits in B are covered by L[i] */
        while( l % s ){ l++; } /* make l become a multiple of s */
        len_B = (size_array)ceil( (int)n / (double)b ); /* |B[]| */
        len_S = (size_array)ceil( (int)n / (double)s ); /* |S[]| */
        len_L = (size_array)ceil( (int)n / (double)l ); /* |L[]| */
        len_P = (size_array)(1 << s); /* |P[0, 2^s)| */

        //----- check if each specified size is enough -----
        size_bits required_bits_p, required_bits_s, required_bits_l;
        required_bits_p = (size_bits)ceil( log2((int)s) );
        required_bits_s = (size_bits)ceil( log2((int)l) );
        required_bits_l = (size_bits)ceil( log2((int)n) );
        if( bits_P < required_bits_p ){
            printf("exceed # of bits of P[i], P[i] needs %d bits\n", (int)required_bits_p );
            exit(1);
        }
        if( bits_S < required_bits_s ){
            printf("exceed # of bits of S[i], S[i] needs %d bits\n", (int)required_bits_s );
            exit(1);
        }
        if( bits_L < required_bits_l ){
            printf("exceed # of bits of L[i], L[i] needs %d bits\n", (int)required_bits_l );
            exit(1);
        }

        //----- show status -----
//        printf("n: %d\n", (int)n);
//        printf("# of bits in B, that are covered by L[i]:%d\n", (int)l);
//        printf("# of bits in B, that are covered by S[i]:%d\n", (int)s);
//        printf("len(B): %d, # of bits of B[i]: %d\n", (int)len_B, (int)b);
//        printf("len(S): %d, # of bits of S[i]: %d (%d bits are required)\n", \
//                (int)len_S, (int)bits_S, (int)required_bits_s );
//        printf("len(L): %d, # of bits of L[i]: %d (%d bits are required)\n", \
//                (int)len_L, (int)bits_L, (int)required_bits_l );
//        printf("len(P): %d  # of bits of P[i]: %d (%d bits are required)\n", \
//                (int)len_P, (int)bits_P, (int)required_bits_p);
//
        //----- reserve mamories for B, L, S, and P -----
        B.resize( len_B, 0 );
        L.resize( len_L );
        S.resize( len_S );
        P.resize( len_P );

        //----- initialize P[0,i) -----
        for(int i=0; i < len_P; ++i){
            P[i] = 0;
            for(int j=i; j > 0; j >>= 1){ /* check bits from right to left */
                if( LSB(j) ){ ++P[i]; }
            }
        }
    };
public:
    size_bits n;
    ~BitVector(){
    };
    BitVector(const BitVector &bv):
        bits_B(TYPE_TO_BITS(block_b)),
        b(TYPE_TO_BITS(block_b)),
        bits_P(TYPE_TO_BITS(block_p)),
        bits_S(TYPE_TO_BITS(block_s)),
        bits_L(TYPE_TO_BITS(block_l))
    {
        initialize(bv.n);
    }
    BitVector(BitContainer &bc):
        bits_B(TYPE_TO_BITS(block_b)),
        b(TYPE_TO_BITS(block_b)),
        bits_P(TYPE_TO_BITS(block_p)),
        bits_S(TYPE_TO_BITS(block_s)),
        bits_L(TYPE_TO_BITS(block_l))
    {
        //----- initialization -----
        initialize( bc.tail_idx );

        //----- create bit vector -----
        L[0] = S[0] = 0;
        for(int i=0, j=0, k=0; i < n; ++i){
            if( !( i % l ) ){
                ++j;
                if( j < len_L ){ L[j] = L[j-1]; };
            }

            if( !( i % s ) ){
                if( !( i % l ) ){ S[k] = 0; } /* if the index of L, j is also incremented */
                ++k;
                if( k < len_S ){ S[k] = S[k-1]; }
            }

            if( bc.access(i) ){
                B[ DIVIDE8(i) ] |= 1 << MOD8(i); /* regist bits from right to left */
                if( j < len_L ){ ++L[j]; }
                if( k < len_S ){ ++S[k]; }
            }
        }
    };
    BitVector(const char* _B):
        bits_B(TYPE_TO_BITS(block_b)),
        b(TYPE_TO_BITS(block_b)),
        bits_P(TYPE_TO_BITS(block_p)),
        bits_S(TYPE_TO_BITS(block_s)),
        bits_L(TYPE_TO_BITS(block_l))
    {
        //----- initialization -----
        initialize( (size_bits)strlen(_B) );

        //----- create bit vector -----
        L[0] = S[0] = 0;
        /* i: the index of _B ('i' is NOT the position of a bit),
         * j: the index of L,
         * k: the index of S */
        for(int i=0, j=0, k=0; i < n; ++i){
            if( !( i % l ) ){
                ++j;
                if( j < len_L ){ L[j] = L[j-1]; };
            }

            if( !( i % s ) ){
                if( !( i % l ) ){ S[k] = 0; } /* if the index of L, j is also incremented */
                ++k;
                if( k < len_S ){ S[k] = S[k-1]; }
            }

            if( _B[i] - '0' ){
                /* |-B[i+1]-|-B[ i ]-|
                 * |87654321|87654321|*/
                B[ DIVIDE8(i) ] |= 1 << MOD8(i); /* regist bits from right to left */
                if( j < len_L ){ ++L[j]; }
                if( k < len_S ){ ++S[k]; }
            }
        }
    };

    const char access(int i){
        return (B[ DIVIDE8(i) ] & ( 1 << MOD8(i) )) ? '1' : '0';
    };

    int rank(int i){
        // (1) extract bits in B[i], that are covered by S[i]
        ULL bitseq = 0;
        int st = s * (i / s), en = st + s;
        for(int j=0; j<s; j++){
            if( B[ DIVIDE8(j + st) ] & ( 1 << MOD8(j + st) ) ){ bitseq |= (1 << j); }
        }

        // (2) create a mask with the same width of the bits
        ULL mask = (1 << (i % s)) - 1;
        //ULL mask = 0;
        //for(int j=(i % s)-1; j >= 0; j--){ mask |= (1 << j); }

        return L[ i / l ] + S[ i / s ] + P[ (int)(bitseq & mask) ];
    };

    int select(int i){
        int s=0, e=n, m, r;
        while(s != e){
            m = (s+e)/2;
            r = rank(m+1);
            if( i < r ){
                e = m;
            }else{
                s = m + 1;
            }
        }

        return s;
    }

    block_s set(int i, char b){
        if( b - '0' > 0 ){
            return B[ DIVIDE8(i) ] |= 1 << MOD8(i);
        }else{
            return B[ DIVIDE8(i) ] &= ~( 1 << MOD8(i) );
        }
    };
};/*}}}*/

int test_for_bitvector(){/*{{{*/
    ifstream ifs("./data/bits.dat", ifstream::in);
    string str;
    ifs >> str;
    const char* B = str.c_str();
    clock_t s_time, e_time;
    double duration;
    bool result;

    s_time = clock();
    // <construst an instance>
    BitVector bv = BitVector(B);
    // </construst an instance>
    e_time = clock();
    duration = (double)(e_time - s_time) / (double)CLOCKS_PER_SEC;
    printf("construction: %f [s]\n", duration);

    s_time = clock();
    // <a test for access>
    result = true;
    for(int i=0; i<strlen(B); ++i){
        if( bv.access(i) != B[i] ){
            printf("bv[%d]=%c, B[%d]=%c\n", i, bv.access(i), i, B[i]);
            result = false;
        }
    }
    if(result){ printf("Access(i) test: clear.\n"); }
    else{exit(1);}
    // </a test for access>
    e_time = clock();
    duration = (double)(e_time - s_time) / (double)CLOCKS_PER_SEC;
    printf("Acsess(i) test: %f [s]\n", duration);

    s_time = clock();
    // <a test for rank>
    result = true;
    int naive=0;
    for(int i=0; i<strlen(B); ++i){
        if( i > 0 ){ naive += ( B[i-1] - '0' ) ? 1 : 0; }
        int r = bv.rank(i);
        if( r != naive ){
            printf("rank(B,%d)=%d (counted naively:rank(B,%d)=%d)\n", i, r, i, naive);
            result = false;
        }
    }
    if(result){ printf("Rank(i) test: clear.\n"); }
    else{exit(1);}
    // <a test for rank>
    e_time = clock();
    duration = (double)(e_time - s_time) / (double)CLOCKS_PER_SEC;
    printf("Rank(i) test: %f [s]\n", duration);

    s_time = clock();
    // <a test for select>
    result = true;
    for(int i=0, k=0, s=0; i<strlen(B); ++i){
        if(B[i] - '0'){
            if( i != (s = bv.select(k)) ){
                printf("select(B,%d)=%d (counted naively:select(B,%d)=%d)\n", k, s, k, i);
                result = false;
            }
            k++;
        }
    }
    if(result){ printf("Select(i) test: clear.\n"); }
    else{exit(1);}
    // </a test for select>
    e_time = clock();
    duration = (double)(e_time - s_time) / (double)CLOCKS_PER_SEC;
    printf("Select(i) test: %f [s]\n", duration);

    s_time = clock();
    // <a test for set>
    result = true;
    for(int i=0; i<strlen(B); ++i){
        int ir = rand();
        char b = ((ir*ir) & 1) + '0';
        bv.set(i, b);
        if( bv.access(i) != b ){
            printf("bv[%d]=%c, r=%c\n", i, bv.access(i), b);
            result = false;
        };
    }
    if(result){ printf("Set(i,b) test: clear.\n"); }
    else{exit(1);}
    // </a test for set>
    e_time = clock();
    duration = (double)(e_time - s_time) / (double)CLOCKS_PER_SEC;
    printf("Set(i,b) test: %f [s]\n", duration);

    return 0;
};/*}}}*/

int main(){
    test_for_bitvector();
}
/* vim:set foldmethod=marker commentstring=//%s : */
