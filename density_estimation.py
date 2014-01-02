#coding: utf-8;
import numpy as np
from scipy.special import gamma

import pyximport
pyximport.install(setup_args={'include_dirs':[np.get_include()]}, inplace=True)
from utils import *

class Multinomial:
    def __init__(self, alpha=None):
        # initialize hyper-parameter
        if alpha is None:
            alpha = np.ones(self.dim)
        self.alpha = alpha
        self.dim = len(alpha)
        self.E = np.identity(self.dim, dtype=np.int)
        self.m = np.zeros(self.dim, dtype=np.int)

    def prob(self, x):
        p = 1.
        mu = self.mu()
        for i,xi in enumerate(x):
            p *= mu[i]**xi
        return p

    def mu(self):
        # return the mean of Dirichlet-Dist.
        return (self.m + self.alpha) / float(sum(self.m) + sum(self.alpha))

    def estimate(self, D):
        if D.shape[1] is not self.dim:
            raise ValueError("each sample must have the same dimention as the ones of hyperparameter of Dirichlet-Dist.")

        for x in D:
            for i in range(self.dim):
                if np.dot(x, self.E[i]) == 1:
                    self.m[i] += 1

N = [10, 1, 1]
alpha = np.array([3,3,3])
dim = len(N)
E = np.identity(dim, dtype=np.int)
D = np.r_[ [E[0],]*N[0], [E[1],]*N[1], [E[2],]*N[2], ]
mult = Multinomial(alpha)
mult.estimate(D)
for i in range(dim):
    print mult.prob(E[i])
