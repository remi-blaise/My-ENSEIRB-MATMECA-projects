for i in seq_basic seq_no_jump omp_for_basic omp_for_no_jump seq_tuilee seq_tuilee_opti ;#omp_for_tuilee_static omp_for_tuilee_opti_static omp_task_tuilee_opti omp_for_tuilee omp_for_tuilee_opti omp_task_tuilee ;
do
    for j in 16 ;
    do
	bash make_data.sh $i $j
    done
done
