
LIB=-L/usr/local/lib -L/usr/X11R6/lib -lX11 ../x11grid/x11grid.a
INC=-I. -I /usr/X11R6/include -I /usr/local/include -I ../x11grid -I ../exexml

INCS=nodedisplay.h trees.h tree.h treemap.h treeset.h treeintegrity.h motivation.h journal.h

FNIT=-fno-implicit-templates
FREPO=-frepo

LINKSETTINGS= $(FREPO) -I. -lpthread $(LIB) $(INC) -w -g
COMPILESETTINGS= $(FNIT) $(FREPO) -c -I. $(LIB) $(INC) -w -g

trees: main.o trees.o nodedisplay.o treexml.o
	g++ $(LINKSETTINGS)  main.o trees.o nodedisplay.o treexml.o -o trees 

tester: tester.cpp tree.h treemap.h treeset.h treeintegrity.h
	g++ -I. tester.cpp -o tester -g

stress: stress.cpp tree.h treemap.h treeset.h 
	g++ -I. stress.cpp -o stress -g

iterators: iterators.cpp tree.h treemap.h treeset.h 
	g++ -I. iterators.cpp -o iterators -g

nodedisplay.o: nodedisplay.cpp $(INCS)
	g++ $(COMPILESETTINGS) nodedisplay.cpp -o nodedisplay.o 

main.o: main.cpp $(INCS)
	g++ $(COMPILESETTINGS) main.cpp -o main.o

trees.o: trees.cpp treexml.h $(INCS)
	g++ $(COMPILESETTINGS) trees.cpp -o trees.o

treexml.o: treexml.cpp treexml.h $(INCS)
	g++ $(COMPILESETTINGS) treexml.cpp -o treexml.o

install: trees
	sudo ./install.sh

clean:
	-rm iterators
	-rm tester
	-rm stress
	-rm trees
	-rm *.o
	-rm *.rpo

