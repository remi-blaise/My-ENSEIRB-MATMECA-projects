from satutils import *

class SatBoundedQueue():

    def __init__(self, size=100):
        self._data = MyArray('I')
        self._data.growTo(size)
        self._first = 0
        self._last = 0
        self._queueSize = 0
        self._sumOfQueue = 0
        self._maxSize = size

    def append(self, x):
        if self._queueSize == self._maxSize:
            assert self._last == self._first
            self._sumOfQueue -= self._data[self._last]
            self._last += 1
            if self._last == self._maxSize: self._last = 0
        else:
            self._queueSize += 1
        self._sumOfQueue += x
        self._data[self._first] = x
        self._first += 1
        if self._first == self._maxSize:
            self._first = 0
            self._last = 0

    def peek(self):
        return self._data[self._last]

    def pop(self):
        self._sumOfQueue -= self._data[self._last]
        self._queueSize -= 1
        self._last += 1
        if self._last == self._maxSize: self._last = 0

    def isValid(self):
        return self._queueSize == self._maxSize

    def fastClear(self):
        self._first = 0
        self._last = 0
        self._queueSize = 0
        self._sumOfQueue = 0

    def getAvg(self):
        return self._sumOfQueue / self._queueSize

    def __len__(self):
        return self._queueSize


