.PHONY: all clean

CFLAGS := -c -I ../../include -Wall -Wextra -Werror -std=gnu99

LUA_OBJ := $(OBJ:%.o=lua_%.o) lua_crunum.o

all: ../../$(LIBRARY).so

clean:
	rm -f $(LUA_OBJ)

lua_%.o: %.c
	gcc $(CFLAGS) -O2 $< -o $@

../../$(LIBRARY).so: $(LUA_OBJ) $(OBJ)
	gcc -shared -fPIC $^ -o $@ -llua5.4
