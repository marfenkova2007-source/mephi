CONTRIB=../../contrib
NUCLEAR=$(CONTRIB)/nuclear

SOURCES := $(wildcard *.cpp)
RESULT := main
SCRIPTS := $(wildcard *.sh)
SCRIPT_TARGETS := $(SCRIPTS:.sh=_run)

ifeq ($(OS), Windows_NT)
    USER_OS := Windows
else
    USER_OS := $(shell uname -s)
endif

ifneq ($(USER_OS), Darwin)
	ASAN_OPTIONS := detect_leaks=1
endif

OBJECTS := $(SOURCES:.cpp=.o)
CFLAGS := -g -fsanitize=address -std=c++20 -Wall -Werror -I $(CONTRIB)
LDFLAGS := -fsanitize=address

all: run

run: compile
	ASAN_OPTIONS=$(ASAN_OPTIONS) ./$(RESULT)

%_run: %.sh
	/bin/bash $<

compile: $(SOURCES) $(RESULT) $(SCRIPT_TARGETS)

.cpp.o: $(wildcard *.h)
	g++ -c $(CFLAGS) $< -o $@

$(RESULT): $(OBJECTS) libnuclear
	g++ $(OBJECTS) $(LDFLAGS) -L$(NUCLEAR) -lnuclear -o $(RESULT)

libnuclear:
	cd $(NUCLEAR) && $(MAKE) compile

clean:
	rm -f $(OBJECTS) $(RESULT)

