NAME ?= Cconf
VERSION ?= v1
# options: LINUX WINDOWS
PLATFORM ?= LINUX

SOURCE := test.c

LIBS += -lm

IDIR := inc
LDIR := lib
SDIR := src
RDIR := res
BDIR := build

OUTPUT := $(NAME)_$(VERSION)

OBJS = $(patsubst %.c, $(SDIR)/%.o, $(SOURCE))

CC := gcc

ifeq ($(PLATFORM),WINDOWS)
    CC = x86_64-w64-mingw32-gcc
    OUTPUT = $(NAME)_$(VERSION).exe
endif

CFLAGS += -g -std=gnu99 -Wall
CFLAGS += -I$(IDIR)

# -L$(LDIR)

$(NAME): $(OBJS)
	@echo -e 'Building...'
	@$(CC) -o $(BDIR)/$(OUTPUT) $(OBJS) $(CFLAGS) $(LIBS)
	@echo -e 'Copying resources...'
	@cp -rf $(RDIR) $(BDIR)/
	@echo -e 'Cleaning up...'
	@find . -name "*.o" -type f -delete

%.o: %.c
	@$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: clean run

clean:
	@find . -name "*.o" -type f -delete
	@rm -rf $(BDIR)/*

run:
	@echo -e 'Running...'
	@./$(BDIR)/$(OUTPUT)
