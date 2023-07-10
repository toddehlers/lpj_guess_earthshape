######################################################################
# Make file for GPLOT 
# Written by Ben Smith, 2008-08-21
#
# To build executable (gplot):
#      make
#
# To clean up (removes object files and custom libraries):
#      make clean
#
# To install (adds symbolic link to /usr/local/bin)
#      - become superuser (log in as root)
#      - type: make install

# File name root
root= gplot

# Install directory 
installdir= /usr/local/bin

# Include file location
include= -I./bengraph -I../gutil -I.

# Library file location
libpath= -L./bengraph 

# Library linkage
libs= -lbengraph -lm ../gutil/gutil.o

# Compiler options
options= -w -O2 

# Compiler
CC= g++

# Nothing to change past this point
######################################################################

# Dependencies
bengraph= ./bengraph/libbengraph.a
gutil= ../gutil/gutil.o
mansource=./doc/gplot.man
manpage=$(mansource).gz

# Individual files
source= $(root).cpp
gplotpath= gplot_path.h
obj= $(root).o
exe= $(root)

$(exe): $(obj) $(bengraph) $(gutil) $(manpage)
	$(CC) $(options) $(libpath) -o $(exe) $(obj) $(libs)
	if ls gplot ; then echo ; echo 'Build was successful' ; else echo ; echo 'Build not complete' ; fi

$(bengraph):
	cd bengraph ; make

$(obj): $(source) $(gplotpath)
	$(CC) $(options) $(include) -c -o $(obj) $(source)

$(gplotpath):
	echo $(PWD)
	sh makeheader $(PWD)

clean:
	rm -f *.o
	rm -f */*.o */*.a
	rm -f $(gplotpath)
	rm -f $(manpage)

$(manpage): $(mansource)
	gzip $(mansource) --stdout > $(manpage)

install:
	ln -s `pwd`/$(exe) $(installdir)
