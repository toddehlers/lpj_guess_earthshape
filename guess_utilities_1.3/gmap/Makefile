######################################################################
# Make file for GMAP 
# Written by Ben Smith, 2007-07-17
#
# To build executable (gmap):
#      make
#
# To clean up (removes object files and custom libraries):
#      make clean
#
# To install (adds symbolic link to /usr/local/bin)
#      - become superuser (log in as root)
#      - type: make install

# File name root
root= gmap

# Install directory 
installdir= /usr/local/bin

# Include file location
include= -I./bengraph -I../gutil -I./libproj4 -I.

# Library file location
libpath= -L./bengraph -L./libproj4 

# Library linkage
libs= -lbengraph -lproj4 -lm ../gutil/gutil.o

# Compiler options
options= -w -O2 

# Compiler
CC= g++

# Nothing to change past this point
######################################################################

# Dependencies
bengraph= ./bengraph/libbengraph.a
libproj4= ./libproj4/libproj4.a
gutil= ../gutil/gutil.o
mansource=./doc/gmap.man
manpage=$(mansource).gz

# Individual files
source= $(root).cpp
gmappath= gmap_path.h
obj= $(root).o
exe= $(root)

$(exe): $(obj) $(bengraph) $(libproj4) $(gutil) $(manpage)
	$(CC) $(options) $(libpath) -o $(exe) $(obj) $(libs)
	if ls gmap ; then echo ; echo 'Build was successful' ; else echo ; echo 'Build not complete' ; fi

$(bengraph):
	cd bengraph ; make

$(libproj4):
	cd libproj4 ; make

$(obj): $(source) $(gmappath)
	$(CC) $(options) $(include) -c -o $(obj) $(source)

$(gmappath):
	sh makeheader $(PWD)

clean:
	rm -f *.o
	rm -f */*.o */*.a
	rm -f $(manpage)

$(manpage): $(mansource)
	gzip $(mansource) --stdout > $(manpage)

install:
	ln -s `pwd`/$(exe) $(installdir)
