.PHONY: all clean install uninstall test

LIBRARY := crunum

PREFIX ?= /usr/local

LIBDIR := $(PREFIX)/lib

all:
	$(MAKE) TARGET="$(TARGET)" LIBRARY="$(LIBRARY)" -C src/ all
	mv src/$(TARGET)$(LIBRARY).so .

clean:
	$(MAKE) TARGET="$(TARGET)" LIBRARY="$(LIBRARY)" -C src/ clean

install:
	$(MAKE) TARGET="$(TARGET)" LIBRARY="$(LIBRARY)" LIBDIR="$(LIBDIR)" -C src/ install

uninstall:
	$(MAKE) TARGET="$(TARGET)" LIBRARY="$(LIBRARY)" LIBDIR="$(LIBDIR)" -C src/ uninstall

test:
	$(MAKE) TARGET="$(TARGET)" -C src/ all
	mv src/$(TARGET)$(LIBRARY).so test/$(LIBRARY).so
	$(MAKE) TARGET="$(TARGET)" -C test/ test
	rm test/$(LIBRARY).so
