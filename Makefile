CXX ?= g++
CXXFLAGS ?= -lpulse -lboost_program_options -Wall


## Target if make is invoked without any parameters (goal)
.DEFAULT_GOAL: all

## "Virtual" targets without actual files to update/create
.PHONY: all clean distclean


all: pamixer

pamixer: pamixer.cc pulseaudio.hh
	$(CXX) $(CXXFLAGS) -o $@ pamixer.cc

clean:
	rm -f pamixer

distclean: clean
