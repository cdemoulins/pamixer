LDLIBS   ?= -lpulse -lboost_program_options
CXXFLAGS ?= -std=c++11 -Wall -Werror -Wextra -pedantic
PREFIX   ?= /usr/local
RM       ?= rm -f

target   := pamixer
manpage  := pamixer.1
main     := $(addsuffix .cc,$(target))
objects  := $(addsuffix .o,callbacks device pulseaudio)

## Target if make is invoked without any parameters (goal)
.DEFAULT_GOAL: all

## "Virtual" targets without actual files to update/create
.PHONY: all clean distclean install


all: $(target)

$(target): $(main) $(objects)

clean:
	$(RM) $(objects)

distclean: clean
	$(RM) $(target)

install: pamixer
	install $(target) $(PREFIX)/bin/
	install -TD $(manpage) $(PREFIX)/man/man1/$(manpage)
	gzip $(PREFIX)/man/man1/$(manpage)
