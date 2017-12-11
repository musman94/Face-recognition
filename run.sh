k=(1 2 5 7 10)
num_of_threads=(1 2 4 6 8 16)
rm Usman_Muhammad.output
rm prof_sequential.txt
rm prof_omp.txt
touch prof_sequential.txt
touch prof_omp.txt
touch Usman_Muhammad.output
./compile.sh 
echo "SEQUENTIAL PART" >> Usman_Muhammad.output
for i in ${k[@]}; 
do
	#gcc -Wall -pg  lbp_seq.c util.h util.c -o lbp_seq
	echo "Results for k = ${i}" >> Usman_Muhammad.output
	./lbp_seq ${i} >> Usman_Muhammad.output
	echo "Results for k = ${i}" >> prof_sequential.txt
	gprof -b lbp_seq gmon.out >> prof_sequential.txt
done
echo "PARALLEL PART" >> Usman_Muhammad.output
for i in ${num_of_threads[@]};
do
	echo "Value of k = 10" >> Usman_Muhammad.output
	echo "Results for n = ${i}" >> Usman_Muhammad.output
	OMP_NUM_THREADS=${i} ./lbp_omp 10 >> Usman_Muhammad.output
	echo "Results for n = ${i}" >> prof_omp.txt
	gprof -b lbp_omp gmon.out >> prof_omp.txt

done