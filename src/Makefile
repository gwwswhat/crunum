.PHONY: all clean

CFLAGS := -c -I../include -Wall -Wextra -Werror -std=gnu99

LIBRARY := crunum

OBJ := vector.o \
			 matrix.o \
			 crunum.o

all: $(LIBRARY).so

clean:
	rm -f $(LIBRARY)
	rm -f $(OBJ)

$(LIBRARY).so: $(OBJ)
	gcc -shared -fPIC $^ -o $@ -llua5.4

%.o: %.c
	gcc $(CFLAGS) -O2 $< -o $@
