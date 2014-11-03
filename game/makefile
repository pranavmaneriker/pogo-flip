
GLLIBS = -lGL -lGLU -lglut -lGLEW 
IMGLIBS = -lSOIL
ALLIBS = -lopenal -lalut

PROGRAMS = game

.SUFFIXES: .cpp .c .h .hpp .cc

all: $(PROGRAMS)

clean: 
	rm -f $(PROGRAMS)
	rm -f *.o
	rm -f *.out

game: main.cpp level.cpp
	g++ main.cpp ../libs/tiny_obj_loader.cc -o $@ $(GLLIBS) $(IMGLIBS) $(ALLIBS)

