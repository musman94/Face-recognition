gcc -pg -o lbp_seq lbp_seq.c util.c util.h
gcc -pg -o lbp_omp lbp_omp.c util.c util.h -fopenmp