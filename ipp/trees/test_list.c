#include "list.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

void test(){
	printf("Declare\n");
	List l = NULL;
	List l2 = NULL;
	ListNode ln = NULL;
	print_list(l); // null
	assert(l == NULL);
	l = new_list();
	l2 = new_list();
	assert(l != NULL);
	print_list(l); // Empty
	
	printf("Push\n");	
	push_back(1,l);
	push_back(3,l);
	print_list(l);
	assert(length(l) == 2);
	assert(!is_empty(l));
	assert(peek_back(l) == 3);
	
	push_back(7,l);
	assert(length(l) == 3);
	
	print_list(l); // 1 3 7
	
	
	printf("Push\n");
	push_back(5,l);
	ln = peek_back_node(l);
	push_back(42,l);
	push_back(17,l2);
	push_back(5,l2);	
	
	print_list(l); // 1 3 7 5 42
	print_list(l2); // 17 5
	
	
	printf("Implace\n");
	implace_list(ln, l2);
	ln = NULL;
	l2 = NULL;
	assert(l2 == NULL);
	print_list(l); // 1 3 7 17 5 42
	assert(length(l) == 6);
	print_list(l2); // Null	

	printf("Push and pop\n");
	push_back(3,l);
	l2 = new_list();
	push_back(5,l2);
	ln = peek_back_node(l2);
	pop_back(l);
	assert(peek_back(l) == 42);
	
	print_list(l); // 1 3 7 17 5 42
	print_list(l2); // 5
	
		
	printf("Implace\n");
	implace_list(ln,l);
	ln = NULL;
	l = NULL;
	print_list(l); // Null
	print_list(l2); // 1 3 7 17 5 42
	
	printf("Split\n");
	l = new_list();
	push_back(8,l);
	push_back(10,l);
	push_back(12,l);
	ln = peek_back_node(l);
	push_back(14,l);
	push_back(6,l);
	
	List l3 = split_list(ln,l);
	print_list(l); // 8 10 12
	print_list(l3);	// 14 6
	
	printf("Clear\n");
	if(l!=NULL) lclear(l);
	if(l2!=NULL) lclear(l2);
	if(l3!=NULL) lclear(l3);
	assert(is_empty(l)&&is_empty(l2)&&is_empty(l3));
	print_list(l); // Empty
	print_list(l2); // Empty
	print_list(l3); // Empty


	free_list(l);
	free_list(l2);
	free_list(l3);
	
}


int main(){
	test();
	
	return 0;
}

