#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <mpi.h>
#include <math.h>

typedef struct
{
	float sum;
	float min;
	float max;
} FileResult;

FileResult read_csv(double *data, int proc_id, int subsize, int num_procs, int rows)
{
	int K = 0;
	float sum = 0, min = 10000, max = 0;

	int i = proc_id * subsize;
	int max_i;
	if (proc_id == num_procs - 1)
		max_i = rows - i;
	else
		max_i = floor((float)rows / num_procs);

	for (int l = i; l < i + max_i; l++)
	{
		if (data[l] > max)
			max = data[l];

		if (data[l] < min)
			min = data[l];

		sum += data[l];
	}

	FileResult result;
	result.min = min;
	result.max = max;
	result.sum = sum;

	return result;
}

double *read_file(char *filename, int rows)
{
	double *data = (double *)malloc(rows * sizeof(double));
	FILE *fp = fopen(filename, "r");

	char line[4098];
	int i = 0;
	while (fgets(line, 4098, fp))
	{
		int j = 0;
		const char *tok;
		tok = strtok(line, ",");
		data[i] = atof(tok);

		i++;
	}

	fclose(fp);

	return data;
}

int main(int argc, char *argv[])
{
	int num_procs, proc_id, ferr, i, j;
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

		double *data = (double *)malloc(rows * sizeof(double));

		if (proc_id == 0)
		{
			printf("\nArquivo %s: \n", filenames[i]);
			fflush(stdout);

			data = read_file(filenames[i], rows);
		}

		MPI_Bcast(data, rows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		int subsize = rows / num_procs;
		FileResult result = read_csv(data, proc_id, subsize, num_procs, rows);
		free(data);

		MPI_Reduce(&result.sum, &total_sum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
		MPI_Reduce(&result.min, &min, 1, MPI_FLOAT, MPI_MIN, 0, MPI_COMM_WORLD);
		MPI_Reduce(&result.max, &max, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);

		MPI_Barrier(MPI_COMM_WORLD);

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

	free(filenames);
	free(filename_sizes);
	free(file_rows);

	ferr = MPI_Finalize();

	return 1;
}
