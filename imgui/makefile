TARGETS=imgui.a

CC=gcc
OPTIMISE=-O0
DEBUG=-ggdb3
CFLAGS= -fno-strict-aliasing -DUNIV_LINUX \
	-Werror -Wall -Wextra $(OPTIMISE) $(DEBUG)
LIBS=-Wl,-Bsymbolic-functions \
	-rdynamic -lrt -lpthread \
	-lGL -lglfw -lXrandr

OBJECTS=imgui.o

all: $(TARGETS)

clean:
	rm -f $(TARGETS)
	rm -f *.o

%.o: %.cpp
	$(CC) $(CFLAGS) -c $*.cpp -o $*.o

imgui.a: $(OBJECTS)
	ar rcs imgui.a $(OBJECTS)

