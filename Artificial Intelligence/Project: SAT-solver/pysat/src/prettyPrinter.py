from satutils import *
from sattypes import *


def printTrail(solver):
    currentLevel = 0
    print("  Trail | Level | Literal  ({dec:d} assignments and {lev:d} decision)".format(dec=len(solver._trail), lev=len(solver._trailLevels)))
    print("--------------------------")
    for i,l in enumerate(solver._trail):
        if len(solver._trailLevels) > currentLevel and solver._trailLevels[currentLevel] == i: currentLevel += 1
        print(("{p:1s}{ind:6d} | {level:5d} | {lit:5d}".format(ind=i, level=currentLevel, lit=litToInt(l), 
               p=" " if not i==solver._trailIndexToPropagate else "*")), end="")
        if solver._reason[litToVar(l)] is not None:
            print(" r=[", end="")
            for l in solver._reason[litToVar(l)]:
                print("{l:5d} ".format(l=litToInt(l)), end="")
            print("]")
        else:
            print("")

def printClauses(solver, assigned = None, value = None):
    nb = 1
    for c in solver._clauses:
        print("{n:3d}: ".format(n=nb), end="")
        nb += 1
        for l in c:
            if assigned is not None and value is not None and assigned(l):
                if value(l):
                    print("\033[1;32m", end="")
                else:
                    print("\033[1;31m", end="")
            print("{l:4d}".format(l=litToInt(l)), end="")
            if assigned is not None and assigned(l): print("\033[0m",end="")
            print(" ", end="")
        print("")


def printOccurrences(solver):
    for i in range(0,solver._nbvars):
        for s in range(0,2):
            l = varToLit(i,s)
            print(" Lit " + str(litToInt(l)) + " (" + str(l) +") : " + str(len(solver._occurences[l])) + " items")
            for c in solver._occurences[l]:
                print(str(c) + "-> " + str(c.literals))

def printWatches(solver):
    for i in range(0,solver._nbvars):
        for s in range(0,2):
            l = varToLit(i,s)
            print(" Lit " + str(litToInt(l)) + " (" + str(l) +") : " + str(len(solver._watches[l])) + " items")
            for c in solver._watches[l]:
                print(str(c) + "-> " + str(c.literals))

