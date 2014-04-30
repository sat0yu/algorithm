#coding: utf-8;
from __future__ import division, print_function, unicode_literals
from future_builtins import *

import math

class MaximumBinaryHeap:
    def __init__(self):
        self.array = []

    def print_dump(self):
        if len(self.array) > 0:
            height = int( math.log(len(self.array),2) ) + 1
            for i in range(height):
                right = 2**i - 1
                print(self.array[ right : (right + 2**i) ] )
        else:
            print(self.array)

    def up_heap(self, obj):
        self.array.append(obj)

        if len(self.array) is 1: return None

        piv = len(self.array)
        i = int( math.floor(piv / 2) )
        while i > 0 and self.array[piv-1] > self.array[i-1]:
            temp = self.array[i-1]
            self.array[i-1] = self.array[piv-1]
            self.array[piv-1] = temp
            piv = i
            i = int( math.floor(piv / 2) )

    def down_heap(self):
        returned = self.array[0]
        self.array[0] = self.array[-1]
        del self.array[-1]

        if not( len(self.array) > 1 ): return returned

        piv = 1
        i = 2 if ( len(self.array) is 2 ) or ( self.array[1] > self.array[2] ) else 3
        while self.array[piv-1] < self.array[i-1]:
            temp = self.array[i-1]
            self.array[i-1] = self.array[piv-1]
            self.array[piv-1] = temp

            piv, right_child = i, i*2
            ## if pivot node has only a child
            if right_child is len(self.array):
                i = right_child
            ## if pivot node has two children
            elif right_child < len(self.array):
                ## select a bigger child to maintain the heap-structure
                if self.array[ right_child-1 ] > self.array[ (right_child+1)-1 ]:
                    i = right_child
                else:
                    i = right_child + 1
            ## if pivot node is reaf node
            else:
                return returned

        return returned

    def insert(self, obj): self.up_heap(obj)
    def append(self, obj): self.up_heap(obj)
    def pop(self): return self.down_heap()
    def remove(self): return self.down_heap()

if __name__ == "__main__":
    h = MaximumBinaryHeap()
    for i in range(16):
        h.append(i)
    h.print_dump()

    for j in range(4):
        for i in range(4):
            print(h.pop())
        h.print_dump()
