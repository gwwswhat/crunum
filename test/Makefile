.PHONY: test

LUA ?= lua
PYTHON ?= python

SCRIPTS := matrix \
					 vector

LUA_SCRIPTS := $(SCRIPTS:%=%.lua)
PYTHON_SCRIPTS := $(SCRIPTS:%=%.py)

test:
	@if [ "$(TARGET)" = "lua" ]; then \
		for lua_script in $(LUA_SCRIPTS); do \
		  $(LUA) $$lua_script; \
		done \
	fi
	@if [ "$(TARGET)" = "python" ]; then \
		for python_script in $(PYTHON_SCRIPTS); do \
		  $(PYTHON) $$python_script; \
		done \
	fi
