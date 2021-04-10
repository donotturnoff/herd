.SUFFIXES:
.SUFFIXES: .c .o

CC=gcc
CFLAGS=-Wall --pedantic -std=c99 #-fsanitize=address
LDLIBS=
OBJDIR=obj
vpath %.c src
vpath %.h src

objects = $(addprefix $(OBJDIR)/, grammar.o utils.o first_test.o )

herd : $(objects)
	$(CC) $(CFLAGS) -o herd $(objects) $(LDLIBS)

$(OBJDIR)/%.o : %.c herd.h
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY : clean
clean :
	rm $(objects)
