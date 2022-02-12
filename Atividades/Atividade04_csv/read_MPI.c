#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <mpi.h>
typedef struct
{
	float sum;
	float min;
	float max;
} FileResult;

FileResult read_csv(char *filename, int proc_id, int subsize)
{
	int K = 0;
	float sum=0, min, max;

	double **data = (double **)malloc(subsize * sizeof(double *));
	for (int l = 0; l < subsize; l++)
	{
		data[l] = (double *)malloc(sizeof(double));
	}

	FILE *file = fopen(filename, "r");

	int i = proc_id * subsize;
	int max_i = i + subsize;

	char line[4098];

	while (fgets(line, 4098, file) && (i < max_i))
	{
		int pos = i % subsize;
		char *tmp = strdup(line);

		int j = 0;
		const char *tok;
		tok = strtok(line, ",");
		while (tok && *tok)
		{
			data[pos] = (double *)realloc(data[pos], (j + 1) * sizeof(double));
			data[pos][j] = atof(tok);
			j++;
			tok = strtok(NULL, ",");
		}

		if (data[pos][K] > max)
			max = data[pos][K];

		if (data[pos][K] < min)
			min = data[pos][K];

		sum += data[pos][K];

		free(tmp);
		i++;
	}

	FileResult result;
	result.min = min;
	result.max = max;
	result.sum = sum;

	return result;
}

int main(int argc, char *argv[])
{
	int num_procs, proc_id, ferr, i;
	double start, finish;

	int ierr = MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

	int n_files = 0;
	char **filenames = (char **)malloc(sizeof(char *));
	int *filename_sizes = (int *)malloc(sizeof(int));
	float *file_rows = (float *)malloc(sizeof(float));

	if (proc_id == 0)
	{
		start = MPI_Wtime();

		DIR *folder;
		struct dirent *entry;

		folder = opendir(".");
		if (folder == NULL)
		{
			perror("Unable to read directory");
			return 0;
		}

		while ((entry = readdir(folder)))
		{
			char *ext = strrchr(entry->d_name, '.');
			if (ext && strcmp(ext + 1, "csv") == 0)
			{
				n_files++;
				filenames = (char **)realloc(filenames, n_files * sizeof(char *));
				filenames[n_files - 1] = entry->d_name;

				filename_sizes = (int *)realloc(filename_sizes, n_files * sizeof(int));
				filename_sizes[n_files - 1] = strlen(filenames[n_files - 1]);

				printf("Arquivo %3d: %s \n", n_files, filenames[n_files - 1]);
				fflush(stdout);
			}
		}

		closedir(folder);

		file_rows = (float *)malloc(n_files * sizeof(float));
		for (i = 0; i < n_files; i++)
		{
			printf("Qtd de linhas do arquivo %s: ", filenames[i]);
			fflush(stdout);
			scanf("%f", &file_rows[i]);
		}
	}

	MPI_Bcast(&n_files, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(file_rows, n_files, MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Bcast(filename_sizes, n_files, MPI_INT, 0, MPI_COMM_WORLD);
	for (i = 0; i < n_files; i++)
	{
		MPI_Bcast(filenames[i], filename_sizes[i], MPI_CHAR, 0, MPI_COMM_WORLD);
	}

	float min, max, total_sum = 0;

	for (i = 0; i < n_files; i++)
	{
		int rows = file_rows[i];
		int subsize = rows / num_procs;
		int rest = rows % num_procs;

		if (proc_id == num_procs - 1)
		{
			subsize += rest;
		}

		if (proc_id == 0)
		{
			printf("\nArquivo %s: \n", filenames[i]);
			fflush(stdout);
		}
		FileResult result = read_csv(filenames[i], proc_id, subsize);

		printf("result.sum %f\n", result.sum);
		// printf("result.min %f\n", result.min);
		// printf("result.max %f\n", result.max);
		fflush(stdout);

		MPI_Barrier(MPI_COMM_WORLD);

		MPI_Reduce(&result.sum, &total_sum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
		MPI_Reduce(&result.min, &min, 1, MPI_FLOAT, MPI_MIN, 0, MPI_COMM_WORLD);
		MPI_Reduce(&result.max, &max, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);


		if (proc_id == 0)
		{
			float media = total_sum / rows;

			printf("Soma total: %.2f \n", total_sum);
			printf("Media: %.2f\n", media);
			printf("Menor: %.2f\n", min);
			printf("Maior: %.2f\n\n", max);
			fflush(stdout);
		}

		MPI_Barrier(MPI_COMM_WORLD);
		fflush(stdout);
	}

	if (proc_id == 0)
	{
		finish = MPI_Wtime();
		printf("\nTempo total de execução: %.4f seg\n", finish - start);
	}

	ferr = MPI_Finalize();

	return 1;
}
