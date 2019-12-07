
from satutils import *

# int representations of lits are what the solver reads (-1, +1, -2, +2, ...)
# literals are an internal representation from 0..2n (0=1, 1=-1, 2=2, 3=-2, 4=3, 5=-3, ...)
# This is suitable for array indexing
def intToLit(i):
    return ((abs(i)-1) << 1) + (1 if i < 0 else 0)

def litToInt(l):
    ''' Function for getting external (user) literals indexing (-N..+N) from internal literal indexing (0..2N-1)'''
    if l & 1: # sign is set
       return - (l >> 1) - 1
    return (l >> 1) + 1

def varToInt(v): return v+1

# Vars are variable indexes suitable for array indexing (0...n-1)
def varToLit(v, sign=0):
    return (v << 1) + sign

def signLit(l):
    return l%2 

def notLit(l):
    return l ^ 1

def litToVar(l):
    return l >> 1

def litToVarSign(l):
    return litToVar(l), signLit(l)

############################################################################################
class Clause():
    ''' Very simple clause wrapper.
    TODO: Needs to add a sorting technique for building the clause'''
    def __init__(self, listOfLiterals = None, learnt = False, lbd = None):
        self.literals = array('i')
        self.score = 0.0
        self.learnt = learnt
        self.dll_isSAT = False
        self.dll_size = len(listOfLiterals)
        if listOfLiterals is not None:
           self.literals.fromlist(listOfLiterals)
        return
    def addLiteral(self, lit):
        self.literals.append(lit)
    def removeLiteral(self, lit):
        self.literals.remove(lit)
    def containsLiteral(self,lit):
        return self.literals.contains(lit)
    def incScore(self):
        self.score += self.var_inc
    def getScore(self):
        return self.score
    def _calcAbstraction(self):
        ''' Computes a simple Bloom filter for the clause. Will be used when we'll preprocess the formulas'''
        filter = 0
        for i in range(0, len(literals)):
            filter &= (l[i] << (i % 64))
    # We (re)define now some classical Python methods
    def __iter__(self):
        ''' Allows to use the iterator from the array import '''
        return self.literals.__iter__()
    def __str__(self):
        ''' Gets the clause as a list of literals '''
        return ",".join(list(map(lambda l: str(litToInt(l)), self.literals)))
    def __getitem__(self, x):
        return self.literals[x]
    def __setitem__(self, x, itm):
        self.literals[x] = itm
    def __len__(self):
        return len(self.literals)
    def __lt__(self, other):
        return self.score < other.score


