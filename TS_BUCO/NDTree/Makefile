C_FLAGS = -Wall -Wextra -lm -lrt -O2 -ggdb
#C_FLAGS = -Wall -Wextra -lm -lrt -ggdb 

all : main

main : main.o tabNDS.o tabSol.o dominance.o treeNDS.o
	gcc $^ ${C_FLAGS} -o main
     
%.o : %.c
	gcc ${C_FLAGS} -c $<	

# Dependance des headers 
dominance : dominance.h tabNDS.h 
tabNDS : tabNDS.h tabSol.h dominance.h
tabSol : tabSol.h
treeNDS : treeNDS.h tabSol.h dominance.h tabNDS.h

clean :
	rm -f *.o
	rm -f *~
	rm -f main	
