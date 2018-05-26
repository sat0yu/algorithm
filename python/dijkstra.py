#coding: utf-8;
from __future__ import division, print_function, unicode_literals
from future_builtins import *

def dijkstra(dist_map, start, end):
    # prefectures
    # ex.)
    # pref = {
    #    "A":[("B",10),("C",20)],
    #    "B":[("A",10),("C",30)],
    #    "C":[("A",20),("B",30)],
    # }
    pref = {}
    for r in dist_map:
        if not r[0] in pref.keys(): pref[ r[0] ] = []
        if not r[1] in pref.keys(): pref[ r[1] ] = []
        pref[ r[0] ].append( ( r[1], r[2] ) )
        pref[ r[1] ].append( ( r[0], r[2] ) )
    N = len( pref.items() )

    # validation
    if not start in pref.keys() or not end in pref.keys():
        raise ValueError("given locations do not exist in given map")

    # piv, fixed, considered
    piv, fixed, cons = start, {start:(0,None),}, {}
    while len(fixed.items()) < N and not( fixed.has_key(end) ):
        # append or recalc. path to prefs that connected to piv
        for p in pref[piv]:
            if not p[0] in fixed.keys():
                to_p = fixed[piv][0] + p[1]
                if not( cons.has_key( p[0] ) ):
                    cons[ p[0] ] = (to_p, piv)
                elif to_p < cons[ p[0] ][0]:
                    cons[ p[0] ] = (to_p, piv)

        # fix shortest path to piv and select next piv
        shortest = float("inf")
        for k,v in cons.items():
            if v[0] < shortest:
                piv, shortest = k, v[0]

        print(piv, cons[piv])
        fixed[piv] = cons[piv]
        del cons[piv]

    return fixed

if __name__ == "__main__":
    map = [
            ('aomori', 'iwate', 20),
            ('akita', 'aomori', 40),
            ('akita', 'iwate', 50),
            ('iwate', 'sendai', 30),
            ('iwate', 'yamagata', 60),
            ('yamagata', 'akita', 40),
            ('yamagata', 'niigata', 30),
            ('fukushima', 'niigata', 60),
            ('fukushima', 'sendai', 40),
            ('niigata', 'nagano', 60),
            ('yamagata', 'sendai', 40),
            ('gunma', 'fukushima', 30),
            ('gunma', 'nagano', 50),
            ('niigata', 'gunma', 60),
            ('tochigi', 'gunma', 20),
            ('tochigi', 'fukushima', 30),
            ('tochigi', 'ibaraki', 20),
            ('ibaraki', 'fukushima', 60),
        ]

    print("dijkstra", dijkstra(map, "nagano", "aomori"))
    print("dijkstra", dijkstra(map, "aomori", "nagano"))
    print("dijkstra", dijkstra(map, "aomori", "tochigi"))
    print("dijkstra", dijkstra(map, "aomori", "ibaraki"))
