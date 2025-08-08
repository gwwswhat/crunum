.PHONY: all debug clean install uninstall test

LIBRARY := crunum

PREFIX ?= /usr/local

LIBDIR := $(PREFIX)/lib

all:
	$(MAKE) TARGET="$(TARGET)" LIBRARY="$(LIBRARY)" -C src/ all
	mv src/$(TARGET)$(LIBRARY).so .

debug:
	$(MAKE) TARGET="$(TARGET)" LIBRARY="$(LIBRARY)" -C src/ debug
	mv src/$(TARGET)$(LIBRARY)-debug.so .

clean:
	$(MAKE) TARGET="$(TARGET)" LIBRARY="$(LIBRARY)" -C src/ clean

install:
	$(MAKE) TARGET="$(TARGET)" LIBRARY="$(LIBRARY)" LIBDIR="$(LIBDIR)" -C src/ install

uninstall:
	$(MAKE) TARGET="$(TARGET)" LIBRARY="$(LIBRARY)" LIBDIR="$(LIBDIR)" -C src/ uninstall

test:
	$(MAKE) TARGET="$(TARGET)" -C src/ debug
	mv src/$(TARGET)$(LIBRARY)-debug.so test/$(LIBRARY).so
	$(MAKE) TARGET="$(TARGET)" -C test/ test
	rm test/$(LIBRARY).so
