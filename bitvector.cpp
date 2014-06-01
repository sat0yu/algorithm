#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<stdlib.h>
#include<string.h>
#include<math.h>

using namespace std;

typedef unsigned char sBlock;
typedef unsigned short lBlock;

class BitVector{
private:
    int n, l, s, len_p;
    size_t sBLC, lBLC;
    vector<sBlock> B, S, P;
    vector<lBlock> L;
public:
    ~BitVector(){
    };
    BitVector(const char* _B){
        // initialize constants
        sBLC = 8 * sizeof(sBlock);
        lBLC = 8 * sizeof(lBlock);
        printf("the size of a smaller block: %d\n", sBLC);
        printf("the size of a larger block: %d\n", lBLC);

        // create bit vector
        n = strlen(_B);
        printf("n=%d\n", n);

        B = vector<sBlock>( n >> 3 );
        for(int i=0; i<n; ++i){
            if( _B[i] - '0' ){ B[ i >> 3 ] |= 1 << (i & 0x07); }
        }

        // create Fully Indexable Dictionary
        int log2n = (int)ceil( log2(n) );
        // s = lg(n)/2 bits are covered by S[i]
        // S[i] can be stored in lg(l) (bits)
        s = ceil( log2n / 2. );
        // l = lg^2(n) bits are covered by L[i]
        // L[i] can be stored in lg(n) (bits)
        l = log2n * log2n;
        // make the size of l be a multiple of s
        while( l % s ){ l++; }
        // |P[0, 2^s)|
        len_p = 1 << s;

        printf("l=%d, sizeof(L[i])=%d(bits)\n", l, (int)ceil(log2(n)));
        printf("s=%d, sizeof(S[i])=%d(bits)\n", s, (int)ceil(log2(l)));
        printf("|P|=%d\n", len_p);

        L = vector<lBlock>( (int)ceil( n / (double)l ) );
        S = vector<sBlock>( (int)ceil( n / (double)s ) );
        P = vector<sBlock>( len_p );

        // initialize L[0,n/l), S[0,n/s)
        L[0] = S[0] = 0;
        for(int i=0,j=0,k=0; i < n; ++i){
            if( !( i % l ) ){
                ++j;
                L[j] = L[j-1];
            }

            if( !( i % s ) ){
                if( !( i % l ) ){ S[k] = 0; }
                ++k;
                S[k] = S[k-1];
            }

            if( B[ i >> 3 ] & (1 << (i & 0x07)) ){
                ++L[j];
                ++S[k];
            }

        }

        // initialize P[0,i)
        for(int i=0; i < (1 << s); ++i){
            P[i] = 0;
            for(int j=1; j <= (1 << s); j <<= 1){
                if( i & j ){ ++P[i]; }
            }
        }

    };

    const char access(int i){
        return (B[ i >> 3 ] & ( 1 << ( i & 0x07 ) )) ? '1' : '0';
    };

    int rank(int i){
        // (1) extract bits covered by S[i] but not B[i]
        sBlock bitseq = 0;
        int st = s * (i / s), en = st + s;
        //// if 'st' and 'en' are stored in the same B[i]
        if( (st >> 3) == (en >> 3) ){
            for(int j=0; j<s; ++j){
                if( B[ st >> 3 ] & ( 1 << (j + (st & 0x07) ) ) ){ bitseq |= (1 << j); };
            }
        //// if 'st' and 'en' are stored in separate sBlocks
        }else{
            for(int j=0; j<sBLC-(st & 0x07); ++j){
                if( B[ st >> 3 ] & ( 1 << (j + (st & 0x07) ) ) ){ bitseq |= (1 << j); };
            }
            for(int j=0; j<(en & 0x07); ++j){
                if( B[ en >> 3 ] & ( 1 << j ) ){ bitseq |= (1 << ( j + ( sBLC - (st & 0x07) ) )); };
            }

        }

        // (2) create a mask with the same width of the bits
        //// the following line means -> sBlock mask = 0xff >> ( (BLC - s) + (s - i % s) );
        sBlock mask = 0xff >> (sBLC - i % s);

//        cout << "L[" << i/l << "]: " << (int)L[ i / l ];
//        cout << ", S[" << i/s << "]: " << (int)S[ i / s ];
//        cout << ", P[" << (int)(bitseq & mask) << "]: " << (int)P[ (bitseq & mask) ] << endl;

        return L[ i / l ] + S[ i / s ] + P[ (bitseq & mask) ];
    };

    sBlock set(int i, char b){
        if( b - '0' > 0 ){
            return B[ i >> 3 ] |= 1 << ( i & 0x07 );
        }else{
            return B[ i >> 3 ] &= ~( 1 << ( i & 0x07 ) );
        }
    };
};

int main(){
    ifstream ifs("./data/bits.dat", ifstream::in);
    string str;
    ifs >> str;
    const char* B = str.c_str();
    BitVector bv = BitVector(B);
    bool result;

    // a test for access
    result = true;
    for(int i=0; i<strlen(B); ++i){
        if( bv.access(i) != B[i] ){
            printf("bv[%d]=%c, B[%d]=%c\n", i, bv.access(i), i, B[i]);
            result = false;
        };
    }
    if(result){ printf("Access(i) test: clear.\n"); }

    // a test for rank
    result = true;
    for(int i=0; i<strlen(B); ++i){
        int naive=0;
        for(int j=0; j<i; ++j){ naive += ( B[j] - '0' ) ? 1 : 0; }

        int r = bv.rank(i);
        if( r != naive ){
            printf("rank(B,%d)=%d (counted naively:rank(B,%d)=%d)\n", i, r, i, naive);
            result = false;
        }
    }
    if(result){ printf("rank(i) test: clear.\n"); }

    // a test for set
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
}
