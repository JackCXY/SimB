simBP : ED_Main.o GraphReader.o
	g++ -o simBP ED_Main.o GraphReader.o
ED_Main.o: ED_Main.cpp
	g++  -c ED_Main.cpp
GraphReader.o: GraphReader.cpp GraphReader.h
	g++  -c GraphReader.cpp
