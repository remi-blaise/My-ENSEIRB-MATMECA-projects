#!/usr/bin/env python3.5

import matplotlib.pyplot as plt

file = open("build/graphs", "r")

scripts = file.read().strip().split("\n\n")

for script in scripts:
    data = script.split("\n")
    name = data[0]
    data = data[1:]

    x = []
    y1 = []
    y2 = []

    for line in data:
        [a, b, c] = line.split(',')
        x.append(a)
        y1.append(int(b) / 1000000)
        y2.append(int(c) / 1000000)

    plt.plot(x, y1, label="Notre librairie")
    plt.plot(x, y2, label="pthread")
    plt.legend()
    plt.xlabel("Valeur passée en paramètre (nb threads/yields)")
    plt.ylabel("Temps d'exécution (ms)")
    plt.title("Évolution du temps d'exécution de %s." % name)
    plt.savefig('build/%s.png' % name)
    plt.clf()
