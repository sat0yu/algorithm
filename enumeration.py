#coding: utf-8;
from __future__ import division, print_function, unicode_literals
from future_builtins import *

def BinaryEnumeration(N):
    for i in range(2**N):
        subset = [ N-j for j,bit in enumerate(format(i, "0%db" % N)) if bit == '1' ]
        print(subset)

def DevideEnumeration(X,i):
    if i == 0:
        print(X)
    else:
        DevideEnumeration(X, i-1)
        XX = X.copy()
        XX.add(i)
        DevideEnumeration(XX, i-1)

def GrayCode(N):
    X = set([])
    while not X == set([N,]):
        print(X)
        if len(X) % 2:
            X = X.symmetric_difference(set([1+min(X)]))
        else:
            X = X.symmetric_difference(set([1]))

if __name__ == "__main__":
    N = 5

    # Binary approach
    print("Binary approach")
    BinaryEnumeration(N)

    # divide approach
    print("divide approach")
    DevideEnumeration(set({}),N)

    # gray code
    print("gray code")
    GrayCode(N)
