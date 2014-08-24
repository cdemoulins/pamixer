CXX ?= g++
CXXFLAGS ?= -lpulse -lboost_program_options -Wall


## Target if make is invoked without any parameters (goal)
.DEFAULT_GOAL: all

## "Virtual" targets without actual files to update/create
.PHONY: all clean distclean


all: pamixer

pamixer: pulseaudio.o device.o pamixer.o
	$(CXX) $^ $(CXXFLAGS) -o $@

clean:
	rm -f pulseaudio.o
	rm -f device.o
	rm -f pamixer.o

distclean: clean
	rm -f pamixer