CURDIR = $(shell pwd)
SRCDIR = $(CURDIR)/src
INCDIR = $(CURDIR)/include
BINDIR = $(CURDIR)/bin

ROOT_LIBS = `root-config --glibs` -lSpectrum -lTreePlayer

LIBRS = -L$(INCDIR) $(ROOT_LIBS)# $(MINSRC) 
INCLUDE = $(INCDIR)# $(MINDIR)

CFLAGS = -std=c++0x -g -fPIC -Wall `root-config --cflags` `gsl-config --cflags` -I$(INCDIR) $(ROOT_LIBS) $(GSLLIBS)

CPP=g++

HEAD = $(wildcard include/*.h)
SHAREDOBJECTS = $(patsubst include/%.h,bin/build/lib%.so,$(HEAD))
OBJECTS = $(patsubst include/%.h,bin/build/%.o,$(HEAD))

TARGET = bin/libGODDESS.so

main: $(TARGET)
	@printf "Make complete\n"

$(TARGET): $(OBJECTS) $(SHAREDOBJECTS) bin/DictOutput.cxx 
	@printf "Now compiling shared library$@\n"
	@$(CPP) $(CFLAGS) -o $@ -shared bin/DictOutput.cxx $(OBJECTS) -I. $(LIBRS) $(GSLLIBS)

bin/DictOutput.cxx: $(HEAD)
	@printf "Linking libraries\n"
	@rootcint -f $@ -c -p $(HEAD) bin/build/linkdef.h

bin/build/lib%.so: src/%.cpp include/%.h
	@printf "Now compiling library $@\n"
	@$(CPP) $(CFLAGS) -o $@ -shared -c $< $(LIBRS) $(GSLLIBS)

bin/build/%.o: src/%.cpp include/%.h
	@printf "Now compiling library $@\n"
	@$(CPP) $(CFLAGS) -o $@ -c $< $(LIBRS) $(GSLLIBS)

clean:  
	@printf "Tidying up...\n"
	@rm $(OBJECTS)
	@rm bin/*.*
