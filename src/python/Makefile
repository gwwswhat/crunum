.PHONY: all debug clean install uninstall

PYTHON_VERSION ?= python3

PY_CFLAGS := -c -I../../include -Wall -Wextra -Werror -std=gnu99 -fPIC  $(shell $(PYTHON_VERSION)-config --includes)
PY_LINK_FLAGS := $(shell $(PYTHON_VERSION)-config --ldflags)

PYLIB := $(shell $(PYTHON_VERSION) -m site --user-site)

DEBUG_OBJ := $(OBJ:%.o=%-debug.o)

PY_OBJ := $(OBJ:%.o=py_%.o) py_$(LIBRARY).o

PY_DEBUG_OBJ := $(PY_OBJ:%.o=%-debug.o)

all: $(LIBRARY).so

debug: $(LIBRARY)-debug.so

clean:
	rm -f $(PY_OBJ)
	rm -f $(OBJ)
	rm -f $(LIBRARY).so

install: $(PYLIB)/$(LIBRARY).so

uninstall:
	rm -f $(PYLIB)/$(LIBRARY).so

$(LIBRARY).so: $(PY_OBJ) $(OBJ)
	gcc -shared -fPIC $(PY_LINK_FLAGS) $^ -o $@

$(LIBRARY)-debug.so: $(PY_DEBUG_OBJ) $(DEBUG_OBJ)
	gcc -shared -fPIC $(PY_LINK_FLAGS) -g $^ -o $@

py_%.o: %.c
	gcc $(PY_CFLAGS) -O2 $< -o $@

py_%-debug.o: %.c
	gcc -DDEBUG $(PY_CFLAGS) -O0 -g $< -o $@

$(PYLIB)/$(LIBRARY).so: $(LIBRARY).so
	@mkdir -p $(dir $@)
	mv $< $@
