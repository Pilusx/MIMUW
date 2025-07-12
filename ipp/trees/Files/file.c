#include "file.h"

void pipe_std_to_file(char *line, size_t bufsize, char *filename) {
	FILE *file = fopen(filename, "w");
	
	while (getline(&line, &bufsize, stdin) != EOF) {
		fprintf(file, line);
	}
	
	fclose(file);
}
