PROGRAM = testappprojinfo
OBJECTS = $(addsuffix .o,$(PROGRAM))

override CFLAGS += -g -Wall -Werror $(shell pkg-config proj --cflags)
override LDFLAGS += $(shell pkg-config proj --libs)

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CC) -o $@ $< $(LDFLAGS)

clean:
	$(RM) $(PROGRAM) $(OBJECTS)

.PHONY: clean
