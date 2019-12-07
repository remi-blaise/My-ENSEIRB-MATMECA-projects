#set terminal postscript eps enhanced color solid "Helvetica" 18
set terminal pdf enhanced color size 5,4 font "Helvetica,14"

##set output "gebrd_scalability-attila.pdf"

#set grid x
#set grid y

#set title "Scalability on Mordor (4 GPUs + 16 CPUs)"

#
# Black
# FlatTS / Elemental
# FlatTT / Scalapack
# Greedy / MKL
# Adaptatif
# Plasma
set style line 9 lc rgb 'black'       linetype 1 linewidth 2 dashtype 1
set style line 1 lc rgb 'red'         linetype 1 linewidth 2 dashtype 1 pt 1
set style line 2 lc rgb 'blue'        linetype 1 linewidth 2 dashtype 1 pt 2
set style line 3 lc rgb 'green'       linetype 1 linewidth 2 dashtype 1	pt 3
set style line 4 lc rgb 'dark-violet' linetype 1 linewidth 2 dashtype 1 pt 4
set style line 5 lc rgb '#dd181f'     linetype 1 linewidth 2 dashtype 1
set style line 6 lc rgb '#ffa040'     linetype 1 linewidth 2 dashtype 1
set style line 8 lc rgb '#b3b3b3'     linetype 1 linewidth 2 dashtype 1

flops_gebrd(m, n) = (((n * (n * (2. * m - 2./3. * n + 2. )+ 20./3.)) + (n * (n * (2. * m - 2./3. * n + 1. ) -  m +  5./3.))) / 1.e9)

fsq(x,    t1, t2, t3) = (flops_gebrd( x, x ) / (t1 + t2 + t3))
fts(x, y, t1, t2, t3) = (flops_gebrd( x, y ) / (t1 + t2 + t3))

#
# Square case
#

#
#  GEBRD
#
set key left top Left reverse
set xlabel "M = N (NB=160)"
set ylabel "GE2BND (GFlop/s)"
set yrange[0:700]
set output "square_gebrd.pdf"
plot \
     '< grep BD-Adapt  square.dat' u 2:9 w lp ls 4 t 'BiDiagAuto',  \
     '< grep BD-FlatTS square.dat' u 2:9 w lp ls 1 t 'BiDiagFlatTS', \
     '< grep BD-FlatTT square.dat' u 2:9 w lp ls 2 t 'BiDiagFlatTT', \
     '< grep BD-Greedy square.dat' u 2:9 w lp ls 3 t 'BiDiagGreedy'

#
#  GE2VAL 
#

set xlabel "M (N = M)"
set ylabel "GE2VAL (GFlop/s)"
#unset yrange
set output "square_gesvd.pdf"
plot \
     '< grep Best-at   square.dat' u 2:(fsq($2, $8, $10, $11)) w lp ls 4 t 'DPLASMA',  \
     '< grep MKL       square.dat' u 2:9 w lp ls 3 t 'MKL', \
     '< grep PLASMA    square.dat' u 2:9 w lp ls 6 t 'PLASMA', \
     '< grep Elemental square.dat' u 2:9 w lp ls 1 t 'Elemental', \
     '< grep Scalapack square.dat' u 2:9 w lp ls 2 t 'Scalapack'

##############################################################################
#
#  GEBRD
#

#
#   Case N = 2000
#
set ylabel "GE2BND (GFlop/s)"
set key top left Left reverse

set xlabel "M (N=2000, NB=160, q=13)"
set yrange[0:800]
set output "rect2000_gebrd.pdf"
plot \
     '< grep RB-Adapt  rect2000.dat' u 2:9 w lp ls 4 dashtype 4 notitle, \
     '< grep RB-FlatTS rect2000.dat' u 2:9 w lp ls 1 dashtype 4 notitle, \
     '< grep RB-FlatTT rect2000.dat' u 2:9 w lp ls 2 dashtype 4 notitle, \
     '< grep RB-Greedy rect2000.dat' u 2:9 w lp ls 3 dashtype 4 notitle, \
     '< grep BD-Adapt  rect2000.dat' u 2:9 w lp ls 4 t 'Auto',   \
     '< grep BD-FlatTS rect2000.dat' u 2:9 w lp ls 1 t 'FlatTS', \
     '< grep BD-FlatTT rect2000.dat' u 2:9 w lp ls 2 t 'FlatTT', \
     '< grep BD-Greedy rect2000.dat' u 2:9 w lp ls 3 t 'Greedy', \
     -10 w lines ls 8 dashtype 4 t 'R-BiDiag', \
     -10 w lines ls 8            t 'BiDiag'


set xlabel "M (N=10000, NB=160, q=63)"
set output "rect10000_gebrd.pdf"
set yrange[0:1000]
plot \
     '< grep RB-Adapt  rect10000.dat' u 2:9 w lp ls 4 dashtype 4 notitle, \
     '< grep RB-FlatTS rect10000.dat' u 2:9 w lp ls 1 dashtype 4 notitle, \
     '< grep RB-FlatTT rect10000.dat' u 2:9 w lp ls 2 dashtype 4 notitle, \
     '< grep RB-Greedy rect10000.dat' u 2:9 w lp ls 3 dashtype 4 notitle, \
     '< grep BD-Adapt  rect10000.dat' u 2:9 w lp ls 4 t 'Auto',   \
     '< grep BD-FlatTS rect10000.dat' u 2:9 w lp ls 1 t 'FlatTS', \
     '< grep BD-FlatTT rect10000.dat' u 2:9 w lp ls 2 t 'FlatTT', \
     '< grep BD-Greedy rect10000.dat' u 2:9 w lp ls 3 t 'Greedy', \
     -10 w lines ls 8 dashtype 4 t 'R-BiDiag', \
     -10 w lines ls 8            t 'BiDiag'

#
#  GE2VAL 
#
set ylabel "GE2VAL (GFlop/s)"
set key left top Left reverse
set xlabel "M (N=2000)"
set yrange[0:800]
 
set output "rect2000_gesvd.pdf"
plot \
     '< grep Best-at   rect2000.dat' u 2:(fts($2, $3, $8, $10, $11)) w lp ls 4 t 'DPLASMA',  \
     '< grep MKL       rect2000.dat' u 2:9 w lp ls 3 t 'MKL', \
     '< grep PLASMA    rect2000.dat' u 2:9 w lp ls 6 t 'PLASMA', \
     '< grep Elemental rect2000.dat' u 2:9 w lp ls 1 t 'Elemental', \
     '< grep Scalapack rect2000.dat' u 2:9 w lp ls 2 t 'Scalapack'

set xlabel "M (N=10000)"
set yrange[0:1000]
set output "rect10000_gesvd.pdf"
plot \
     '< grep Best-at   rect10000.dat' u 2:(fts($2, $3, $8, $10, $11)) w lp ls 4 t 'DPLASMA',  \
     '< grep MKL       rect10000.dat' u 2:9 w lp ls 3 t 'MKL', \
     '< grep PLASMA    rect10000.dat' u 2:9 w lp ls 6 t 'PLASMA', \
     '< grep Elemental rect10000.dat' u 2:9 w lp ls 1 t 'Elemental', \
     '< grep Scalapack rect10000.dat' u 2:9 w lp ls 2 t 'Scalapack'

