#!/usr/bin/python

import sys
import numpy as np

name = sys.argv[1]

with open(name) as f:
    content = f.readlines()
# you may also want to remove whitespace characters like `\n` at the end of each line
content = [x.strip().split() for x in content if '#' not in x]

arr = np.array(content, int)
edges = np.sort(arr[:,:2].flatten())
vertices = np.unique(edges)

#print(edges)

degrees = np.searchsorted(edges, vertices, side='right') - np.searchsorted(edges, vertices, side='left')
degrees = np.array(degrees, int)
edeg = np.array([edges, degrees])
#print(edeg)

print("=== FILE: ", name)
print("Directed edges:", edges.size)
print("Vertices:", vertices.size)
print("Maxdeg: ", max(degrees))

