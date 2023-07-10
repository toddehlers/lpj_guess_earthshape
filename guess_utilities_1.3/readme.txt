UTILITIES - LPJ-GUESS related utilities
=======================================

This package contains a collection of tools which can be useful when working
with LPJ-GUESS. Most of them are used when post processing output data from
the model.

To get instructions about what a program does and how to use it, run the
program with the "-help" command line argument, such as:

   aslice -help


Included tools
--------------

append      joins two or more files end-to-end, e.g. from different windows or nodes

aslice      area-weighted averages for a geographic area 

balance     computes pools vs. flux balance values for verification of model output

bbox        limits a gridlist to a bounding box

cbalance    computes carbon balance values for verification of model output,
            obsolete, use balance instead

clean       describe content, identify and repair minor formatting errors

compute     add, remove or transform data using a spreadsheet-like formula

delta       compute difference between corresponding items in two output files,
            e.g. two timeslices

dominance   gets the dominant species per grid cell from e.g. LAI output file

extract     extract records matching a logical expression, e.g. a particular range 
            of years, gridcells, PFT abundance etc 

gmap        produces a map as a postscript file, may be viewed or converted to other 
            graphical formats using gs (GhostScript) 

gmapall     runs gmap for each PFT

gplot       plots two-dimensional data

joyn        joins items from two files based on common values, e.g. for a particular 
            grid cell or grid cell+time slice (similar to a GIS join) 

tslice      average values for a timeslice


Dependencies
------------
The gmap program produces output in postscript format. To view or convert to other graphical
formats gs (GhostScript) should be installed on your system.

Building
--------
The utilities package is currently only available in a Unix version. Most of
the programs should be fairly easy to get running under other platforms with a
little extra work however.

To build all the tools on a Unix system, simply run "make".


Installing
----------
After building the programs, all the tools will be available under the "bin"
directory. To be able to use the programs, either make sure that this directory
is in your PATH environment variable, or copy the tools you want to a location
which is already in your PATH.

Adding the bin directory to PATH is done differently depending on your shell.

If you're using bash, you can for instance add something like this to your
.bashrc file (in your home directory):

   export PATH=$PATH:/home/joe/utilities/bin

If you're using csh or tcsh, add something like this to your .cshrc file
(in your home directory):

   set PATH = ($PATH /home/joe/utilities/bin)


Contact
-------
Questions, comments and bug reports can be sent to joe.lindstrom@nateko.lu.se
