##################################

RELEASE_STEM = g++
PROD_STEM = clang++ -pedantic -Wall -Werror

STEM = $(PROD_STEM)

##################################

all: dirs bin/venus.out bin/venue.out bin/cove.out bin/benchmark.out

install: all
	sudo cp bin/venus.out /usr/bin/venus
	sudo cp bin/venue.out /usr/bin/venue
	sudo cp bin/cove.out /usr/bin/cove

uninstall:
	sudo rm -f /usr/bin/venus /usr/bin/venue /usr/bin/cove

##################################

recomp:
	bin/venue.out cpuSrc/helloWorld.tasl -o cpuBin/helloWorld.venus
	bin/venue.out cpuSrc/function.tasl -o cpuBin/function.venus

##################################

dirs:	bin/ build/

bin/:
	mkdir bin

build/:
	mkdir build

bin/venus.out:	build/venus.o build/cpu2.o
	$(STEM) -o bin/venus.out build/venus.o build/cpu2.o

bin/venue.out:	build/assemblerMain.o build/assembler.o build/stdmac.o
	$(STEM) -o bin/venue.out build/assemblerMain.o build/assembler.o build/stdmac.o

bin/cove.out:	build/cove.o build/assembler.o build/stdmac.o
	$(STEM) -o bin/cove.out build/cove.o build/assembler.o build/stdmac.o

bin/benchmark.out:	benchmark.cpp
	$(STEM) -o bin/benchmark.out benchmark.cpp

##################################

build/venus.o:	venusMain.cpp
	$(STEM) -c -o build/venus.o venusMain.cpp

build/assemblerMain.o:	assemblerMain.cpp
	$(STEM) -c -o build/assemblerMain.o assemblerMain.cpp

build/cpu2.o:	src/cpu2.cpp src/cpu2.hpp
	$(STEM) -c -o build/cpu2.o src/cpu2.cpp

build/assembler.o:	src/assembler.cpp src/assembler.hpp
	$(STEM) -c -o build/assembler.o src/assembler.cpp

build/cove.o:	src/decompiler.cpp src/assembler.hpp
	$(STEM) -c -o build/cove.o src/decompiler.cpp

build/stdmac.o:	src/stdMacros.cpp src/stdMacros.hpp
	$(STEM) -c -o build/stdmac.o src/stdMacros.cpp

##################################

clean:
	rm build/*

pclean:
	rm bin/* build/*

##################################