#coding=utf-8
from __future__ import division, print_function, unicode_literals
from future_builtins import *

import numpy as np

def levenshtein(S, T):
    _S, _T = len(S), len(T)
    dp = np.zeros((_S,_T), dtype=np.int)
    for i in range(_S): dp[i][0] = i
    for j in range(_T): dp[0][j] = j

    for i in range(1,_S):
        for j in range(1,_T):
            dp[i][j] = min(
                        dp[i-1][j] + 1,
                        dp[i][j-1] + 1,
                        dp[i-1][j-1] + (0 if S[i] == T[j] else 1),
                    )

    return dp[-1][-1]

if __name__ == "__main__":
    S = "sample"
    T = "simple"
    print( levenshtein(S,T) )
