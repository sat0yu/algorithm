#coding=utf-8

import numpy as np
import pylab

def normalize(X):
    # num of column
    ncol = X.shape[1]

    # normalize
    mu = np.mean(X, axis=0)
    sig = np.std(X, axis=0)
    for i in range(ncol):
        X[:,i] = (X[:,i] - mu[i]) / sig[i]

    return X

def gaussian(x, mu ,sigma):
    Z = 1 / ( (2 * np.pi) ** (x.size / 2) )
    Z *= 1 / (np.linalg.det(sigma) ** 0.5)
    return Z * np.exp( -0.5 * np.dot( np.dot(x-mu, np.linalg.inv(sigma)), x-mu ) )

def likelihood(X, pi, mu, sigma):
    N = len(X)
    conf_p_X = np.zeros(N)
    for n in range(N):
        for k in range(K):
            conf_p_X[n] += pi[k] * gaussian(X[n], mu[k], sigma[k])
    return np.sum( np.log(conf_p_X) )

if __name__ == '__main__':
    ## setting
    datapath = 'data/faithful.txt'
    K = 2 # num of gauss elem.
    N = 0 # num of data
    D = 0 # dim of data

    ## load data and normalize it
    X = np.genfromtxt(datapath)
    N = len(X)
    D = X.shape[1]
    print 'data has %d dimentions' % D
    
    X = normalize(X)
    print X

    ## initialize
    # mu: means
    # sigma: cov-matrix
    # pi: mixing coefficient
    # gamma: burden rate
    mu = np.random.rand(K, D)
    print mu
    
    sigma = range(K)
    for k in range(K):
        sigma[k] = np.identity(D)
    print sigma
    
    pi = np.random.rand(K)
    print pi

    gamma = np.zeros((N, K))
    print gamma
    
    
    ## EM Algorithm
    # llh: log( p(X|mu, pi, sigma) )
    # step: upper bound of counter
    llh = likelihood(X, pi, mu, sigma)
    step = 20
    counter = 0
    while counter < step:
        ## E-step
        for n in range(N):
            # calc denominator (common)
            denominator = 0.
            for j in range(K):
                denominator += pi[j] * gaussian(X[n], mu[j], sigma[j])
                
            # calc buren rate for each k
            for k in range(K):
                gamma[n][k] = pi[k] * gaussian(X[n], mu[k], sigma[k]) / denominator

        ## M-step
        for k in range(K):
            # calc Nk
            Nk = np.sum( gamma[:,k] )

            # calc mu
            print mu[k]
            mu[k] = np.zeros(D)
            for n in range(N):
                mu[k] += gamma[n][k] * X[n]
            mu[k] /= Nk

            # calc sigma
            sigma[k] =  np.zeros((D, D))
            for n in range(N):
                diff = np.matrix( X[n] - mu[k] )
                sigma[k] += gamma[n][k] * ( diff.T * diff )
            sigma[k] /= Nk

            # calc pi
            pi[k] = Nk / N

        # print condition
        print "%d\t likelihood:%f" % (counter, llh)
        llh = likelihood(X, pi, mu, sigma)
        counter += 1

    # draw means for each gauss dist.
    for k in range(K):
        pylab.scatter(*mu[k], color='r', marker='o')

    # draw contour
    S = np.linspace(-2.5, 2.5, 50)
    T = np.linspace(-2.5, 2.5, 50)
    s,t = np.meshgrid(S,T)
    for k in range(K):
        sigmax = np.sqrt( sigma[k][0,0] ) # std. devariation
        sigmay = np.sqrt( sigma[k][1,1] ) # std. devariation
        sigmaxy = sigma[k][0,1] # codevariation
        z = pylab.bivariate_normal(s, t, sigmax, sigmay, mu[k][0], mu[k][1], sigmaxy)
        cs = pylab.contour(s, t, z, 3, colors='k', linewidths=1)    
    
    # draw training data
    pylab.scatter(X[:,0], X[:,1], color='g', marker='x')

    # show graph
    pylab.xlim(-2.5, 2.5)
    pylab.ylim(-2.5, 2.5)
    pylab.show()
