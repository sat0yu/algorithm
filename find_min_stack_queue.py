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

class Queue:
    def __init__(self):
        self._que = []
        self._min_que = []

    def enqueue(self, i):
        idx = len(self._min_que)
        for j in reversed( range( len(self._min_que) ) ):
            idx = j if i < self._min_que[j] else idx
        self._min_que = self._min_que[:idx]

        self._que.append(i)
        self._min_que.append(i)

    def dequeue(self):
        if not( len(self._que) > 0 ):
            return None

        ret = self._que[0]
        self._que = self._que[1:]
        if ret == self._min_que[0]:
            self._min_que = self._min_que[1:]

        return ret

    def findmin(self):
        return None if not( len(self._min_que) > 0 ) else self._min_que[0]

if __name__ == "__main__":
    #random.seed(1)
    s = Stack()
    print("------------------------------")
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
        print("")

    #random.seed(1)
    q = Queue()
    print("------------------------------")
    print("q.findmin()", q.findmin())
    print("q.dequeue():", q.dequeue())
    randint = range(10)
    random.shuffle(randint)
    for i in randint:
        q.enqueue(i)
        print("q.enqueue(%d)" % i, q._que)
        print("q.findmin()", q.findmin())
        if random.randint(0,1):
            print("q.dequeue():", q.dequeue())
        print("")
