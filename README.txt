Hello there. Look at this novel concept - a README that isn't empty! Haha!

The main sorting code, in sortcode/Sort.C, parses data that has been unpacked with the Unpack.cpp code that is in ~/repositories/gretina-unpack-2019GODDESS/. 

For the 30P(d,p) GODDESS run (for a few long weeks in Feb-March 2019), the data are written to /global/data1b/gretina/1707_data1b/Run###/Run###.root. If you don't see those root files, you need to unpack them. 

Sort.C loads the gretina and the goddess libraries, loops over each tree and plots histograms. It might make an output tree too, but I'll see how life is going. So if you want new histograms, Sort.C is the place for it. 

To compile from /sortcode:
g++ Sort.C -std=c++0x -I../include -L../../lib `root-config --cflags --libs` -lTreePlayer -lSpectrum -lGRETINA -lGODDESS -o SortData

To run, manually change the TFile name in the sortcode, OR run sorter.sh:

./sorter.sh 096 200

will process runs 96 to 200. Note the preceding 0. This will dump histogram files in /global/data1b/gretina/1707_data1b/Histograms/hist###.root.

Then, add all the histograms you want together! Type this directly into the command line: 
hadd output.root hist*.root

for adding everything. 

TO NOTE:
 - if you change things in the unpacker, like add branches to anything, then you also need to check the src/header files in GODDESS_GLW. Then make, make clean. 
 - the calibrations for the silicon are read into Sort.C. So you don't have to unpack again and again. 

Ta duck,
Gemma
gem@anl.gov

Special thanks to Jack "Henderswan" Henderson for helping me to understand the unpacker's output for GODDESS, particularly. Cheers, bru. 
