ifeq ($(OS),Windows_NT)
	EXE=.exe
else
	EXE=
endif

CC := gcc
CXX := g++

IVY_SRC_FOLDER := src/ivy
IVY_H_MATCH := $(wildcard $(IVY_SRC_FOLDER)/*.h)
IVY_CPP_MATCH := $(wildcard $(IVY_SRC_FOLDER)/*.cpp)
IVY_H_EXCLUDE := 
IVY_CPP_EXCLUDE :=

IVY_H := $(filter-out $(IVY_H_EXCLUDE), $(IVY_H_MATCH))
IVY_CPP := $(filter-out $(IVY_CPP_EXCLUDE), $(IVY_CPP_MATCH))
IVY_O := $(patsubst %.cpp, obj/%.o, $(IVY_CPP))
IVY_DEPS := $(sort $(patsubst %.o, %.d, $(IVY_O)))
IVY_LIB := obj/libivy.a

GIFLIB_SRC_FOLDER := src/giflib/lib
GIFLIB_H := $(wildcard $(GIFLIB_SRC_FOLDER)/*.h)
GIFLIB_C := $(wildcard $(GIFLIB_SRC_FOLDER)/*.c)
GIFLIB_O := $(patsubst %.c, obj/%.o, $(GIFLIB_C))
GIFLIB_LIB := obj/libgiflib.a

CFLAGS := -O2 -Wall -Werror -Lobj
CXXFLAGS := -O2 -std=c++14 -Wall -Werror -Wold-style-cast -Wnon-virtual-dtor -Wno-parentheses -Wno-unused-but-set-variable -Wno-unused-but-set-parameter -Wno-unused-variable -Lobj
LXXFLAGS := -lm -livy -lgiflib
INCLUDES := -Isrc -I$(IVY_SRC_FOLDER) -I$(GIFLIB_SRC_FOLDER)
IVY := ivy$(EXE)

AR := ar
ARFLAGS := cr
RANLIB := ranlib

.PHONY: clean all

define uniq
	$(eval seen :=)
	$(foreach _,$1,$(if $(filter $_,${seen}),,$(eval seen += $_)))
	${seen}
endef
DIRECTORIES := $(call uniq, $(dir \
	$(IVY_O) \
	$(GIFLIB_O) \
	))

all: $(DIRECTORIES) $(IVY)

$(IVY_O): obj/%.o: %.cpp $(IVY_H)
	$(CXX) $(CXXFLAGS) -MMD -c -o $@ $< $(INCLUDES)

$(IVY_LIB): $(IVY_O)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

$(GIFLIB_O): obj/%.o: %.c $(GIFLIB_H)
	$(CC) $(CFLAGS) -c -o $@ $< $(INCLUDES)

$(GIFLIB_LIB): $(GIFLIB_O)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

$(IVY): src/ivy/ivy.cpp $(IVY_LIB) $(GIFLIB_LIB)
	$(CXX) $(CXXFLAGS) $< $(LXXFLAGS) -o $@ $(INCLUDES)

clean:
	rm -rf obj $(IVY)

define makedir
$(1):
	@mkdir -p $(1)
endef
$(foreach d, $(DIRECTORIES), $(eval $(call makedir, $(d))))	

-include $(IVY_DEPS)