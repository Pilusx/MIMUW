typedef struct _Tree* Tree;

void print_children(int k, Tree t);

Tree new_tree(int max_num_of_nodes);
void free_tree(Tree t);

void add_node(int k, Tree t);
int rightmost_node(int k, Tree t);
void delete_node(int k, Tree t);
void delete_subtree(int k, Tree t);
void split_node(int k, int w, Tree t);
int nodes(Tree t);
