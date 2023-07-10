###############################################################################
# Makefile for building all the tools in the package.
#
# Usage:
#
# make         - Builds all tools
# make clean   - Removes .o files
#
# The generated programs and placed in the 'bin' directory.

# Compiler to use
CC = g++

# Compiler options for C++ source files
CXXFLAGS = -w -O3 -g

# Where to search for header files
INCLUDES = -I./gutil

# Descriptions of targets, paths and rules for building targets
# (nothing to change past this point)
###############################################################################

OUTPUT = bin

GUTIL = gutil/gutil.o

.PHONY : clean all

ALL_TARGETS = append aslice cbalance balance clean compute delta extract joyn tslice gmap gplot

all : $(patsubst %, $(OUTPUT)/%, $(ALL_TARGETS))

$(OUTPUT)/append : append.o $(GUTIL)
	$(CC) -o $@ $(CXXFLAGS) $(INCLUDES) $^

$(OUTPUT)/aslice : aslice.o $(GUTIL)
	$(CC) -o $@ $(CXXFLAGS) $(INCLUDES) $^

$(OUTPUT)/cbalance : cbalance.o $(GUTIL)
	$(CC) -o $@ $(CXXFLAGS) $(INCLUDES) $^

$(OUTPUT)/balance : balance.o $(GUTIL)
	$(CC) -o $@ $(CXXFLAGS) $(INCLUDES) $^

$(OUTPUT)/clean : clean.o $(GUTIL)
	$(CC) -o $@ $(CXXFLAGS) $(INCLUDES) $^

$(OUTPUT)/compute : compute.o $(GUTIL)
	$(CC) -o $@ $(CXXFLAGS) $(INCLUDES) $^

$(OUTPUT)/delta : delta.o $(GUTIL)
	$(CC) -o $@ $(CXXFLAGS) $(INCLUDES) $^

$(OUTPUT)/extract : extract.o $(GUTIL)
	$(CC) -o $@ $(CXXFLAGS) $(INCLUDES) $^

$(OUTPUT)/joyn : joyn.o $(GUTIL)
	$(CC) -o $@ $(CXXFLAGS) $(INCLUDES) $^

$(OUTPUT)/tslice : tslice.o $(GUTIL)
	$(CC) -o $@ $(CXXFLAGS) $(INCLUDES) $^

gmap/gmap : $(GUTIL)
	cd gmap && $(MAKE)

gplot/gplot : $(GUTIL)
	cd gplot && $(MAKE)

$(OUTPUT)/gmap : gmap/gmap
	ln -fs ../gmap/gmap $@

$(OUTPUT)/gplot : gplot/gplot
	ln -fs ../gplot/gplot $@

clean :
	rm -rf *.o; \
	rm gutil/*.o; \
	cd gmap && $(MAKE) clean && cd ..; \
	cd gplot && $(MAKE) clean

# Pattern rule for building .o files from .cpp files
%.o : %.cpp
	$(CC) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<
