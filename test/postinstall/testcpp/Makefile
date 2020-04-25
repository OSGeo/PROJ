PROGRAM = testcpp
OBJECTS = $(addsuffix .o,$(PROGRAM))

override CXXFLAGS += -std=c++11 -fvisibility=hidden -g -Wall -Werror $(shell pkg-config proj --cflags)
override LDFLAGS += $(shell pkg-config proj --libs)

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CXX) -o $@ $< $(LDFLAGS)

clean:
	$(RM) $(PROGRAM) $(OBJECTS)

.PHONY: clean
