OUT = bin/nl
CC = cc
CFLAGS =
ODIR = obj
SDIR = src
INC = -Iinc

_OBJS = main.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c $(INC) -o $@ $< $(CFLAGS) 

$(OUT): $(OBJS) 
	$(CC) -o $(OUT) $^

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(OUT)