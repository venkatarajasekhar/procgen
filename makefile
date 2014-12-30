TARGETS=procedural

CC=g++
#CC=clang++
#OPTIMISE=-O2
OPTIMISE=-O0
DEBUG=-ggdb3
CFLAGS= -fno-strict-aliasing -DUNIV_LINUX \
	-Werror -Wall -Wextra $(OPTIMISE) $(DEBUG)
LIBS=-Wl,-Bsymbolic-functions \
	-rdynamic -lrt -lpthread \
	-lGL -lglfw

RAW_COMMONOBJECTS=GameTextures.o GameMeshes.o
RAW_PROCEDURALOBJECTS=
RAW_TARGETOBJECTS=$(TARGETS:%=%.o)
COMMONOBJECTS=$(RAW_COMMONOBJECTS:%=build/%)
PROCEDURALOBJECTS=$(RAW_PROCEDURALOBJECTS:%=build/%)
TARGETOBJECTS=$(RAW_TARGETOBJECTS:%=build/%)
ALLOBJECTS=$(COMMONOBJECTS) $(TARGETOBJECTS) $(PROCEDURALOBJECTS)
CORE=core/core.a
LUA=lua/lua.a

recurse:
	@make -j$(shell getconf _NPROCESSORS_ONLN) -C core all
	@make -j$(shell getconf _NPROCESSORS_ONLN) -C lua all
	@make -j$(shell getconf _NPROCESSORS_ONLN) all

all: $(TARGETS)

build:
	mkdir -p build


-include $(ALLOBJECTS:.o=.d)

clean:
	@make -C core clean
	@make -C lua clean
	rm -f $(TARGETS)
	rm -f build/*

build/%.o: %.cpp | build
	$(CC) $(CFLAGS) -c $*.cpp -o build/$*.o
	$(CC) $(CFLAGS) -MM $*.cpp > build/$*.d
	sed -i '1s/^/build\//' build/$*.d

# testing
procedural: build/procedural.o $(COMMONOBJECTS) $(PROCEDURALOBJECTS) $(CORE) $(LUA)
	$(CC) build/procedural.o $(COMMONOBJECTS) $(PROCEDURALOBJECTS) $(CORE) $(LUA) -o procedural $(LIBS)

compilers: basepackages

basepackages:
	sudo apt-get update -y
	sudo apt-get upgrade -y
	sudo apt-get -y install g++ libglfw-dev xorg-dev libglu1-mesa-dev

run:
	./procedural
