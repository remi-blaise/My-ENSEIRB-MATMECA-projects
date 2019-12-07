''' heapq module inspired by the heap.h file from Minisat (see Minisat for copyrights).

    I'm not using the heapq solution from python because the update function is not
    easily handled (implies tagging an element as removed and inserting a new one).
    '''

from satutils import *

''' A heap implementation of integers with support for decrease/increase key.'''

class SatHeapq():

    def __init__(self, lt):
        self._lt = lt # Function to compare two keys
        self._heap = MyArray('i') # Heap of integers
        self._indices = MyArray('i') # Each integer position (index) in the heap

    def ___len__(self):
        return len(self._heap)

    def __getitem__(self, index):
        assert index < len(self._heap)
        return self._heap[index]

    def __contains__(self, n):
        return self.inHeap(n)

    def __truth__(self, n):
        return not self.empty()

    ''' Index traversal functions '''
    @staticmethod
    def left(i): return i*2+1
    
    @staticmethod
    def right(i): return (i+1)*2
    
    @staticmethod
    def parent(i): return (i-1) >> 1

    def _percolateUp(self, i):
        x  = self._heap[i]
        p  = SatHeapq.parent(i)
        
        while i is not 0 and self._lt(x, self._heap[p]):
            self._heap[i]                = self._heap[p]
            self._indices[self._heap[p]] = i
            i                            = p
            p                            = SatHeapq.parent(p)
        self._heap[i]    = x;
        self._indices[x] = i;

    def _percolateDown(self, i):
        x = self._heap[i]
        while SatHeapq.left(i) < len(self._heap):
            if SatHeapq.right(i) < len(self._heap) and self._lt(self._heap[SatHeapq.right(i)], self._heap[SatHeapq.left(i)]):
                child = SatHeapq.right(i)
            else:
                child = SatHeapq.left(i)
            if not self._lt(self._heap[child], x): break
            self._heap[i]          = self._heap[child]
            self._indices[self._heap[i]] = i
            i                            = child
        self._heap[i]    = x
        self._indices[x] = i

    def empty(self):
        return len(self._heap) == 0

    def inHeap(self, n):
        return n < len(self._indices) and self._indices[n] >= 0

    def decrease(self, n):
        assert self.inHeap(n)
        self._percolateUp(self._indices[n])

    def increase(self, n):
        assert self.inHeap(n)
        self._percolateDown(self._indices[n])

    def update(self, n):
         if not self.inHeap(n):
             self.insert(n)
         else:
             self._percolateUp(self._indices[n])
             self._percolateDown(self._indices[n])

    def insert(self, n):
        self._indices.growTo(n+1, fillWith=-1)
        assert not self.inHeap(n)

        self._indices[n] = len(self._heap)
        self._heap.append(n)
        self._percolateUp(self._indices[n])

    def removeMin(self):
        x                            = self._heap[0];
        self._heap[0]                = self._heap[len(self._heap)-1];
        self._indices[self._heap[0]] = 0;
        self._indices[x]             = -1;
        self._heap.pop();
        if len(self._heap) > 1: self._percolateDown(0);
        return x 




