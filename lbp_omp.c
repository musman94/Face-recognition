#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <omp.h>
#include "util.h"

#define num_ppl 18
#define num_files 20
#define rows 180
#define cols 200

void create_histogram(int * hist, int ** img, int num_rows, int num_cols)
{
	int b = 1;
	int val = 0;
	int centre;
	for(int i = 1; i < rows - 1; i++)
		#pragma omp parallel for private(centre) firstprivate(b,val)
		for(int j = 1; j < cols - 1; j++)
		{
			val = 0;
			b = 1;
			centre = img[i][j];
			if(centre < img[i][j - 1])
				val += b;
			b = 2;
			if(centre < img[i + 1][j - 1])
				val += b;
			b = 4;
			if(centre < img[i + 1][j])
				val += b;
			b = 8;
			if(centre < img[i + 1][j + 1])
				val += b;
			b = 16;
			if(centre < img[i][j + 1])
				val += b;
			b = 32;
			if(centre < img[i - 1][j + 1])
				val += b;
			b = 64;
			if(centre < img[i - 1][j])
				val += b;
			b = 128;
			if(centre < img[i - 1][j - 1])
				val += b; 
			#pragma omp atomic
			hist[val] += 1;
		}


}

double distance(int * a, int *b, int size)
{
	double dis = 0;
	#pragma omp parallel for reduction(+:dis)
	for(int i = 0; i < size; i++)
		if(a[i] + b[i] != 0)
			dis += (double)(a[i] - b[i])*(a[i] - b[i])/(double)(2 * (a[i] + b[i]));
	return dis;
}

int find_closest(int ***training_set, int num_persons, int num_training, int size, int * test_image)
{
	double max = LONG_MAX;
	double diff = 0;
	int id = -1000;
	for(int i = 0; i < num_persons; i++)
		for(int j = 0; j < num_training; j++)
		{
			diff = distance(training_set[i][j],test_image,size);
			if(diff <= max)
			{
				max = diff;
				id = i + 1;
			}
		}

	return id;	
}


int main(int argc,char* argv[])
{
	int i,j,a,b;
	clock_t start, end;
	double time;
	start = clock();
	if(argc < 2)
	{
		printf("Please enter a number\n");
		return 1;
	}
	int k = atoi(argv[1]);
	// Training part
	int ****ptrain_Set = (int ****)malloc(num_ppl * sizeof(int ***));
	for(i = 0; i < num_ppl; i++)
		ptrain_Set[i] = (int ***)malloc(k * sizeof(int **));
	char file[128];
	//#pragma omp parallel for default(shared) collapse(2)
	for(i = 0; i < num_ppl; i++)
		for(j = 0; j < k; j++)
		{
			sprintf(file, "images/%d.%d.txt", i + 1, j + 1);
			ptrain_Set[i][j] = read_pgm_file(file,rows,cols);
		}
	int ***training_set = (int ***)malloc(num_ppl * sizeof(int **));
	for(i = 0; i < num_ppl; i++)
	{
		training_set[i] = (int **)malloc(k * sizeof(int *));
		for(j = 0; j < k; j++)
		{
			training_set[i][j] = (int *)malloc(256 * sizeof(int));
			#pragma omp parallel for schedule(static)
			for(a = 0; a < 256; a++)
				training_set[i][j][a] = 0;
			create_histogram(training_set[i][j],ptrain_Set[i][j],rows,cols);
		}

	}
	//Testing part
	int test_files = num_files - k;
	int ****ptest_Set = (int ****)malloc(num_ppl * sizeof(int ***));
	for(i = 0; i < num_ppl; i++)
		ptest_Set[i] = (int ***)malloc(test_files * sizeof(int **));
	//	#pragma omp parallel for default(shared)
	for(i = 0; i < num_ppl; i++)
		for(j = 0; j < test_files; j++)
		{
			sprintf(file, "images/%d.%d.txt", i + 1, j + 1 + k);
			ptest_Set[i][j] = read_pgm_file(file,rows,cols);
		}
	int ***test_Set = (int ***)malloc(num_ppl * sizeof(int **));
	for(i = 0; i < num_ppl; i++)
	{
		test_Set[i] = (int **)malloc(test_files * sizeof(int *));
		for(j = 0; j < test_files; j++)
		{
			test_Set[i][j] = (int *)malloc(256 * sizeof(int));
			for(a = 0; a < 256; a++)
				test_Set[i][j][a] = 0;
			create_histogram(test_Set[i][j],ptest_Set[i][j],rows,cols);
		}
	}
	int t_count = 0;
	int c_count = 0;
	for(i = 0; i < num_ppl; i++)
		for(j = 0; j < test_files; j++)
		{
			int closest = find_closest(training_set, num_ppl, k, 256, test_Set[i][j]);
			if(closest == i + 1)
				c_count++;
			t_count++;
			printf("%d.%d.txt %d %d\n",i + 1, j + 1 + k, closest, i + 1);
		}
	printf("Accuracy: %d correct answers for %d tests\n",c_count,t_count);
	
	//Deallocation

	//#pragma omp parallel for schedule(static)
	for(i = 0; i < num_ppl; i++)
	{
		for(j = 0; j < k; j++)
		{
			for(a = 0; a < rows; a++)
				free(ptrain_Set[i][j][a]);
			free(ptrain_Set[i][j]);
		}
		free(ptrain_Set[i]);
	}
	free(ptrain_Set);

	for(i = 0; i < num_ppl; i++)
	{
		for(j = 0; j < test_files; j++)
		{
			for(a = 0; a < rows; a++)
				free(ptest_Set[i][j][a]);
			free(ptest_Set[i][j]);
		}
		free(ptest_Set[i]);
	}
	free(ptest_Set);

	for(i = 0; i < num_ppl; i++)
	{
		for(j = 0; j < k; j++)
			free(training_set[i][j]);
		free(training_set[i]);
	}
	free(training_set);

	for(i = 0; i < num_ppl; i++)
	{
		for(j = 0; j < test_files; j++)
			free(test_Set[i][j]);
		free(test_Set[i]);
	}
	free(test_Set);
	end = clock();
	time = (double)(end - start);
	printf("Parallel time: %0.2f ms\n",time / CLOCKS_PER_SEC);
	return 0;

}



