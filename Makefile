PROGS:=xsquare
PLATFORM:=$(shell uname -s)
CPP_FILES:=$(wildcard *.cpp) $(wildcard audio/$(PLATFORM)/*.cpp)
OBJ_FILES:=$(patsubst %.cpp,%.o,$(CPP_FILES))
DEP_FILES:=deps.d
CPPFLAGS:=-Wno-write-strings -O3 -I./include -I/opt/X11/include -Iaudio/$(PLATFORM) -MMD -MF $(DEP_FILES)
LIBS:=-L/opt/X11/lib -lX11 -lXext -lpng -lz
FRAMEWORKS:=
ifeq ($(PLATFORM),Darwin)
	FRAMEWORKS+=-framework AudioUnit
endif

all: $(PROGS)

$(PROGS): $(OBJ_FILES)
	$(CXX) -o $(PROGS) $(notdir $(OBJ_FILES)) $(FRAMEWORKS) $(LIBS) $(CPPFLAGS)

%.o: %.cpp
	$(CXX) -c $< $(CPPFLAGS)

clean:
	$(RM) $(DEP_FILES) $(OBJ_FILES) $(PROGS)
