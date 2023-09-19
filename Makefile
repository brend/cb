OUT = bin/nl
CC = cc
CFLAGS = -g -Wall
ODIR = obj
SDIR = src
INC = -Iinc

_OBJS = main.o stream.o lexer.o parser.o aux.o queue.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(ODIR)/%.o: $(SDIR)/%.c
	mkdir -p $(@D)
	$(CC) -c $(INC) -o $@ $< $(CFLAGS) 

$(OUT): $(OBJS) 
	mkdir -p $(@D)
	$(CC) -o $(OUT) $^

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(OUT)