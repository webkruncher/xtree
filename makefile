
LIB=-L/usr/local/lib -L/usr/X11R6/lib -lX11 ../x11grid/x11grid.a
INC=-I. -I /usr/X11R6/include -I /usr/local/include -I ../x11grid

INCS=nodedisplay.h trees.h tree.h treeintegrity.h motivation.h

FNIT=-fno-implicit-templates
FREPO=-frepo

LINKSETTINGS= $(FREPO) -I. -lpthread $(LIB) $(INC) -w
COMPILESETTINGS= $(FNIT) $(FREPO) -c -I. -lpthread $(LIB) $(INC) -w

trees: main.o trees.o nodedisplay.o
	g++ $(LINKSETTINGS)  main.o trees.o nodedisplay.o -o trees 


	#g++ $(LINKSETTINGS)  -I. main.o trees.o nodedisplay.o -lpthread -o trees $(LIB) $(INC) -w

nodedisplay.o: nodedisplay.cpp $(INCS)
	g++ $(COMPILESETTINGS) nodedisplay.cpp -o nodedisplay.o 


	#g++ $(COMPILESETTINGS) nodedisplay.cpp -o nodedisplay.o 

main.o: main.cpp $(INCS)
	g++ $(COMPILESETTINGS) main.cpp -o main.o
	
	#g++ $(COMPILESETTINGS) -c -I. main.cpp -lpthread -o main.o $(LIB) $(INC) -w

trees.o: trees.cpp $(INCS)
	g++ $(COMPILESETTINGS) trees.cpp -o trees.o

	#g++ $(COMPILESETTINGS) -c -I. trees.cpp -lpthread -o trees.o $(LIB) $(INC) -w

clean:
	-rm trees
	-rm *.o
	-rm *.rpo
