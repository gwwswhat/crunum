.PHONY: all clean install uninstall test

LIBRARY := crunum

all:
	$(MAKE) -C src/ all

clean:
	$(MAKE) -C src/ clean

install:
	$(MAKE) -C src/ install

uninstall:
	$(MAKE) -C src/ uninstall

test:
	$(MAKE) -C src/ all
	mv $(LIBRARY).so test/
	$(MAKE) -C test/ test
