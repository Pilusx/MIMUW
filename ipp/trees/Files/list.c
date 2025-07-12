#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "list.h"

#define GUARD -42

struct _ListNode {
	int number;
	ListNode before;
	ListNode next;	
};
struct _List {
	ListNode guardian;
};

void tie_pointers(ListNode first, ListNode second) {
	first->next = second;
	second->before = first;	
}

ListNode new_listnode(int k) {
	ListNode ln = (ListNode) malloc(sizeof(struct _ListNode));
	ln->number = k;
	ln->before = NULL;
	ln->next = NULL;	
	
	return ln;
}

// Returns the first node or the guardian if empty
ListNode first_node(List l) {
	assert(l->guardian != NULL);
	
	return l->guardian->next;	
}

// Returns the last node or guardian if empty
ListNode last_node(List l) {
	assert(l->guardian != NULL);
	
	return l->guardian->before;
}

List new_list() {
	List l = (List) malloc(sizeof(struct _List));
	l->guardian = new_listnode(GUARD);
	tie_pointers(l->guardian, l->guardian);
	
	return l;	
}

void free_list(List l) {
	if (l == NULL) {
		return;
	}
	while (!is_empty(l)) {
		pop_back(l);
	}
		
	free(l->guardian);
	free(l);
	l = NULL;
}

bool is_guardian(ListNode n) {
	assert(n != NULL);
	
	return n->number == GUARD;
}

bool is_empty(List l) {
	assert(l != NULL);
	
	return is_guardian(first_node(l));
}

int length(List l) {
	assert(l != NULL);
	
	int n = 0;
	ListNode no = first_node(l);
	while (!is_guardian(no)) {
		++n;
		no = no->next;
	}
	
	return n;
}

ListNode push_back(int k, List l) {
	ListNode ln = new_listnode(k);
	tie_pointers(l->guardian->before, ln);
	tie_pointers(ln, l->guardian);
	
	return ln;
}

int peek_back(List l) {
	assert(!is_empty(l));
	
	return last_node(l)->number;
}

void pop_listnode(ListNode n) {
	assert(!is_guardian(n));
	tie_pointers(n->before, n->next);
	free(n);
}

void pop_back(List l) {
	assert(l != NULL);
	ListNode n = last_node(l);
	if (!is_guardian(n)){
		pop_listnode(n);
	}
}

void replace_node_by_list(ListNode *node, List *list) {
	ListNode n = *node;
	List l = *list;
	
	assert(n !=NULL && list != NULL);
	
	if(is_empty(l)) {
		pop_listnode(n);
	}
	else {
		tie_pointers(n->before, first_node(l));
		tie_pointers(last_node(l), n->next);		
		tie_pointers(l->guardian, l->guardian);
		free(n);
	}
	
	free_list(l);
	node = NULL;
	list = NULL;
}

List split_list(ListNode w, List l) {
	assert(w != NULL && l != NULL);
	
	List nl = new_list();
	ListNode first = w->next;
	ListNode last = last_node(l);
	if(!is_guardian(first)) {
		tie_pointers(w, l->guardian);
		tie_pointers(nl->guardian, first);
		tie_pointers(last, nl->guardian);		
	}
		
	return nl;
}


void print_list_stdout(List l) {	
	if (l == NULL) {
		printf("[NULL]\n");		
	}
	else {
		printf("[%d]: ", length(l));
		ListNode ln = first_node(l);
		while (!is_guardian(ln)) {
			printf("%d ", ln->number);
			ln = ln->next;		
		}
		printf("\n");
	}
}
