.PHONY: all clean install uninstall

CFLAGS := -c -I ../../include -Wall -Wextra -Werror -std=gnu99

LUA_OBJ := $(OBJ:%.o=lua_%.o) lua_crunum.o

LUALIB := $(LIBDIR)/lua/5.4

all: $(LIBRARY).so

clean:
	rm -f $(LUA_OBJ)
	rm -f $(OBJ)
	rm -f $(LIBRARY).so

install: $(LUALIB)/$(LIBRARY).so

uninstall:
	rm -f $(LUALIB)/$(LIBRARY).so

lua_%.o: %.c
	gcc $(CFLAGS) -O2 $< -o $@

$(LIBRARY).so: $(LUA_OBJ) $(OBJ)
	gcc -shared -fPIC $^ -o $@ -llua5.4

$(LUALIB)/$(LIBRARY).so: $(LIBRARY).so
	@mkdir -p $(dir $@)
	mv $< $@
