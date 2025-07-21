.PHONY: all clean install uninstall

CFLAGS := -c -I../include -Wall -Wextra -Werror -std=gnu99

LIBRARY := crunum

PREFIX ?= /usr/local

LIBDIR := $(PREFIX)/lib

LUALIB := $(LIBDIR)/lua/5.4

OBJ := vector.o \
			 matrix.o \
			 crunum.o

all: $(LIBRARY).so

clean:
	rm -f $(LIBRARY)
	rm -f $(OBJ)

install: $(LIBRARY).so
	@mkdir -p $(LUALIB)
	cp $< $(LUALIB)

uninstall:
	rm -f $(LIBDIR)/$(LIBRARY).so

$(LIBRARY).so: $(OBJ)
	gcc -shared -fPIC $^ -o $@ -llua5.4

%.o: %.c
	gcc $(CFLAGS) -O2 $< -o $@
