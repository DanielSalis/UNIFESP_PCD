#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <mpi.h>
#include <math.h>

typedef struct
{
	int sum;
	int min;
	int max;
	int count;
	int created;
} FileResult;

typedef struct
{
	FileResult *results;
	int size;
} Result;

Result read_csv(int **data, int proc_id, int subsize, int num_procs, int rows)
{
	FileResult *results = (FileResult *)malloc(sizeof(FileResult));
	results[0].min = 10000;
	results[0].max = 0;
	results[0].sum = 0;
	results[0].count = 0;
	results[0].created = 1;

	int initial_size = 1;

	int preco_col = 0, univ_col = 1;
	int sum = 0, min = 10000, max = 0;

	int i = proc_id * subsize;
	int max_i;
	if (proc_id == num_procs - 1)
		max_i = rows - i;
	else
		max_i = floor((int)rows / num_procs);

	for (int l = i; l < i + max_i; l++)
	{
		int preco = data[l][preco_col];
		int univ_id = data[l][univ_col];

		if (univ_id > initial_size)
		{
			initial_size = univ_id;
			results = (FileResult *)realloc(results, univ_id * sizeof(FileResult));
		}

		if (results[univ_id - 1].created != 1)
		{
			results[univ_id - 1].min = preco;
			results[univ_id - 1].max = preco;
			results[univ_id - 1].sum = preco;
			results[univ_id - 1].count = 1;
			results[univ_id - 1].created = 1;
		}
		else
		{
			if (preco > results[univ_id - 1].max)
				results[univ_id - 1].max = preco;

			if (preco < results[univ_id - 1].min)
				results[univ_id - 1].min = preco;

			results[univ_id - 1].sum += preco;
			results[univ_id - 1].count++;
		}
	}

	Result result;
	result.results = results;
	result.size = initial_size;

	return result;
}

int **read_file(char *filename, int rows)
{
	int i;
	int **data = (int **)malloc(rows * sizeof(int *));
	for (i = 0; i < rows; i++)
	{
		data[i] = (int *)malloc(sizeof(int));
	}

	FILE *fp = fopen(filename, "r");

	i = 0;
	char line[4098];
	while (fgets(line, 4098, fp) && i < rows)
	{
		int j = 0;
		const char *tok;
		tok = strtok(line, ",");
		while (tok && *tok)
		{
			if (j > 1)
				data[i] = (int *)realloc(data[i], (j + 1) * sizeof(int));

			data[i][j] = atof(tok);

			j++;
			tok = strtok(NULL, ",");
		}

		i++;
	}

	fclose(fp);

	return data;
}

void process(int *file_rows, char **filenames, int n_files, int proc_id, int num_procs)
{
	int i, j;
	for (i = 0; i < n_files; i++)
	{
		int rows = file_rows[i];

		int **data = (int **)malloc(rows * sizeof(int *));

		if (proc_id == 0)
		{
			printf("\nArquivo %s: \n", filenames[i]);
			fflush(stdout);

			data = read_file(filenames[i], rows);
		}

		for (j = 0; j < rows; j++)
		{
			if (proc_id != 0)
			{
				data[j] = (int *)malloc(3 * sizeof(int));
			}

			MPI_Bcast((int **)&(data[j][0]), 3, MPI_INT, 0, MPI_COMM_WORLD);
		}

		int subsize = rows / num_procs;
		Result result = read_csv(data, proc_id, subsize, num_procs, rows);
		free(data);

		int *count = (int *)malloc(result.size * sizeof(int));
		int *total_sum = (int *)malloc(result.size * sizeof(int));
		int *min = (int *)malloc(result.size * sizeof(int));
		int *max = (int *)malloc(result.size * sizeof(int));

		for (j = 0; j < result.size; j++)
		{
			MPI_Reduce(&result.results[j].count, &count[j], 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
			MPI_Reduce(&result.results[j].sum, &total_sum[j], 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
			MPI_Reduce(&result.results[j].min, &min[j], 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
			MPI_Reduce(&result.results[j].max, &max[j], 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
		}

		MPI_Barrier(MPI_COMM_WORLD);

		if (proc_id == 0)
		{
			for (j = 0; j < result.size; j++)
			{
				float media = total_sum[j] / count[j];

				printf("Universidade: %d\n", j + 1);
				printf("Soma total: %d\n", total_sum[j]);
				printf("Quantidade: %d\n", count[j]);
				printf("Media: %.2f\n", media);
				printf("Menor: %d\n", min[j]);
				printf("Maior: %d\n\n", max[j]);
				fflush(stdout);
			}
		}

		MPI_Barrier(MPI_COMM_WORLD);
		fflush(stdout);
	}
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
	int *file_rows = (int *)malloc(sizeof(int));

	if (proc_id == 0)
	{
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
				if (n_files > 1)
				{
					filenames = (char **)realloc(filenames, n_files * sizeof(char *));
				}
				filenames[n_files - 1] = strdup(entry->d_name);

				printf("Arquivo %3d: %s \n", n_files, filenames[n_files - 1]);
				fflush(stdout);
			}
		}

		closedir(folder);

		file_rows = (int *)malloc(n_files * sizeof(int));
		for (i = 0; i < n_files; i++)
		{
			printf("Qtd de linhas do arquivo %s: ", filenames[i]);
			fflush(stdout);
			scanf("%d", &file_rows[i]);
		}
	}

	if (proc_id == 0)
	{
		start = MPI_Wtime();
	}

	MPI_Bcast(&n_files, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(file_rows, n_files, MPI_INT, 0, MPI_COMM_WORLD);

	process(file_rows, filenames, n_files, proc_id, num_procs);

	if (proc_id == 0)
	{
		finish = MPI_Wtime();
		printf("\nTempo total de execução: %.4f seg\n", finish - start);
	}

	free(filenames);
	free(file_rows);

	ferr = MPI_Finalize();

	return 1;
}
