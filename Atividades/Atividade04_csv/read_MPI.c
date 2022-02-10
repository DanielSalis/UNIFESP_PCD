#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

// nome do arquivo
#define FILENAME "MOCK_DATA_100000.csv"

// total de linhas
#define ROWS 100000

// total de colunas
#define COLUMNS 3

// indice da coluna que será baseada para fazer os cálculos
#define K 2

void read_csv(double **data, int num_procs, int proc_id, int subsize)
{
	FILE *file;
	file = fopen(FILENAME, "r");

	float menor = ROWS, maior = 0, media = 0, soma = 0;
	float soma_total = 0;

	float *maxTot = (float *)malloc(num_procs * sizeof(float));
	float *minTot = (float *)malloc(num_procs * sizeof(float));

	int i = proc_id * subsize;
	int max = i + subsize;

	char line[4098];

	maxTot[proc_id] = 0;
	minTot[proc_id] = ROWS;

	while (fgets(line, 4098, file) && (i < max))
	{
		char *tmp = strdup(line);

		int j = 0;
		const char *tok;
		for (tok = strtok(line, ","); tok && *tok; j++, tok = strtok(NULL, ","))
		{
			data[i][j] = atof(tok);
		}

		if (data[i][K] > maxTot[proc_id])
			maxTot[proc_id] = data[i][K];

		if (data[i][K] < minTot[proc_id])
			minTot[proc_id] = data[i][K];

		soma += data[i][K];

		free(tmp);
		i++;
	}

	MPI_Barrier(MPI_COMM_WORLD);

	printf("Soma: %.2f \n", soma);

	MPI_Reduce(&soma, &soma_total, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&minTot[proc_id], &menor, 1, MPI_FLOAT, MPI_MIN, 0, MPI_COMM_WORLD);
	MPI_Reduce(&maxTot[proc_id], &maior, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);

	if (proc_id == 0)
	{
		media = soma_total / (ROWS - 1);

		printf("Soma total: %.2f \n", soma_total);
		printf("Media: %.2f\n", media);
		printf("Menor: %.2f\n", menor);
		printf("Maior: %.2f\n", maior);
	}
}

int main(int argc, char *argv[])
{
	int num_procs, proc_id, ferr;
	double start, finish;

	int ierr = MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

	int subsize = ROWS / num_procs;
	int rest = ROWS % num_procs;

	if (proc_id == num_procs - 1)
	{
		subsize += rest;
	}

	printf("size proc_id %d = %d\n", proc_id, subsize);

	MPI_Barrier(MPI_COMM_WORLD);

	if (proc_id == 0)
	{
		start = MPI_Wtime();
	}

	double **data;
	data = (double **)malloc(ROWS * sizeof(double *));
	for (int i = 0; i < ROWS; i++)
	{
		data[i] = (double *)malloc(COLUMNS * sizeof(double));
	}

	read_csv(data, num_procs, proc_id, subsize);

	if (proc_id == 0)
	{
		finish = MPI_Wtime();
		printf("\nTempo total de execução: %.4f seg\n", finish - start);
	}

	ferr = MPI_Finalize();

	return 0;
}
