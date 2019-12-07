# README #

This is a CDCL Python (3.X) native implementation. It is very slow (like 10x-50x times) but the data structure is a native python one, made with as many tricks as possible.
It is a nice toy to play with. 

### Learn Clause Learning Algorithms ###

The idea is to be able to quickly play with CDCL concepts.
If you understand it, you can dig into Minisat/Glucose source code now!

### How to use it? ###

Just go into the src directory and type:
''
python pysat.py ../example/sample.cnf
''


Note: the DPLL implementation is not yet working. Need to implement a proper heuristics mechanism.

Good luck!