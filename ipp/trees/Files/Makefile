CC = gcc
# Remove -DNDEBUG to enable assertions
CFLAGS = -g -c -DNDEBUG
FLAGS = -g -o
OBJS = solution.o tree.o list.o file.o 

solution : $(OBJS)
	$(CC) $(FLAGS) solution $(OBJS)

debug : $(OBJS)
	$(CC) $(FLAGS) solution.dbg $(OBJS)


%.o :
	$(CC) $(CFLAGS) $^ 

solution.o : solution.c tree.h file.h
tree.o : tree.c list.h
list.o : list.c
file.o : file.c


clean : 
	rm -f *.o
	rm -f *.gch
	rm -f *.dbg
	rm -f *.dbo
	rm -f solution


# Testing
test_list : test_list.c list.o
	$(CC) $(FLAGS) test_list test_list.c list.o
	
test_tree : test_tree.c tree.o list.o
	$(CC) $(FLAGS) test_tree test_tree.c tree.o list.o
