# First we need to print clauses
def printClause(clause):
    print(" ".join([str(v) for v in clause]) + " 0")

def equals1a(arrayOfVars):
    printClause(arrayOfVars)
    for i,x in enumerate(arrayOfVars):
        for y in arrayOfVars[i+1:]:
            printClause([-x, -y])

def varToInt(x, y, z):
    return 100*x + 10*y + z

def printCaseConstraint(i, j):
    return equals1a([varToInt(i, j, z) for z in range(1,10)])

def printLineConstraint(i, z):
    return equals1a([varToInt(i, j, z) for j in range(1,10)])

def printColumnConstraint(i, z):
    return equals1a([varToInt(j, i, z) for j in range(1,10)])

def printSquareConstraint(i, z):
    i -= 1
    return equals1a([varToInt(int(i / 3)*3 + int(j / 3) + 1, (i % 3)*3 + (j % 3) + 1, z) for j in range(9)])


tmp = [x for x in range(1,5)]
print("c Constraints forcing just on true variable in", tmp)
print("c we don't print any 'p cnf X Y' line, sorry")

for z in range(1, 10):
    for i in range(1, 10):
        printCaseConstraint(i, z)
        printLineConstraint(i, z)
        printColumnConstraint(i, z)
        printSquareConstraint(i, z)
