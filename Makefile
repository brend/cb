OUT = bin/cb
CC = cc
CFLAGS = -Wall -g
ODIR = obj
BDIR = bin
SDIR = src
INC = -Iinc

_OBJS = main.o stream.o lexer.o parser.o aux.o queue.o io.o interpreter.o opt.o types.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(ODIR)/%.o: $(SDIR)/%.c | odir
	$(CC) -c $(INC) -o $@ $< $(CFLAGS) 

$(OUT): $(OBJS)  | bdir
	$(CC) -o $(OUT) $^

.PHONY: clean

odir:
	mkdir -p  $(ODIR)

bdir:
	mkdir -p  $(BDIR)

clean:
	rm -f $(ODIR)/*.o $(OUT)
