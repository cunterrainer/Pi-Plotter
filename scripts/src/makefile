CXXFILE=setup.cpp
GOFILE=download.go
LIBFILE=$(addprefix lib,$(GOFILE:.go=.a))
LIBHEADER=$(LIBFILE:.a=.h)

CXX=g++
CXXFLAGS=-D$(PLATFORM) -O2 -std=c++17

ifeq ($(OS),Windows_NT)
	BINFILE=$(CXXFILE:.cpp=.exe)
	PLATFORM=WINDOWS
	RMCMD=del /f
else
	BINFILE=$(CXXFILE:.cpp=)
	PLATFORM=LINUX
	RMCMD=rm -f
endif


.PHONY: all build_go_library buid_cxx_bin run clean clean_all

all: build_go_library build_cxx_bin

build: all clean

build_go_library:
	go build -buildmode=c-archive -o $(LIBFILE) $(GOFILE)

build_cxx_bin:
	$(CXX) $(CXXFLAGS) $(CXXFILE) $(LIBFILE) -o $(BINFILE)
	strip $(BINFILE)

run: all
	./$(BINFILE) $(args)

clean:
	$(RMCMD) $(LIBFILE) $(LIBHEADER)

clean_all: clean
	$(RMCMD) $(BINFILE)
