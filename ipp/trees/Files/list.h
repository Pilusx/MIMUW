#include <stdbool.h>

typedef struct _ListNode* ListNode;
typedef struct _List* List; // of unsigned ints

List new_list(); // O(1)
void free_list(List l); // O(n)
void print_list_stdout (List l); // O(n)

bool is_empty(List l); // O(1)
int length(List l); // O(n)

ListNode push_back(int k, List l);
// Takes the last element or returns -1 if empty
int peek_back(List l); // O(1)
void pop_back(List l); // O(1)
void pop_listnode(ListNode ln); // O(1)

/* Inserts the listnodes from list in the place of the listnode 
 * and destroys the node, node and list becomes null
 */
void replace_node_by_list(ListNode *node, List *list);
List split_list(ListNode w, List l);
