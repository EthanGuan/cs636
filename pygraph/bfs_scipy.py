import numpy as np
from scipy.sparse import csr_matrix
from scipy.sparse.csgraph import breadth_first_tree, breadth_first_order

def read():

    ifile = "smallworld.txt"
    dd = np.zeros((1000, 1000))
    with open(ifile, 'r') as f:
        for line in f: # read rest of lines
            x = line.split()
            #print(x);
            if x[0] != "#": 
                dd[int(x[0]), int(x[1])] = 1
    return dd
dd = read()
print(dd)
X = csr_matrix(dd)
Tcsr = breadth_first_tree(X, 0, directed=False)
print(type(Tcsr))

print(len(breadth_first_order(X, 0)))