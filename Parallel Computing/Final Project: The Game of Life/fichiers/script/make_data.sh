echo "# data of $1 with GRAIN=$2 and world size 512" > random_512_$1_$2.data
for i in 10 20 30 40 50 ;
do
    sum=0
    for j in 1 2 3 4 5 ;
    do
	../2Dcomp -k vie -v $1 -s 512 -g $2 -n -i $i -a random 2> _res
	res=$(cat _res)
	sum=$(echo "scale=10; ($sum+$res)" | bc -l)
    done
    echo "$i $(echo "scale=10; ($sum/5)" | bc)" >> 512_$1_$2.data
done
echo "# data of $1 with GRAIN=$2 and world size 4096" > random_4096_$1_$2.data
for i in 10 20 30 40 50 ;
do
    sum=0
    for j in 1 2 3 4 5 ;
    do
	../2Dcomp -k vie -v $1 -s 512 -g $2 -n -i $i -a random 2> _res
	res=$(cat _res)
	sum=$(echo "scale=10; ($sum+$res)" | bc -l)
    done
    echo "$i $(echo "scale=10; ($sum/5)" | bc)" >> random_4096_$1_$2.data
done
