###############################################################################
# MAKEFILE FOR THE BENGRAPH
# 
# Written by Ben Smith, 2007-07-17

# Path to C++ compiler
CC=g++

# C++ compiler options and non-default libraries
CFLAGS= -O3 -w

# (Nothing to modify past this point)
###############################################################################

# Name of the archive file that will contain the library functions
AR= libbengraph.a

# Path to BenGraph header files
INCLUDE= -I. -I../libproj4

# Path to C++ source code
BENGRAPHC= source/bengraph.cpp
BENGRAPHGEOGC= source/bengraph_geog.cpp

# Path to header files
BENGRAPHH= bengraph.h
BENGRAPHPATH= bengraph_path.h

# Path to object files
BENGRAPHO= bengraph.o
BENGRAPHGEOGO= bengraph_geog.o

# List of target:dependency directives

$(AR): $(BENGRAPHO) $(BENGRAPHGEOGO) 
	ar -r $(AR) $(BENGRAPHO) $(BENGRAPHGEOGO)
	
$(BENGRAPHO): $(BENGRAPHC) $(BENGRAPHH)
	$(CC) $(INCLUDE) $(CFLAGS) -c $(BENGRAPHC)

$(BENGRAPHGEOGO): $(BENGRAPHGEOGC) $(BENGRAPHH) $(BENGRAPHPATH)
	$(CC) $(INCLUDE) $(CFLAGS) -c $(BENGRAPHGEOGC)

$(BENGRAPHPATH):
	sh makeheader $(PWD)
