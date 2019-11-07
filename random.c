#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 32768

void merge(int array[], int left, int m, int right) {
	int aux[MAX] = { 0 };
	int i;
	int j;
	int k;

	for (i = left, j = m + 1, k = 0; k <= right - left; k++)
	{
		if (i == m + 1)
		{
			aux[k] = array[j++];
			continue;
		}

		if (j == right + 1)
		{
			aux[k] = array[i++];
			continue;
		}
		if (array[i] < array[j])
		{
			aux[k] = array[i++];
		}

		else
		{
			aux[k] = array[j++];
		}
	}

	for (i = left, j = 0; i <= right; i++, j++)
	{
		array[i] = aux[j];
	}
}

void mergesort(int a[], int low, int high) {
	int mid;
	if (low < high) {
		mid = (high + low) / 2;
		mergesort(a, low, mid);
		mergesort(a, mid + 1, high);
		merge(a, low, mid, high);
	}
}

int main(int argc, char** argv) {
	int n = MAX;
	int original_array[MAX];

	int c;
	srand(time(NULL));
	// printf("This is the unsorted array: \n");
	for (c = 0; c < n; c++) {

		original_array[c] = rand() % n;
		// printf("%d ", original_array[c]);
	}

	mergesort(original_array, 0, n - 1);

	// printf("\n\n\n\n");

	FILE* out = fopen("sa1.txt", "w");

	int i = 0;
	for (i = 0; i < n; i++) {
		// printf("%d ", original_array[i]);
		fprintf(out, "%d ", original_array[i]);
	}
	
	fclose(out);

	printf("Done!\n");
}

