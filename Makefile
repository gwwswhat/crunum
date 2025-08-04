.PHONY: all clean install uninstall test

LIBRARY := crunum

PREFIX ?= /usr/local

LIBDIR := $(PREFIX)/lib

all:
	$(MAKE) TARGET="$(TARGET)" -C src/ all

clean:
	$(MAKE) TARGET="$(TARGET)" -C src/ clean

install:
	$(MAKE) TARGET="$(TARGET)" LIBDIR="$(LIBDIR)" -C src/ install

uninstall:
	$(MAKE) TARGET="$(TARGET)" LIBDIR="$(LIBDIR)" -C src/ uninstall

test:
	$(MAKE) TARGET="$(TARGET)" -C src/ all
	mv src/$(TARGET)$(LIBRARY).so test/$(LIBRARY).so
	$(MAKE) TARGET="$(TARGET)" -C test/ test
	rm test/$(LIBRARY).so
