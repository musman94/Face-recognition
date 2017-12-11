# Face recognition
A program that performs face recognition using Local Binary Patterns.     
lbp_seq.c is the sequential implementation and lbp_omp.c is the parallel implementation using OpenMP.     
Both of the sequential and parallel programs take input k, where k is the number of training sets. So for example if k = 8 then the first 8 images of each person in the /images folder will be used for training the program, while the remaining 12
images will be used for testing.        
./run.sh is used to run the program. It compiles and runs the sequential part with k as 1, 2, 5, 7, 10. And it runs the parallel part with the number of threads as 1, 2, 4, 6, 8, 16 while keeping k as constant at 10.     
The output of the program is outputted to Usman_Muhammad.output.    
The gprof results for both the sequential and parallel parts are outputted to prof_sequential.txt and prof_omp.txt.       
