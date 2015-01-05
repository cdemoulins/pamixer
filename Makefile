LDLIBS   ?= -lpulse -lboost_program_options
CXXFLAGS ?= --std=c++11 -Wall -Werror -Wextra -pedantic
PREFIX   ?= /usr/local

## Target if make is invoked without any parameters (goal)
.DEFAULT_GOAL: all

## "Virtual" targets without actual files to update/create
.PHONY: all clean distclean install


all: pamixer

pamixer: pulseaudio.o device.o pamixer.o callbacks.o
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	rm -f pulseaudio.o
	rm -f device.o
	rm -f pamixer.o
	rm -f callbacks.o

distclean: clean
	rm -f pamixer

install: pamixer
	install pamixer $(PREFIX)/bin/
