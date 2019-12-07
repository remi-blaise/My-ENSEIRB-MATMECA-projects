''' Generates a random CNF file '''

k = 3
n = 170
m = int(n * 4.26)

import random 

random.seed() # Uses system time
for nc in range(0,m):
    c = []
    while len(c) < k:
        l = (random.randint(1,n)) * (1 if random.randint(0,1) else -1)
        if not l in c and not -l in c:
            c.append(l)
    for l in c:
        print(str(l) + " ")#, end="")
    print("0")
    
