CC = gcc
CFLAGS = -Wall -Wextra -pthread

## Remove some warnings.
CFLAGS += -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function

all: linked_list_finegrain linked_list_opt linked_list_lazy 

## Which linked list implementation to use?
LL_FILE = ll_lazy.c


## Additional source files
SRC_FILES = ../common/aff.c

linked_list_finegrain: main.c ll_fgl.c ${SRC_FILES}
	$(CC) $(CFLAGS) $^ -o $@
linked_list_opt:  main.c ll_opt.c ${SRC_FILES}
	$(CC) $(CFLAGS) $^ -o $@
linked_list_lazy:  main.c ll_lazy.c ${SRC_FILES}
	$(CC) $(CFLAGS) $^ -o $@
linked_list_noblocking: main.c ll_nb.c ${SRC_FILES}
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f linked_list_finegrain linked_list_opt linked_list_lazy 

