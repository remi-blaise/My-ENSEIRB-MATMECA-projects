export OMP_NUM_THREADS
export GOMP_CPU_AFFINITY

ITE=$(seq 5) # nombre de mesures
THREADS="1 6 12" # nombre de threads à utiliser pour les expés
GOMP_CPU_AFFINITY=$(seq 0 11) # on fixe les threads

PARAM="../2Dcomp -n -k vie -i 50 -g 16 -s " # parametres commun à toutes les executions

execute (){
EXE="$PARAM $*"
OUTPUT="$(echo $* | tr -d ' ')".data
for nb in $ITE; do for OMP_NUM_THREADS in $THREADS ; do echo -n "$OMP_NUM_THREADS " >> $OUTPUT  ; $EXE 2>> $OUTPUT ; done; done
}

# on suppose avoir codé 2 fonctions :
#   mandel_compute_omp_dynamic()
#   mandel_compute_omp_static()

for i in 2024 ;  # 2 tailles : -s 256 puis -s 512
do
    execute $i -v seq_basic
    execute $i -v seq_no_jump
    execute $i -v seq_tuilee
    execute $i -v seq_tuilee_opti
done
