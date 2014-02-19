
LIB=-L/usr/local/lib -L/usr/X11R6/lib -lX11 ../x11grid/x11grid.a
INC=-I. -I /usr/X11R6/include -I /usr/local/include -I ../x11grid

trees: trees.cpp trees.h tree.h
	g++ -I. trees.cpp -lpthread -o trees $(LIB) $(INC) -w

clean:
	rm trees
	rm *.o
