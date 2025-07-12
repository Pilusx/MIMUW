#include "tree.h"
#include "list.h"
#include <assert.h>

int main() {
	Tree t = new_tree(101);
	int i = 0;
	for(; i < 100; i++)
		add_node(0,t);
	
	print_children(0,t);
	assert(nodes(t) == 101);
	for(i=1; i < 100; i = i+2)
		delete_node(i,t);
		
	split_node(0,40,t);
	print_children(0,t);
	print_children(101,t);
	
	delete_subtree(101,t);
	print_children(0,t);
		
	free_tree(t);
	return 0;
}
