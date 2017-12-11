# Face recognition
A program that performs face recognition using Local Binary Patterns. lbp_seq.c is the sequential implementation and lbp_omp.c is the parallel implementation using OpenMP. 
Both of the sequential and parallel programs take input k, where k is the number of training sets. So for example if k = 8,  
then the first 8 images of each person in the /images folder will be used for training the program, while the remaining 12
images will be used for testing.
