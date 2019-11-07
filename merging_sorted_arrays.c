#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX 32768
#define MIN(a,b) (a < b ? a : b)

void file_to_array(int* init_array, int file)
{
	FILE* input;
	if (file == 0)
		input = fopen("sa1.txt", "r");
	else
		input = fopen("sa2.txt", "r");

	printf("Starting to read file...\n");

	int num;
	int count = 0;
	while (fscanf(input, "%d", &num) != EOF)
	{
		init_array[count] = num;
		count++;
	}
	fclose(input);

	printf("File reading finished...\n");
}

void array_to_file(int* array)
{
	FILE* out = fopen("result.txt", "w");

	int i = 0;
	for (i = 0; i < 2 * MAX; i++) {
		fprintf(out, "%d ", array[i]);
	}

	fclose(out);
}

int binary_search(int* array, int target, int size)
{
	int left = 0;
	int right = size - 1;
	int mid = floor((left + right) / 2);

	int found_index = -1;

	while (left <= right)
	{
		if (array[mid] < target)
		{
			left = mid + 1;
		}
		else if (array[mid] == target)
		{
			left = mid + 1;
			found_index = mid;
		}
		else
		{
			right = mid - 1;
		}

		mid = floor((left + right) / 2);
	}

	return mid;
}

void init_p4s(int total_size, int* A, int* B, int num_procs,
	int* a_part_size, int* a_indices, int* b_part_size, int* b_indices)
{
	printf("Starting to initialize...\n");

	int remainder = total_size % num_procs;
	int local_n = total_size / num_procs;

	for (int i = 0; i < num_procs; i++)
	{
		if (i < remainder)
			a_part_size[i] = local_n + 1;
		else
			a_part_size[i] = local_n;
	}

	for (int i = 0; i < num_procs; i++)
		a_indices[i] = i * local_n + MIN(i, remainder);

	int start = 0;
	int largest_a_index, largest_a;
	int largest_b_index;
	int size;

	for (int i = 0; i < num_procs; i++) {
		largest_a_index = a_part_size[i] + a_indices[i] - 1;
		largest_a = A[largest_a_index];

		largest_b_index = binary_search(B, largest_a, total_size);

		if (largest_b_index > 0) {

			size = largest_b_index + 1 - start;
			b_indices[i] = start;
			b_part_size[i] = size;
			start = largest_b_index + 1;
		}
		else {
			b_indices[i] = start;
			b_part_size[i] = 0;
		}

		largest_b_index += a_part_size[i + 1];
	}
	printf("Initializing finished...\n");
}

void merge(int* a, int* b, int* merged, int size_a, int size_b) {
	int merged_index = 0, i = 0, j = 0;

	while (i < size_a && j < size_b)
	{
		if (a[i] < b[j])
		{
			merged[merged_index] = a[i];
			i++;
		}
		else
		{
			merged[merged_index] = b[j];
			j++;
		}
		merged_index++;
	}

	if (i == size_a)
	{
		while (j < size_b)
		{
			merged[merged_index] = b[j];
			merged_index++;
			j++;
		}
	}

	if (j == size_b)
	{
		while (i < size_a)
		{
			merged[merged_index] = a[i];
			merged_index++;
			i++;
		}
	}

}

int main(int argc, char** argv) {
	int num_procs;
	int rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	int* A;
	int* B;

	// Define the arrays for using MPI_Scatterv().
	// We are not using MPI_Scatter(), so this may be more complex.
	int* a_indies = malloc(sizeof(int) * num_procs);
	int* a_part_length = malloc(sizeof(int) * num_procs);
	int* b_indies = malloc(sizeof(int) * num_procs);
	int* b_part_length = malloc(sizeof(int) * num_procs);

	if (rank == 0)
	{
		A = malloc(sizeof(int) * MAX);
		B = malloc(sizeof(int) * MAX);
		file_to_array(A, 0);
		file_to_array(B, 1);

		init_p4s(MAX, A, B, num_procs, a_part_length, a_indies, b_part_length, b_indies);
	}

	MPI_Bcast(a_indies, num_procs, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(a_part_length, num_procs, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(b_indies, num_procs, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(b_part_length, num_procs, MPI_INT, 0, MPI_COMM_WORLD);


	int a_size = a_part_length[rank];
	int b_size = b_part_length[rank];
	int merged_size = a_size + b_size;

	int* a = malloc(sizeof(int) * a_size);
	int* b = malloc(sizeof(int) * b_size);
	int* merged = malloc(sizeof(int) * merged_size);

	MPI_Scatterv(A, a_part_length, a_indies, MPI_INT, a, a_size, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatterv(B, b_part_length, b_indies, MPI_INT, b, b_size, MPI_INT, 0, MPI_COMM_WORLD);

	merge(a, b, merged, a_size, b_size);

	int* result;
	int* result_indies;
	int* result_part_size;
	if (rank == 0)
	{
		result = malloc(sizeof(int) * (MAX * 2));
		result_indies = malloc(sizeof(int) * num_procs);
		result_part_size = malloc(sizeof(int) * num_procs);

		for (int i = 0; i < num_procs; i++)
		{
			result_part_size[i] = a_part_length[i] + b_part_length[i];
			result_indies[i] = a_indies[i] + b_indies[i];
		}
	}

	MPI_Gatherv(merged, merged_size, MPI_INT, result, result_part_size, result_indies, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank == 0)
	{
		/*
			for (int i = 32760; i < 32768; ++i)
				printf("%d, ", A[i]);
			printf("\n");
			for (int i = 32760; i < 32768; ++i)
				printf("%d, ", B[i]);
			printf("\n");
			for (int i = 65522; i < 65536; ++i)
				printf("%d, ", result[i]);
			printf("\n");
		*/
		array_to_file(result);

		free(A); free(B); free(result);
		free(result_indies); free(result_part_size);
	}
	free(a_indies);
	free(a_part_length);
	free(b_indies);
	free(b_part_length);
	free(a);
	free(b);
	free(merged);

	printf("Processor %d done!\n", rank);

	MPI_Finalize();

	return 0;
}