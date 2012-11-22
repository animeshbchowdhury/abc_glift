CC := gcc
DIR := src/base/glift
SRC :=
LD := g++

include $(DIR)/module.make.debug

ARCHFLAGS := $(shell $(CC) arch_flags.c -o arch_flags && ./arch_flags)
OPTFLAGS := -g -O -DLIN
LIBS := -L. -labc -ldl -lreadline -lpthread -lm

CFLAGS += -Wall -Wno-unused-function -Wno-unused-but-set-variable $(OPTFLAGS) $(ARCHFLAGS) -I$(PWD)/src
OBJ := $(patsubst %.c, %.o, $(filter %.c, $(SRC)))
DEP := $(OBJ:.o=.d)

%.o: %.c
	@echo "\`\` Compiling:" $(LOCAL_PATH)/$<
	@$(CC) -c $(CFLAGS) $< -o $@

%.d: %.c
	@echo "\`\` Dependency:" $(LOCAL_PATH)/$<
	@./depends.sh $(CC) `dirname $*.c` $(CFLAGS) $*.c > $@

-include $(DEP)

.PHONY: clean

depend: $(DEP)

main: $(OBJ)
	@echo "Finished!"
	@$(LD) -o $@ $^ $(LIBS)

clean:
	rm -f $(OBJ) $(DEP)
