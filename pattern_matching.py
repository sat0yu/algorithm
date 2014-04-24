#coding=utf-8
from __future__ import division, print_function, unicode_literals
from future_builtins import *

import timeit

def Naive(T, P):
    i, N, M = 0, len(T), len(P)
    pos = []
    while i < N - M:
        j = 0
        while j < M:
            if T[i+j] != P[j]: break
            j += 1
        if j == M:
            pos.append(i)

        i += 1

    return pos

def KnuthMorrisPratt(T, P):
    pos = []
    # create table
    Q = [-1,0]
    for i in range(2,len(P)):
        suffix_set = [ P[i-j:i] for j in range(1,2+Q[i-1]) ]
        max_match = 0
        for suffix in suffix_set:
            matched = 0
            for k,x in enumerate(suffix):
                if x != P[k]:
                    break
                else:
                    matched = k+1
            if matched > max_match: max_match = matched
        Q.append(max_match)

    # search
    i, m = 0, 0
    while m + i < len(T):
        if P[i] == T[m + i]:
            i += 1
            if i == len(P):
                pos.append(m)
                i = 0
                m += 1
        else:
            m += i - Q[i]
            if i > 0:
                i = Q[i]
    return pos

def BoyerMoore(T, P):
    N, M = len(T), len(P)
    # construct shift table
    table = {}
    for i,c in enumerate(P[:-1]):
        table[c] = (M-1) - i
    if not table.has_key( P[-1] ):
        table[ P[-1] ] = M

    # serach
    pos = []
    i = M - 1
    while i < N:
        j = 0
        while j < M:
            if T[i-j] != P[(M-1)-j]: break
            j += 1
        if j == M:
            pos.append(i-j+1)
            i += M
        else:
            i += M if not table.has_key(T[i-j]) else table[ T[i-j] ]

    return pos

if __name__ == '__main__':
    T = "this it the sample text from which Search methods search the Pattern."
    T += "Naive Search Algorithm needs O(NM) comparison."
    T += "KnuthMorrisPratt Search Algorithm can search only with O(N+M) comparison."
    T += "BoyerMoore Search Algorithm also can search only with O(N+M) comparison."
    P = "Search"

    print("Naive(T,P): ", Naive(T,P))
    print("KnuthMorrisPratt(T,P): ", KnuthMorrisPratt(T,P))
    print("BoyerMoore(T,P): ", BoyerMoore(T,P))

    t1 = timeit.Timer('Naive(T,P)', 'from __main__ import T, P, Naive')
    t2 = timeit.Timer('KnuthMorrisPratt(T,P)', 'from __main__ import T, P, KnuthMorrisPratt')
    t3 = timeit.Timer('BoyerMoore(T,P)', 'from __main__ import T, P, BoyerMoore')
    print("Naive: ", t1.timeit(10000))
    print("KnuthMorrisPratt: ", t2.timeit(10000))
    print("BoyerMoore: ", t3.timeit(10000))
