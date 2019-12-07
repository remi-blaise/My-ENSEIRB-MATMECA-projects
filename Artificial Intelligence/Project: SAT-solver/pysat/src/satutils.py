from heapq import *
from array import *

import gzip, copy

# Small functions to facilitate the map integers with internal values of the solver
def sign(i):
    if i > 0: return 0
    return 1

def abs(i):
    if i > 0: return i
    return -i

def myopen(f):
    if f.endswith(".gz"):
        return gzip.open(f, mode='rt')
    return open(f, 'r')

def do_all(f, l):
    for e in l: f(e)
    
#
class MyArray(array):
    ''' My own array with growing function '''
    def __init__(self, *args):
        array.__init__(args)
    def growTo(self, size, fillWith=0):
        while (len(self)<size): self.append(fillWith)

class MyList(list):
    ''' My own list with growing function'''
    def __init__(self, *args):
        list.__init__(self, *args)
    def growTo(self, size, fillWith=0):
        while (len(self)<size): self.append(copy.copy(fillWith))

# Some useful classes to ease my algorithms
class VarHeap():

    def __init__(self, score):
        self._score = score;
        self._heap = [] # this is a heapq data structure
        self._entry = MyArray('i') # indexes of variables
        return

    def growTo(self, size):
        # don't need to do anything: I'm using heapq from python list
        return

def luby(y,x): # copied from Minisat implementation
    # Find the finite subsequence that contains index 'x', and the
    # size of that subsequence:
    # pythonic way of getting a list : [luby(2,x) for x in range(1,20)]
    size = 1; seq = 0
    while size < x + 1:
        seq += 1
        size = 2 * size + 1 

    while size-1 != x:
        size = (size-1) >> 1
        seq -= 1
        x = x % size

    return y ** seq


