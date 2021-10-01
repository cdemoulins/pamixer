GIT_VERSION := "$(shell git describe --abbrev=4 --dirty --always --tags)"
LDLIBS   ?= -lpulse -lboost_program_options
CXXFLAGS ?= -std=c++11 -Wall -Werror -Wextra -pedantic -DVERSION=\"$(GIT_VERSION)\"
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
	install -d $(DESTDIR)$(PREFIX)/bin/
	install $(target) $(DESTDIR)$(PREFIX)/bin/
	install -d $(DESTDIR)$(PREFIX)/man/man1/
	install $(manpage) $(DESTDIR)$(PREFIX)/man/man1/
	gzip $(DESTDIR)$(PREFIX)/man/man1/$(manpage)
