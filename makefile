
LIB=-L/usr/local/lib -L/usr/X11R6/lib -lX11 ../x11grid/x11grid.a
INC=-I. -I /usr/X11R6/include -I /usr/local/include -I ../x11grid

INCS=nodedisplay.h trees.h tree.h treeintegrity.h motivation.h

trees: main.o trees.o nodedisplay.o
	g++ -I. main.o trees.o nodedisplay.o -lpthread -o trees $(LIB) $(INC) -w

nodedisplay.o: nodedisplay.cpp $(INCS)
	g++ -c -I. nodedisplay.cpp -lpthread -o nodedisplay.o $(LIB) $(INC) -w

main.o: main.cpp $(INCS)
	g++ -c -I. main.cpp -lpthread -o main.o $(LIB) $(INC) -w

trees.o: trees.cpp $(INCS)
	g++ -c -I. trees.cpp -lpthread -o trees.o $(LIB) $(INC) -w

clean:
	rm trees
	rm *.o
