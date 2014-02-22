
LIB=-L/usr/local/lib -L/usr/X11R6/lib -lX11 ../x11grid/x11grid.a
INC=-I. -I /usr/X11R6/include -I /usr/local/include -I ../x11grid

trees: trees.cpp trees.h tree.h Motivation.h nodedisplay.o
	g++ -I. trees.cpp nodedisplay.o -lpthread -o trees $(LIB) $(INC) -w

nodedisplay.o: NodeDisplay.cpp trees.h tree.h NodeDisplay.h NodeDisplay.cpp
	g++ -c -I. NodeDisplay.cpp -lpthread -o nodedisplay.o $(LIB) $(INC) -w

clean:
	rm trees
	rm *.o
