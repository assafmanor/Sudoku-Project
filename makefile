CC = gcc
OBJS = main.o MainAux.o Parser.o Game.o Solver.o FileManager.o LinkedList.o Stack.o ILP_Solver.o
EXEC = sudoku-console
COMP_FLAG = -ansi -Wall -Wextra \
-Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(OBJS)  $(GUROBI_LIB) -o $@ -lm
	
main.o:    SPBufferset.h MainAux.h Parser.h Game.h
	$(CC) $(COMP_FLAG) -c $*.c
	
MainAux.o: MainAux.h Solver.h Parser.h FileManager.h LinkedList.h ILP_Solver.h
	$(CC) $(COMP_FLAG) -c $*.c
	
Parser.o:  Parser.h Game.h
	$(CC) $(COMP_FLAG) -c $*.c
		
Game.o: Game.h LinkedList.h
	$(CC) $(COMP_FLAG) -c $*.c
	
Solver.o: Game.h Solver.h Stack.h ILP_Solver.h
	$(CC) $(COMP_FLAG) -c $*.c

FileManager.o: FileManager.h Game.h Parser.h
	$(CC) $(COMP_FLAG) -c $*.c
	
LinkedList.o: LinkedList.h
	$(CC) $(COMP_FLAG) -c $*.c
	
Stack.o: Stack.h Game.h
	$(CC) $(COMP_FLAG) -c $*.c

ILP_Solver.o: Game.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
	
clean:
	rm -f $(OBJS) $(EXEC)

