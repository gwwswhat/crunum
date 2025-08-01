.PHONY: test

LUA ?= lua

LUA_SCRIPTS := $(wildcard *.lua)

test:
	@for lua_script in $(LUA_SCRIPTS); do \
		$(LUA) $$lua_script; \
	done
