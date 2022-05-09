CFLAGS = -g -I$(shell wasmer config --includedir)
LDFLAGS = -Wl,-rpath,$(shell wasmer config --libdir) -lm
LDLIBS = $(shell wasmer config --libs)

example: example.o

.PHONY: all
all: example
