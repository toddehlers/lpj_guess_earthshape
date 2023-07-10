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
INCLUDE= -I. 

# Path to C++ source code
BENGRAPHC= source/bengraph.cpp

# Path to header files
BENGRAPHH= bengraph.h
BENGRAPHPATH= bengraph_path.h

# Path to object files
BENGRAPHO= bengraph.o

# List of target:dependency directives

$(AR): $(BENGRAPHO) $(BENGRAPHPATH)
	ar -r $(AR) $(BENGRAPHO)

$(BENGRAPHO): $(BENGRAPHC) $(BENGRAPHH)
	$(CC) $(INCLUDE) $(CFLAGS) -c $(BENGRAPHC)

$(BENGRAPHPATH):
	sh makeheader $(PWD)
