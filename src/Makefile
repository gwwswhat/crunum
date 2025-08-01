.PHONY: all clean install uninstall

CFLAGS := -c -I../include -Wall -Wextra -Werror -std=gnu99

LIBRARY := crunum

PREFIX ?= /usr/local

LIBDIR := $(PREFIX)/lib

LUALIB := $(LIBDIR)/lua/5.4

OBJ := vector.o \
			 matrix.o

all: $(LIBRARY).so

clean:
	rm -f $(LIBRARY)
	rm -f $(OBJ)
	$(MAKE) OBJ="$(OBJ)" -C lua/ clean

install: $(LIBRARY).so
	@mkdir -p $(LUALIB)
	cp $< $(LUALIB)

uninstall:
	rm -f $(LUALIB)/$(LIBRARY).so

$(LIBRARY).so: $(OBJ)
	mv $(OBJ) lua/
	$(MAKE) OBJ="$(OBJ)" LIBRARY=$(LIBRARY) -C lua/ all

%.o: %.c
	gcc $(CFLAGS) -O2 $< -o $@
