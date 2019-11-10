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
	int original_array1[MAX];
	int original_array2[MAX];

	int c;
	srand(time(NULL));
	for (c = 0; c < n; c++) {

		original_array1[c] = rand() % n;
		original_array2[c] = rand() % n;
	}

	mergesort(original_array1, 0, n - 1);
	mergesort(original_array2, 0, n - 1);

	FILE* out1 = fopen("sa1.txt", "w");
	FILE* out2 = fopen("sa2.txt", "w");

	int i = 0;
	for (i = 0; i < n; i++) {
		fprintf(out1, "%d ", original_array1[i]);
		fprintf(out2, "%d ", original_array2[i]);
	}
	
	fclose(out1);
	fclose(out2);

	printf("Generation Done!\n");
}

