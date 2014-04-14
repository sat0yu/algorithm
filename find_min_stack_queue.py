#coding: utf-8;
from __future__ import division, print_function, unicode_literals
from future_builtins import *

import random

class Stack:
    class Elm:
        def __init__(self, i, stack):
            self.val = i
            self.prev = stack.findmin()

    def __init__(self):
        self._stack = []
        self._min = None

    def push(self, i):
        self._stack.append( Stack.Elm(i, self) )
        if self._min is None or i < self.findmin():
            self._min = i

    def pop(self):
        ret = None
        if len(self._stack) > 0:
            ret = self._stack[-1].val
            self._min = self._stack[-1].prev
            self._stack = self._stack[:-1]
        return ret

    def findmin(self):
        return self._min

class Stack:
    pass

if __name__ == "__main__":
    #random.seed(1)
    s = Stack()
    print("s.findmin()", s.findmin())
    print("s.pop:", s.pop())
    randint = range(10)
    random.shuffle(randint)
    for i in randint:
        s.push(i)
        print("s.push(%d)" % i, [j.val for j in s._stack])
        print("s.findmin()", s.findmin())
        if random.randint(0,1):
            print("s.pop:", s.pop())
