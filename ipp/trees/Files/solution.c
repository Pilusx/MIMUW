#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "tree.h"

#define TEMP "TEMPSTD.temp"

typedef enum {
	ADD_NODE,
	RIGHTMOST_CHILD,
	DELETE_NODE,
	DELETE_SUBTREE,
	SPLIT_NODE,
	OTHER
} Action;

Action parse(char *line, int *k, int *w) {
	char first, middle;
	first = line[0];
	middle = line[7];
	switch (first) {
		case 'A' :
			sscanf(line, "ADD_NODE %d", k);
			return ADD_NODE;
		case 'R' :
			sscanf(line, "RIGHTMOST_CHILD %d", k);
			return RIGHTMOST_CHILD;
		case 'S' :
			sscanf(line, "SPLIT_NODE %d %d", k, w);
			return SPLIT_NODE;
		case 'D' :
			switch (middle) {
				case 'N' :
					sscanf(line, "DELETE_NODE %d", k);
					return DELETE_NODE;
				case 'S' :
					sscanf(line, "DELETE_SUBTREE %d", k);
					return DELETE_SUBTREE;
				default :
					return OTHER;
			}
		default :
			return OTHER;
	}
}

int maximal_number_of_nodes(char *line, size_t bufsize, 
		char *filename) {
			
	FILE *file = fopen(filename,"r");
	int counter = 1;
	
	while(getline(&line,&bufsize,file) != EOF){
		if(line[0] == 'A' || line[0] == 'S'){
			++counter;
		}
	}
	
	fclose(file);
	return counter;
}

void operate_on_tree(Tree t, Action act, bool show_nodes, 
		int k, int w) {
			
	if (act == ADD_NODE) {
		add_node(k, t);	
	}
	else if (act == RIGHTMOST_CHILD) {
		printf("%d\n", rightmost_node(k, t));			
	}
	else if (act == SPLIT_NODE) {
		split_node(k, w, t);			
	}
	else if (act == DELETE_NODE) {
		delete_node(k, t);		
	}
	else if (act == DELETE_SUBTREE) {
		delete_subtree(k, t);			
	}
		
	if (act != RIGHTMOST_CHILD) {
		printf("OK\n");
	}
	
	if (show_nodes) {
		fprintf(stderr, "NODES: %d\n", nodes(t));
	}
}


int main(int argc, char** argv){
	size_t bufsize = 64;
	char buffer[bufsize];
	char *line = buffer;
	int k, w;
	int treesize = 1;
	Action act;
	bool show_nodes = false;
	
	// Read parameters
	int i;
	for (i = 0; i < argc; i++) {
		size_t size = (strlen(argv[i]) + 1) * sizeof(char);
		char *paramString = (char*) malloc(size);
		strcpy(paramString, argv[i]);
		
		if (strcmp(paramString, "-v") == 0) {
			show_nodes = true;
		}
		else if (paramString[0] == '-') {
			printf("ERROR");
			return 1;			
		}
		free(paramString);
    }
    
	
	pipe_std_to_file(line, bufsize, TEMP);
	treesize = maximal_number_of_nodes(line, bufsize, TEMP);
	Tree t = new_tree(treesize);
	
	FILE *temp = fopen(TEMP, "r");
	while (getline(&line, &bufsize, temp) != EOF) {
		act = parse(line, &k, &w);
		operate_on_tree(t, act, show_nodes, k, w);
	}
	fclose(temp);
	remove(TEMP);
	
	free_tree(t);
	return 0;
}
