
__GRIDSIZE__ = 4

def inBound(x):
   if x < 0 or x >= __GRIDSIZE__: return False
   return True

def around(coord):
   (x,y) = coord
   toret = []
   for (dx,dy) in [(-1,0), (0,1), (1,0), (0,-1)]:
      if inBound(dx+x) and inBound(dy+y):
         toret.append((dx+x, dy+y))
   return toret

encodings = []
def varToInt(symbol, coord):
   try:
      return str(encodings.index((symbol, coord)) + 1)
   except ValueError:
      encodings.append((symbol, coord))
      return str(len(encodings))

def printConstraint(symbol1, symbol2, coord):
   for (x,y) in around(coord):
      print("-" + varToInt(symbol1, coord) + " " + varToInt(symbol2,(x,y)) + " 0")

def printConstraintNeg(symbol1, symbol2, coord):
   for (x,y) in around(coord):
      print(varToInt(symbol1, coord) + " -" + varToInt(symbol2,(x,y)) + " 0")

def printExclusionConstraint(symbol1, symbol2, coord):
   print("-" + varToInt(symbol1,coord) + " -" + varToInt(symbol2,coord) + " 0")
   print("-" + varToInt(symbol2,coord) + " -" + varToInt(symbol1,coord) + " 0")

def printAllConstraints(coord):
   printConstraint("T", "V", coord) # A Hole implies wind all around
   printConstraint("M", "O", coord) # A Monster implies odors all around
   printConstraintNeg("O", "M", coord) # No Odor means no monster all around
   printConstraintNeg("V", "T", coord) # No wind implies no hole all around
   printExclusionConstraint("M","T",coord) # A cell cannot contain a Monster and a Hole
   printExclusionConstraint("G","T",coord) # A cell cannot contain a Gold and a Hole
   printExclusionConstraint("G","M",coord) # A cell cannot contain a Gold and a Monster

## Now prints all the constaints

for x in range(__GRIDSIZE__):
  for y in range(__GRIDSIZE__):
    printAllConstraints((x,y))

WORLD = [
   ["O", "", "V", "T"],
   ["MO", "VOG", "T", "V"],
]
def reveal()

print(varToInt("M", (0,0)) + " 0")
