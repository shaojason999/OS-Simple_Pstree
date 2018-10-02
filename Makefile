CC := gcc
override CFLAGS += -O3 -Wall

SOURCE := simple_pstree.c
BINARY := simple-pstree

GIT_HOOKS := .git/hooks/applied

all: $(GIT_HOOKS) $(BINARY)

$(GIT_HOOKS):
	@.githooks/install-git-hooks
	@echo

$(BINARY): $(SOURCE) $(patsubst %.c, %.h, $(SOURCE))
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f *.o $(BINARY)
