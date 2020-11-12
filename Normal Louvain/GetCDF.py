import numpy as np
import sys
from scipy.stats import binom

def BinomCDF(n, p):
    result = np.empty([n])
    for i in range(n):
        result[i] = binom.cdf(i, n, p) #(current - prev)
    return result


n = int(sys.argv[1])
p = float(sys.argv[2])
filename = sys.argv[3]

r = BinomCDF(n, p)

f = open(filename, "w")

for i in range(n):
    f.write(str(r[i])+"\n")

f.close()

