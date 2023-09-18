OUT = bin/nl
CC = cc
CFLAGS = -g -Wall
ODIR = obj
SDIR = src
INC = -Iinc

_OBJS = main.o stream.o lexer.o parser.o aux.o queue.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c $(INC) -o $@ $< $(CFLAGS) 

$(OUT): $(OBJS) 
	$(CC) -o $(OUT) $^

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(OUT)