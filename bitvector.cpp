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
    int n, l, s;
    size_t sBLC, lBLC, len_l, len_s, len_p, size_l, size_s;
    vector<sBlock> B, S, P;
    vector<lBlock> L;
public:
    ~BitVector(){
    };
    BitVector(const char* _B){
        //----- initialize constants -----
        sBLC = 8 * sizeof(sBlock);
        lBLC = 8 * sizeof(lBlock);
        n = strlen(_B);
        int log2n = (int)ceil( log2(n) );
        /* s = lg(n)/2 bits are covered by S[i] */
        /* S[i] can be stored in lg(l) (bits) */
        s = ceil( log2n / 2. );
        len_s = (int)ceil( n / (double)s );
        /* l = lg^2(n) bits are covered by L[i] */
        /* L[i] can be stored in lg(n) (bits) */
        l = log2n * log2n;
        while( l % s ){ l++; }
        len_l = (int)ceil( n / (double)l );
        /* |P[0, 2^s)| */
        len_p = 1 << s;

        size_l = (int)ceil( log2(n) );
        size_s = (int)ceil( log2(l) );
        if( size_l > lBLC ){
            printf("exceed the expected number of bits for L[i], L[i] needs %d bits\n", (int)size_l);
            exit(1);
        }
        if( size_s > sBLC ){
            printf("exceed the expected number of bits for S[i], S[i] needs %d bits\n", (int)size_s);
            exit(1);
        }

        printf("the size of a smaller block: %d\n", (int)sBLC);
        printf("the size of a larger block: %d\n", (int)lBLC);
        printf("n=%d\n", n);
        printf("l=%d, sizeof(L[i])=%d(bits)\n", l, (int)size_l);
        printf("s=%d, sizeof(S[i])=%d(bits)\n", s, (int)size_s);
        printf("|P|=%d\n", (int)len_p);

        //----- reserve mamory spaces of B, L, S, and P -----
        B.resize( n >> 3 );
        L.resize( len_l );
        S.resize( len_s );
        P.resize( len_p );

        //----- create bit vector -----
        L[0] = S[0] = 0;
        for(int i=0,j=0,k=0; i < n; ++i){
            if( !( i & 0x07 ) ){ B[ i >> 3 ] = 0; }

            if( !( i % l ) ){
                ++j;
                if( j < len_l ){ L[j] = L[j-1]; };
            }

            if( !( i % s ) ){
                if( !( i % l ) ){ S[k] = 0; }
                ++k;
                if( k < len_s ){ S[k] = S[k-1]; }
            }

            if( _B[i] - '0' ){
                B[ i >> 3 ] |= 1 << (i & 0x07);
                if( j < len_l ){ ++L[j]; }
                if( k < len_s ){ ++S[k]; }
            }

        }

        //----- initialize P[0,i) -----
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

    int select(int i){
        int s=0, e=n, m, r;
        while(s != e){
            m = (s+e)/2;
            r = rank(m+1);
//            printf("i:%d,r:%d,s:%d,e:%d\n", i, r, s, e);
            if( i < r ){
                e = m;
            }else{
                s = m + 1;
            }
        }
//        printf("i:%d,r:%d,s:%d,e:%d\n", i, r, s, e);
        return s;
    }

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

    // a test for select
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
