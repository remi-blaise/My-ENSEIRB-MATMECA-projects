import pdb # debugger
from array import *
import time, sys

# My imports
from satutils import *
from sattypes import *
from satheapq import *
from prettyPrinter import *

# The banner
thisispysat = '''
   ___         ____ ___  ______
  / _ \ __ __ / __// _ |/_  __/
 / ___// // /_\ \ / __ | / /   
/_/    \_, //___//_/ |_|/_/    (DPLL)
      /___/                    
'''

def readFile(solver, filename):
    ''' A very python-like parser for CNF files (probably too nested I fear)'''
    starttime = time.time()
    print("c Opening file {f:s}".format(f=filename))
    
    for line in myopen(filename):
        if not line[0] in ['c','p']:
            solver.addClause([l for l in list(map(int,line.split())) if l is not 0]) 

    print("c File readed in {t:03.2f}s".format(t=time.time()-starttime))

class Constants():
    lit_False = 0
    lit_True = 1
    lit_Undef = 2

cst = Constants()

class Configuration():
    ''' Contains all the configuration variables for the solver '''
    varDecay = 0.95
    default_value = cst.lit_False    # default value for branching
    verbosity = 1
    printModel = False

     
class Solver():
    ''' Some function names are taken from the Minisat interface '''
    _config = None            # Configuration of this solver
    _nbvars = 0               # Number of variables
    _scores = MyArray('d')    # Array of doubles, score (VSIDS) of each variable (not literals)
    _scoreLit = MyArray('d')  # Array of doubles, score (VSIDS) of each literals
    _varHeap = None           # Heap (that can update scores) of variables
    _clauses = []             # Simply the list of initial clauses
    _reason = MyList()        # _reason[v] is the clause that propagated the literal v or -v (or None if v,-v was a decision)
    _occ = MyList()           # list of clauses in which this literal occurs
    _values = MyArray('b')    # Current assigned values for each variable (in Constants())
    _level = MyArray('I')     # decision level of this assigned variable

    _finalModel = []          # the model (if SAT) will be copied in this array of variables)

    # statistics
    _conflicts = 0          # total number of conflicts
    _restarts = 0
    _propagations = 0       # total number of propagations
    _occInspections = 0     # number of inspected clauses during propagations
    _sumDecisionLevel = 0

    # Propagation Queue
    _decisions = []            # redundant with the following representation, but a simple stack of decisions
    _trail = MyList()          # trail representing the current partial assignment (trail of literals)
    _trailLevels = MyList()    # Splits the trail in levels
    _trailIndexToPropagate = 0 # Handles the propagation queue. Literals in _trail (strictly) above are already propagated

    def __init__(self, config=None):
        if config == None:
            config = Configuration()
        self._time0 = time.time()
        self._config = config
        self._varHeap = SatHeapq(lambda x,y: self._scores[x] > self._scores[y]) 
        return
    
    def _valueLit(self, l):
        v,s = litToVarSign(l)
        if self._values[v] is cst.lit_Undef: return cst.lit_Undef
        if s: return cst.lit_False if self._values[v] is cst.lit_True else cst.lit_True
        return self._values[v]

    def _pickBranchLit(self):
        ''' Returns the literal on which we must branch. None if no more
        literals are unassigned '''
        v = None
        while len(self._varHeap._heap) > 0:
            v = self._varHeap.removeMin()
            if self._values[v] == cst.lit_Undef: break
        if v == None or self._values[v] != cst.lit_Undef: return None
        return varToLit(v, True)
    
    def _cancelUntil(self, level = 0):
        ''' Backtrack to the given level (undoing everything) '''
        if len(self._trailLevels) <= level: return
        x = len(self._trail) - 1
        while x > self._trailLevels[level] - 1:
            l = self._trail[x]
            self._values[litToVar(l)] = cst.lit_Undef
            if not self._varHeap.inHeap(litToVar(l)):
                self._varHeap.insert(litToVar(l))
            x -= 1
        del self._trail[self._trailLevels[level] - len(self._trail):]
        self._trailIndexToPropagate = self._trailLevels[level]
        del self._trailLevels[level - len(self._trailLevels):]
        
    def _newDecisionLevel(self):
        self._trailLevels.append(len(self._trail))

    def _decisionLevel(self):
        return len(self._trailLevels)
    
    def _uncheckedEnqueue(self, l, r=None):
        ''' Enqueue a literal l to the propagation queue.
            This is unchecked in the sense that no contradiction can be detected'''
        v,s = litToVarSign(l)
        assert self._values[v] == cst.lit_Undef # Checks that the literal was not already assigned
        self._values[v] = cst.lit_False if s else cst.lit_True
        self._reason[v] = r
        self._level[v] = self._decisionLevel()
        self._trail.append(l)

    def _newlyTrueSATClauses(self, lp):
        ''' Returns the clauses in occurences of lp that are not true by lp '''
        toreturn = []
        for c in self._occ[l]:
            alreadyTrue = False

    def _computeScore(self, size):
        return 1<<size

    def _compteScoreVar(self, sp, sn):
        return sp * sn + sp + sn

    def _propagate(self):
        ''' Can return a conflict or None '''
        heuristicsToUpdate = [] # Set of variables that need to have a recomputed heuristics
        while self._trailIndexToPropagate < len(self._trail):
            litToPropagate = self._trail[self._trailIndexToPropagate]
            self._propagations += 1
            self._trailIndexToPropagate += 1

            # Heuristics: take into account newly satisfied clauses
            for c in self._occ[litToPropagate]:
                if not c.dll_isSAT:
                    self._trailSATClauses.append(c)
                    c.dll_isSAT = True
                    for l in c: # Update the scores of all this literals
                        if l != litToPropagate:
                            self._scoreLit[l] -= self._computeScore(c.size)
                            if not heuristicsToUpdate.contains(litToVar(l)): heuristicsToUpdate.append(litToVar(l))

            for c in self._occ[notLit(litToPropagate)]:
                clauseSize = 0
                unaryClause = False
                onlyLitUndef = None
                if c.dll_isSAT: continue
                for l in c:
                    if litToVar(l) != litToVar(litToPropagate) and self._valueLit(l) == cst.lit_True: 
                        unaryClause = False # Don't propagate anything, the clause is satisfied
                        onlyLitUndef = l    # dummy assignment
                        break
                    if self._valueLit(l) is cst.lit_Undef:
                        clauseSize += 1
                        if not unaryClause and onlyLitUndef is None:
                           unaryClause = True
                           onlyLitUndef = l
                        else: # We found more than one undef literal in the clause
                           unaryClause  = False
                if unaryClause:
                    assert onlyLitUndef is not None
                    self._uncheckedEnqueue(onlyLitUndef, c)
                elif onlyLitUndef is None: # This is an empty clause
                else: # The clause is not true, not unary and not empty: its size is reduced by 1 and we need to update the heuristics!
                    for l in c:
                        if litToVar(l) != litToVar(litToPropagate) and self._valueLit(l) == cst.lit_Undef
                    return c
        return None
  
    def addClause(self, listOfInts):
        self._clauses.append(Clause([intToLit(l) for l in listOfInts]))
        self._nbvars = max(self._nbvars, max(abs(i) for i in listOfInts))

    def buildDataStructure(self): 
        starttime = time.time()

        self._values.growTo(self._nbvars, cst.lit_Undef)
        for e in [self._values, self._scores, self._reason, self._level]:
            e.growTo(self._nbvars)
        self._occ.growTo(self._nbvars * 2, [])
        
        for c in self._clauses:
            if len(c)==1:
                self._uncheckedEnqueue(c[0]) #FIXME I need to check here if there is a contradiction
            for l in c: 
                self._occ[l].append(c)
                self._scores[litToVar(l)] += 1 # each occ count as 1 
        for i in range(0,self._nbvars): self._varHeap.insert(i)
        print("c Building data structures in {t:03.2f}s".format(t=time.time()-starttime))
        print("c Ready to go with {v:d} variables and {c:d} clauses".format(v=self._nbvars, 
                  c=len(self._clauses)))
 
    def _checkRestart(self):
        ''' Checks if a restart is needed '''
        return False
    
    def _printState(self):
        printTrailStructure(self)
        printClauses(self, assigned = lambda x: self._valueLit(x)!=cst.lit_Undef, value=lambda x:self._valueLit(x)==cst.lit_True)

    def _search(self, budget=None):
        conflictC = 0                                              # Number of conflicts for this search

        while budget is None or conflictC < budget:
            confl = self._propagate()
            if confl is not None: # We reached a conflict
                conflictC += 1; self._conflicts += 1
                self._sumDecisionLevel += self._decisionLevel()
                if self._conflicts % 100 == 0:
                    print("c conflict " + str(self._conflicts)) 
                if self._decisionLevel() is 0: return cst.lit_False # We proved UNSAT
                lastDecision = self._trail[self._trailLevels[self._decisionLevel()-1]]
                self._cancelUntil(self._decisionLevel()-1)
                self._uncheckedEnqueue(notLit(lastDecision))
            else:   # No conflict
                if self._checkRestart(): return cst.lit_Undef
                l = self._pickBranchLit()
                if l == None: return cst.lit_True
                self._newDecisionLevel()
                self._uncheckedEnqueue(l)
        return cst.lit_Undef
            
    def solve(self):
        self._time1 = time.time()
        try:
            self._status = cst.lit_Undef
            self._restarts = 0
            while self._status == cst.lit_Undef:
                self._restarts += 1
                self._status = self._search(100)
        except KeyboardInterrupt:
            self._searchTime = time.time() - self._time1
            print("c Interrupted")
            return cst.lit_Undef

        self._searchTime = time.time() - self._time1
        return self._status

    def printStats(self):
        if self._conflicts == 0:
            print("c conflicts: 0")
            return
        print("c conflicts: " + str(self._conflicts) + " (" + str(int(self._conflicts /self._searchTime)) + "/s)")
        print("c cpu time: \033[1;32m{t:03.2f}\033[0ms (search={ts:03.2f}s)".format(t=time.time()-self._time0, ts=self._searchTime)) 
        print("c restarts: " + str(self._restarts))
        print("c propagations: " + str(self._propagations) + " (" + str(int(self._propagations / self._searchTime)) + "/s)")
        print("c Avg Decision Levels: " + str(int(self._sumDecisionLevel / self._conflicts)))



def printUsage():
    print("pysat solver: learning clause learning algorithms (slowly learning things).")


# Main
def banner():
    print('\n'.join([ 'c \033[1;31m' + line + '\033[0m' for line in thisispysat.split('\n')]))
    print("c                               \033[1;33mThis is pysat 0.1 (L. Simon 2016)\033[0m\nc")
    print("c (slowly) learning CDCL algorithms (roughly 10-50x slower than plain C/C++ CDCL implementations)")
    print("c          but this is a native Python implementation. Easy to play with!")
    
banner()
solver = Solver()

if len(sys.argv) > 1:
    readFile(solver, sys.argv[1])
    solver.buildDataStructure()
else:
    printUsage()
    sys.exit(1)

result = solver.solve()
if result == cst.lit_False:
    print("c UNSATISFIABLE")
elif result == cst.lit_True:
    print("c SATISFIABLE")
else:
    print("c UNKNOWN")
solver.printStats()

if result == cst.lit_True and solver._config.printModel: # SAT was claimed
    #print("v ", end="")
    for v in range(0, len(solver._values)):
        val = solver._values[v]
        assert val is not cst.lit_Undef
        solver._finalModel.append(val==cst.lit_True)
        if val==cst.lit_False: print("-")#, end="")
        print(str(v+1)+ " ")#, end="")
    print("")

if result == cst.lit_False:
    sys.exit(20)
if result == cst.lit_True:
    sys.exit(10)


