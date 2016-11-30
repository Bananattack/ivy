ifeq ($(OS),Windows_NT)
	EXE=.exe
else
	EXE=
endif

IVY_H_HOLDER := include
IVY_CPP_FOLDER := lib
IVY_LIB_FOLDER := lib

IVY_H_MATCH := $(wildcard $(IVY_H_HOLDER)/*.h $(IVY_H_HOLDER)/ivy/*.h)
IVY_CPP_MATCH := $(wildcard $(IVY_CPP_FOLDER)/*.cpp $(IVY_CPP_FOLDER)/ivy/*.cpp)
IVY_H_EXCLUDE := 
IVY_CPP_EXCLUDE := 

IVY_H := $(filter-out $(IVY_H_EXCLUDE), $(IVY_H_MATCH))
IVY_CPP := $(filter-out $(IVY_CPP_EXCLUDE), $(IVY_CPP_MATCH))
IVY_O := $(patsubst %.cpp, %.o, $(IVY_CPP))
IVY_DEPS := $(sort $(patsubst %.o, %.d, $(IVY_O)))
IVY_LIB := $(IVY_LIB_FOLDER)/libivy.a

CXX_FLAGS := -O2 -s -std=c++14 -MMD -Wall -Werror -Wold-style-cast -Wnon-virtual-dtor -Wno-parentheses -Wno-unused-but-set-variable -Wno-unused-but-set-parameter -Wno-unused-variable -L$(IVY_LIB_FOLDER)
LXXFLAGS := -lm -livy
INCLUDES := -I$(IVY_H_HOLDER)
IVYCHR := ivychr(EXE)

all: $(IVYCHR)

$(IVY_O): %.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $< $(INCLUDES)

$(IVY_LIB): $(IVY_O)
	ar cr $(IVY_LIB) $(IVY_O)
	ranlib $(IVY_LIB)

$(IVYCHR): src/ivychr.cpp $(IVY_LIB)
	$(CXX) $(CXX_FLAGS) $< $(LXXFLAGS) -o $@ $(INCLUDES)

clean:
	rm -f $(IVY_O) $(IVY_DEPS) $(IVY_LIB) $(IVYCHR)
