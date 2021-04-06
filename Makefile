.SUFFIXES:
.SUFFIXES: .c .o

CC=gcc
CFLAGS=-Wall --pedantic -std=c99 -fsanitize=address
LDLIBS=
OBJDIR=obj
vpath %.c src
vpath %.h src

objects = $(addprefix $(OBJDIR)/, grammar.o utils.o )

utlami : $(objects)
	$(CC) $(CFLAGS) -o utlami $(objects) $(LDLIBS)

$(OBJDIR)/%.o : %.c herd.h
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY : clean
clean :
	rm $(objects)
