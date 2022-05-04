CFLAGS = -g -I$(shell wasmer config --includedir)
LDFLAGS = -Wl,-rpath,$(shell wasmer config --libdir)
LDLIBS = $(shell wasmer config --libs)

example: example.o

.PHONY: all
all: example