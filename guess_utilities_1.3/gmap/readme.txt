===============================================================================
                     Graphical visualisation utility GMAP
===============================================================================

GMAP is a visualisation program for latitude-longitude referenced data on a
geographic grid.

Input
=====

The input data to GMAP should be in the form of a plain-text file with rows as
records and columns as items. Each record should contain data corresponding to
one grid cell (pixel) on a regular latitude-longitude grid. Each record
should contain at least three items, latitude, longitude and at least one item
containing a value for output. The same number of items should be present in
each record. Items may be separated by spaces or tabs. There may be an initial
header row containing item (column) labels. If no header row is present, items
are referred to by their 1-based order counting from left to right
(1, 2, ... n, where n is the number of columns).

Here is an extract from a valid input file to GMAP. The first column
contains longitude in degrees (+=east, -=west), the second is latitude in
degrees (+=north, -=south), the third is an item for output:

-7.206  22.3    0.0906
-6.8    22.444  0.092
-6.394  22.586  0.0943333
-5.986  22.725  0.0939333
-5.576  22.863  0.0928

Here is another valid input file, this one with a header row and seven
possible output items:

Lon     Lat     NE TBS IBS        BE         MNE       Grass   Total
-7.206  22.3    0  0   0.0043     0.0103     0.0160333 0.0599  0.0906
-6.8    22.444  0  0   0.00326667 0.0105     0.0165667 0.0615  0.092
-6.394  22.586  0  0   0.00466667 0.0109     0.0169667 0.0618  0.0943333
-5.986  22.725  0  0   0.005      0.0111     0.0177    0.0601  0.0939333
-5.576  22.863  0  0   0.0053     0.0101667  0.0160667 0.0613  0.0928

The following is NOT a valid input file (different number of items in different
rows), blank line, non-numerical data, white space in column header:

Lon     Lat     Value      Land use

-7.206  22.3    0.0906     Forest
-6.8    22.444  0.092      Forest
-6.394  22.586  0.0943333  Crops   0.0109
-5.986  22.725  0.0939333  Forest
-5.576  22.863  0.0928     Crops   0.0021

Output
======

The basic output from GMAP is a PostScript file comprising a map with values
for an output item from the input file divided into classes and shown as
colours or a dither pattern on a raster grid, one grid cell for each
longitude-latitude coordinate (record) in the input data. A legend and title
are also shown. Optional elements include coastlines, political boundaries and
grid lines.

Output to graphical formats other than PostScript or directly to the screen
is possible if GhostScript (http://pages.cs.wisc.edu/~ghost) is also available
on the system on which GMAP is installed. For screen output, an X-server must
be running on the local machine.

PostScript files may be viewed or converted into other graphical formats
using software such as GhostScript (http://pages.cs.wisc.edu/~ghost) or
Adobe Acrobat.

Usage and command syntax
========================

SYNOPSIS
       gmap input-file [ options ]


OPTIONS
       -o output-file
              Pathname for graphical output file. The file  format  is  deter‐
              mined  from  the extension of the specified pathname. Recognised
              extensions are .eps, .jpg, .pdf, .png, .ps and .tif. The default
              is  .ps  (PostScript).  Formats  other  than  PostScript require
              GhostScript (gs) to be installed and available  at  the  command
              line.

       -xv    Preview  graphical  output  on  screen. Ghostscript (gs) must be
              available at the command line, an X server must  be  running  on
              the  local  machine and X11 tunnelling or communication via port
              6000 must be enabled.

       -i item-name | column-number
              Item name or 1-based column number for output data.

       -lon item-name | column-number
              Item name or 1-based column number for longitude data.

       -lat item-name | column-number
              Item name or 1-based column number for latitude data.

       -seps  Columns separated by spaces or single tabs.

       -sept  Columns separated by single tabs.

       -pixsize x y
              Raster pixel dimensions in degrees of longitude (x) and latitude
              (y).

       -pixoffset dx dy
              Offset  in  degrees of centre of pixel from coordinate specified
              in data.

       -c colour-scale [ colour-scale ]
              Colour scheme for scalar data and legends. If  a  second  colour
              scale  is  specified, the first is used for negative values, the
              second  for  positive  values.  Available  schemes:   REDVIOLET,
              VIOLETRED,  REDBLUE,  BLUERED,  REDCYAN, CYANRED, GREYRED, GREY‐
              CYAN, RED, BLUE, GREEN, SHADE,  COOL,  WARM,  WET,  DRY,  DEPTH,
              ALTITUDE, NEEGREEN, NEERED, SOIL, VEGETATION, FIRE, DITHER.

       -s min range nclass
              Data  scale for scalar data legends: min = lower range for first
              class; range = interval between  classes;  nclass  =  number  of
              classes. Overrides rounding parameter if given by -r.

       -s min max
              Data  scale  for  scalar  data  legends: min = approximate lower
              range for lowest class; max = approximate upper range for  high‐
              est  class.  Exact ranges depend on rounding parameter which may
              be set by -r.

       -s     Force plotting as scalar data.

       -cat   Force plotting as categorical data (maximum 100 classes).

       -slog [ min max [ nclass ] ]
              Logarithmic data scale for scalar data legend: min = approximate
              lower  range for lowest class; max = approximate upper range for
              highest class; nclass = approximate number of classes.

       -r round
              Rounding parameter (1-9) for scalar data legend: values  outside
              range 1-9 cause default rounding.

       -nozero
              Suppress "exactly zero" class in data and legend.

       -legend legend-file
              Specifies  file  containing  colours and class values/boundaries
              for  legend.  Colours  are  specified  as  space-delimited   RGB
              triplets  on  a single row. For categorical legends, class value
              is specied on a single row preceding  the  corresponding  colour
              triplet.  For  scalar  legends, class boundaries and colours are
              specified on alternate rows.

              e.g.   Categorical legend:   Scalar legend:
                     1                     2.0
                     1.0 0.0 0.0           1.0 0.0 0.0
                     2                     1.5
                     1.0 0.0 0.5           1.0 1.0 0.0
                     3                     1.0
                     0.3 0.7 0.0           0.0 1.0 1.0
                     4                     0.5
                     0.0 0.0 1.0           0.0 0.0 1.0
                                           0.0

       -hlslegend legend-file
              Specifies file containing colours  and  class  values/boundaries
              for  legend.  Colours  are specified as HLS triplets on a single
              row (H=hue in range 0-360, L=lightess in range 0-100;  S=satura‐
              tion in range 0-100). Format as for RGB legends (see -legend)

       -horiz Forces horizontal legend underneath map.

       -vert  Forces vertical legend beside map.

       -w window
              Geographical  window  to  display, one of: AFRICA, ARCTIC, ASIA,
              AUSTRALIA, BALTIC, BOREAL, EU15, EURASIA, EUROPE, GLOBAL,  GLOB‐
              ALLAND, ICELAND, JAPAN, NAMERICA, NORDIC, SAMERICA, SCANDINAVIA,
              SWEDEN, US48, USA.

       -west longitude -east longitude -south latitude -north latitude
              Boundaries for geographic window to display (override -w). Nega‐
              tive  longitude  west  of  Greenwich, negative latitude south of
              equator.

       -t title
              Title text for display above map. Multi-word  titles  should  be
              given in single quotes (e.g. 'Map of NPP'). The following format
              specifiers may be embedded:

              !b      change to bold font
              !i      change to italic font
              !^      change to superscript font
              !_      change to subscript font
              !p      change to plain font
              !s(x)   change text size to x points
              !c(x)   insert character with code x from symbols character set
              !!      insert exclamation mark

       -p projection
              Geographic projection, one of:

              FAHEY       - pseudocylindrical, limited distortion, default
              WINKEL      - Winkel Tripel, azimuthal, limited distortion
              MERCATOR    - cylindrical, classic cartographic projection
              LAMBERT     - cylindrical equal area
              CYLEQD      - cylindrical equidistant
              MOLLWEIDE   - pseudocylindrical equal area
              GOODE       - Goode Homolosine, pseudocylindrical
              ROBINSON    - pseudocylindrical, common cartographic projection
              NPOLAR      - north polar stereographic
              SPOLAR      - south polar stereographic

       -rot longitude
              Principal meridian for polar projections.

       -bound NONE | COAST | POLIT | SWEDEN
              Vector boundaries to be plotted above raster (none,  coastlines,
              coastlines+political boundaries, Swedish national boundary).

       -nogrid
              Suppresses plotting of grid lines on map.

       -noframe
              Suppresses plotting of frame around map.

       -portrait
              Forces 'portrait' page orientation.

       -landscape
              Forces 'landscape' page orientation.

       -textsize_title points
              Text size for title in points.

       -textsize_legend points
              Text size for legend in points.

       -linewidth points
              Width of grid lines, vectors and map frame in points.

       -mask [ OCEAN | SWEDEN ]
              Mask  out  data  over  oceans  (default)  or around the national
              boundary of Sweden.

       -smooth [ density ]
              Smooth raster by a bicubic interpolation at  the  given  density
              (default  16)  between  data  points  in  both the west-east and
              south-north dimension. The data are first interpolated to a reg‐
              ular latitude-longitude grid by distance-weighted nearest-neigh‐
              bour interpolation (no shift unless the data are not regular  to
              start with).

       -help  Displays this help text.

EXAMPLES
       gmap data.txt -i 3
              Plots  data  from  Column 3 in data.txt using the default colour
              scale, with coastlines shown. Longitude should be given in  Col‐
              umn  1 and latitude in Column 2, no header row present. The num‐
              ber of records should be sufficient to allow  gmap  to  estimate
              pixel sizes from distances between data points.  The coordinates
              are assumed to refer to the southwest corner of the  grid  cell.
              The output will consist of a PostScript file called figure.ps.

       gmap data.txt -i Total
              Plots  data from the column labelled Total in the initial header
              row of data.txt. Longitude item should be labelled "Lon" or sim‐
              ilar; latitude should be labelled "Lat" or similar.

       gmap data.txt -i Total -pixsize 0.5 0.5 -pixoffset 0 0
              As  above,  pixels  are  0.5 x 0.5 degrees, the given coordinate
              refers to the centre of the grid cell.

       gmap data.txt -lon X -lat Y -i Total -o map.jpg
              Longitude given in  column  labelled  "X",  latitude  in  column
              labelled  "Y". If Ghostscript (gs) is available, output is writ‐
              ten to a JPEG file, map.jpg.

       gmap data.txt -c cool warm -proj npolar -s -100 10 20 -t "Arctic  Data"
       -xv
              Plot data using a  north  polar  projection  with  cool  colours
              (green, blue, violet) for negative values and warm colours (yel‐
              low, red, magenta) for positive values. Classes range from  -100
              to  100  with  an interval of 10 between classes. Show the title
              "Arctic Data" above the map.  A  preview  is  displayed  on  the
              screen if possible (see -xv under OPTIONS).

       gmap data.txt -i Total -pixsize 0.5 0.5 -smooth -mask
              Plot  data  from the column labelled Total with a smooth transi‐
              tion between data points. The data are first interpolated  to  a
              0.5 x 0.5 degree grid if they do not already follow such a grid.
              Ocean areas are masked out (appear white).


Acknowledgement
===============

GMAP employs a customised version of the projections library libproj4 by
Gerald I. Evenden (original available at www.remotesensing.org/proj).


Ben Smith
www.nateko.lu.se/personal/benjamin.smith/software
2008-08-22

