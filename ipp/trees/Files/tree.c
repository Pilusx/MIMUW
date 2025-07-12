#include "tree.h"
#include "list.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

struct _Node {
	int number;
	ListNode where_in_parent;
	List children;	
};
typedef struct _Node* Node;

struct _Tree {
	int max_num_of_nodes;
	int next_possible_num;
	int current_num_of_nodes;
	Node *nodes;
};

void update_nodes(int k, Node n, Tree t){
	assert(t->max_num_of_nodes > k && k >= 0);
	
	if (n == NULL) {
		t->nodes[k] = NULL;
		--(t->current_num_of_nodes);
	} 
	else {
		t->nodes[k] = n;
		++(t->current_num_of_nodes);		
	}	
}

Node new_node(Tree t){
	assert(t->next_possible_num < t->max_num_of_nodes);
	
	Node n = (Node) malloc(sizeof(struct _Node));
	n->number = t->next_possible_num;
	n->where_in_parent = NULL;
	n->children = NULL;
	
	update_nodes(n->number, n, t);
	++(t->next_possible_num);
	
	return n;	
}

Node get_node(int k, Tree t){
	assert(t->next_possible_num > k && k >= 0);
	
	return t->nodes[k];
}


Tree new_tree(int max_num_of_nodes){
	Tree t = (Tree) malloc(sizeof(struct _Tree));
	t->nodes = (Node*) malloc(sizeof(Node) * (max_num_of_nodes +1));
	t->max_num_of_nodes = max_num_of_nodes + 1;
	t->next_possible_num = 0;
	t->current_num_of_nodes = 0;
	
	Node n = new_node(t);
	n->children = new_list();	
	
	return t;
}
void free_tree(Tree t){
	assert(t != NULL);
	
	int n = t->next_possible_num;
	int i = 0;
	for(; i < n; ++i){
		Node no = get_node(i, t);
		if(no != NULL){
			free_list(no->children);
			free(no);			
		}
	}
	
	free(t->nodes);
	free(t);
}

void set_parent(int child, int parent, Tree t){
	Node c = get_node(child,t);
	assert(c->where_in_parent == NULL);
	
	Node p = get_node(parent,t);
	ListNode ln = push_back(child,p->children);
	c->where_in_parent = ln;
}


void add_node(int k, Tree t){
	Node n = new_node(t);
	set_parent(n->number, k, t);
	n->children = new_list();
}

int rightmost_node(int k, Tree t){
	Node n = get_node(k, t);
	assert(n != NULL);
	
	if (is_empty(n->children)) {
		return -1;
	}
	else {
		return peek_back(n->children);	
	}
}

void delete_node(int k, Tree t){
	Node n = get_node(k, t);
	assert(n != NULL);
	
	replace_node_by_list(&n->where_in_parent, &n->children);
	update_nodes(k, NULL, t);
	free(n);	
}

void delete_subtree(int k, Tree t){
	assert(k != 0);
	
	Node n = get_node(k, t);
	update_nodes(k, NULL, t);
	pop_listnode(n->where_in_parent);
	List stack = n->children;
	free(n);
	
	while(!is_empty(stack)){
		n = get_node(peek_back(stack),t);
		update_nodes(n->number, NULL, t);
		replace_node_by_list(&n->where_in_parent, &n->children);
		free(n);		
	}
	free_list(stack);	
}

void split_node(int k, int w, Tree t){
	Node n = new_node(t);
	Node nk = get_node(k, t);
	Node nw = get_node(w, t);
	
	List l = split_list(nw->where_in_parent, nk->children);
	set_parent(n->number, nk->number, t);
	n->children = l;	
}

int nodes(Tree t){
	return t->current_num_of_nodes;	
}

void print_children(int k, Tree t){
	printf("Treestats-> curr: %d next: (%d/%d) max: %d \n",
		t->current_num_of_nodes,
		t->next_possible_num,
		t->max_num_of_nodes-1,
		t->max_num_of_nodes);
	
	Node n = get_node(k, t);
	printf("TreeNode num: %d\n", k);
	print_list_stdout(n->children);	
}
