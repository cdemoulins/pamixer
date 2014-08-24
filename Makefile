LDLIBS   ?= -lpulse -lboost_program_options
CXXFLAGS ?= --std=c++11 -Wall -Werror -Wextra -pedantic

## Target if make is invoked without any parameters (goal)
.DEFAULT_GOAL: all

## "Virtual" targets without actual files to update/create
.PHONY: all clean distclean


all: pamixer

pamixer: pulseaudio.o device.o pamixer.o
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	rm -f pulseaudio.o
	rm -f device.o
	rm -f pamixer.o

distclean: clean
	rm -f pamixer

