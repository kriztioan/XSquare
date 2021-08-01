PROG:=xsquare
PLATFORM:=$(shell uname -s)
CPP_FILES:=$(wildcard *.cpp) $(wildcard audio/$(PLATFORM)/*.cpp)
OBJ_FILES:=$(patsubst %.cpp,%.o,$(CPP_FILES))
CPPFLAGS:=-Wno-write-strings -O3 -I./include -I/opt/X11/include -Iaudio/$(PLATFORM)
LIBS:=-L/opt/X11/lib -lX11 -lXext -lpng -lz
FRAMEWORKS:=
ifeq ($(PLATFORM),Darwin)
	FRAMEWORKS+=-framework AudioUnit
endif

$(PROG): $(OBJ_FILES)
	$(CXX) -o $(PROG) $(notdir $(OBJ_FILES)) $(FRAMEWORKS) $(LIBS)

%.o: %.cpp
	$(CXX) -c $< $(CPPFLAGS)

clean:
	$(RM) *.o $(PROG)
