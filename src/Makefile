.PHONY: all clean install uninstall

CFLAGS := -c -I../include -Wall -Wextra -Werror -std=gnu99 -fPIC

LIBRARY := crunum

OBJ := vector.o \
			 matrix.o

all: $(TARGET)/$(LIBRARY).so
	mv $< ./$(TARGET)$(LIBRARY).so

clean:
	rm -f $(LIBRARY)
	rm -f $(OBJ)
	$(MAKE) OBJ="$(OBJ)" -C $(TARGET)/ clean

install: $(TARGET)/$(LIBRARY).so
	$(MAKE) LIBRARY="$(LIBRARY)" LIBDIR="$(LIBDIR)" -C $(TARGET) install

uninstall:
	$(MAKE) LIBRARY="$(LIBRARY)" LIBDIR="$(LIBDIR)" -C $(TARGET) uninstall

$(TARGET)/$(LIBRARY).so: $(OBJ)
	cp $(OBJ) $(TARGET)/
	$(MAKE) OBJ="$(OBJ)" LIBRARY="$(LIBRARY)" -C $(TARGET)/ all

%.o: %.c
	gcc $(CFLAGS) -O2 $< -o $@
