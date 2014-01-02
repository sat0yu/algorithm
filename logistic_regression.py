#coding: utf-8;
import numpy as np
from scipy.special import gamma

import pyximport
pyximport.install(setup_args={'include_dirs':[np.get_include()]}, inplace=True)
from utils import *

class LogisticRegression:
    def __init__(self, eta=1.0, decay=1.0):
        self.eta = eta
        self.decay = decay
        self.w = None

    def fit(self, X, T, N=50):
        if self.w is None:
            dim = X.shape[1]
            self.w = np.random.rand(dim + 1)

        D = np.c_[T, X]
        for i in range(N):
            np.random.shuffle(D)

            for dn in D:
                t, x = dn[0], dn[1:]

                if not(t == 0 or t == 1):
                    raise ValueError("invalid value; target label must be 0 or 1")

                # addPseudoDim:R^n -> R^(n+1)
                x = self.addPseudoDim(x)

                # update weight vec.
                y = self.sigmoid( np.dot(self.w, x) )
                self.w -= self.eta * (y - t) * x

            # validate w
            #print self.cross_entorpy_error_function(D[:,1:], D[:,0])

            # update learning-rate
            self.eta *= self.decay

    def cross_entorpy_error_function(self, D, T):
        ceef = 0
        for x,t in zip(D,T):
            x = self.addPseudoDim(x)
            y = self.sigmoid( np.dot(self.w, x) )
            ceef += np.sum( t*np.log(y) + (1-t)*np.log(1-y) )
        return -ceef

    def update(self, x, t):
        if self.w is None:
            dim = len(x)
            self.w = np.random.rand(dim + 1)

        if not(t == 0 or t == 1):
            raise ValueError("invalid value; target label must be 0 or 1")

        # addPseudoDim:R^n -> R^(n+1)
        x = self.addPseudoDim(x)

        # update weight vec.
        y = self.sigmoid( np.dot(self.w, x) )
        self.w -= self.eta * (y - t) * x

        # update learning-rate
        self.eta *= self.decay

    def addPseudoDim(self, x):
        return np.r_[x, [1,]]

    def sigmoid(self, t):
        return 1. / (1 + np.exp(-t))

def genFunc(a,b,c):
    def h(x):
        return 1 if np.dot( np.array([a,b]), x) + c > 0 else 0
    return h

# initialize random seed so as not to change data
np.random.seed(0)

D = np.random.rand(100,2)
h = genFunc(1,2,-1)
T = [h(x) for x in D]

lr = LogisticRegression(decay=0.99)
for x,t in zip(D,T):
    lr.update(x,t)
print "online", lr.w, lr.eta, lr.cross_entorpy_error_function(D, T)

lr = LogisticRegression(decay=0.99)
lr.fit(D,T,1)
print "batch", lr.w, lr.eta, lr.cross_entorpy_error_function(D, T)
