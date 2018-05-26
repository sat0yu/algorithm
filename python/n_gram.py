#coding: utf-8;
from __future__ import division, print_function, unicode_literals
from future_builtins import *

def find_top_ngram(sequences, N, k=1):
    tree = {}
    for seq in sequences:
        terms = seq.split()
        if len(terms) < N: continue
        for i in range( len(terms) - N + 1 ):
            v = tree
            for j in range(N-1):
                if terms[i+j] not in v:
                    v[terms[i+j]] = {}
                v = v[terms[i+j]]
            v[terms[i+N-1]] = 1 if terms[i+N-1] not in v else v[terms[i+N-1]] + 1

    return dfs(tree, N, "", k)

def dfs(tree, height, prefix, k):
    if height > 1:
        ret = []
        for e in tree.keys():
            buf = []
            pref = " ".join([prefix, e]) if len(prefix) > 0 else e
            tmp = dfs(tree[e], height - 1, pref, k)
            i, j = 0, 0
            while i < len(ret) and j < len(tmp):
                if ret[i][1] > tmp[j][1] \
                        or ( ret[i][1] == tmp[j][1] and ret[i][0] < tmp[j][0] ):
                    buf.append(ret[i])
                    i += 1
                else:
                    buf.append(tmp[j])
                    j += 1

            buf = buf + ret[i:] if i < len(ret) else buf + tmp[j:]
            ret = buf[:k]
        return ret
    else:
        ret = sorted(tree.items(), key=lambda x:x[1], reverse=True)[:k]
        return [ (" ".join([prefix, e]), v) for e,v in ret ]

if __name__ == "__main__":
    sequences = [
            "this is an apple",
            "that is an apple",
            "this is a pen",
            "that is a pen",
            "this is a banana",
            "that is a banana",
            "there is an apple",
            "there are two bananas",
            "please give me a pen",
            "my favorite fruit is banana",
            ]
    print("3-gram", find_top_ngram( sequences, 3, k=5 ))
    print("2-gram", find_top_ngram( sequences, 2, k=5 ))
