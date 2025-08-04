.PHONY: all clean install uninstall

PYTHON_VERSION ?= python3

PY_INCLUDE_FLAGS := -c -I../../include -Wall -Wextra -Werror -std=gnu99  $(shell $(PYTHON_VERSION)-config --includes)
PY_LINK_FLAGS := $(shell $(PYTHON_VERSION)-config --ldflags)

PYLIB := $(shell $(PYTHON_VERSION) -m site --user-site)

PY_OBJ := $(OBJ:%.o=py_%.o) py_crunum.o

all: $(LIBRARY).so

clean:
	rm -f $(PY_OBJ)
	rm -f $(OBJ)
	rm -f $(LIBRARY).so

install: $(PYLIB)/$(LIBRARY).so

uninstall:
	rm -f $(PYLIB)/$(LIBRARY).so

$(LIBRARY).so: $(PY_OBJ) $(OBJ)
	gcc -shared -fPIC $(PY_LINK_FLAGS) $^ -o $@

py_%.o: %.c
	gcc $(PY_INCLUDE_FLAGS) -O2 $< -o $@

$(PYLIB)/$(LIBRARY).so: $(LIBRARY).so
	@mkdir -p $(dir $@)
	mv $< $@
