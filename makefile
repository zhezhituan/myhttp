CC = g++
CFLAGS = -Wall -pthread  -g -std=c++11
LIBDIR = lib
BINDIR = bin
OBJDIR = $(BINDIR)/obj
TARGET = main

INCLUDE += 	include/

SRCDIR += src/util
SRCDIR += src/pthread
SRCDIR += src/main
SRCDIR += src/socket
SRCDIR += src/event
SRCDIR += src/http

TARGET_T = $(BINDIR)/$(TARGET)

ALL_SRC += $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.cpp))
ALL_OBJ += $(patsubst %.cpp, $(OBJDIR)/%.o, $(ALL_SRC))
ALL_INCLUDE += $(patsubst %, -I%, $(INCLUDE))
ALLTEST = $(wildcard $(BINDIR)/obj/src/test/*.o)
ALL_OBJ_T = $(filter-out $(ALLTEST),$(ALL_OBJ))
ALL_OBJ_T += $(BINDIR)/obj/src/main/$(TARGET).o

# make obj directory
$(foreach dir, $(SRCDIR), $(shell mkdir -p $(OBJDIR)/$(dir)))



$(OBJDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) $(ALL_INCLUDE) -c $< -o $@
	
$(TARGET_T): $(ALL_OBJ_T)
	$(CC) $(CFLAGS) $(ALL_INCLUDE) $^ -o $@

.PHONY: clean
clean:
	-rm $(BINDIR)/* -rf

echo:
	@echo $(ALL_OBJ_T)
