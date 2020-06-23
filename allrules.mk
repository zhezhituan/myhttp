ALL_SRC += $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.cpp))
ALL_OBJ += $(patsubst %.cpp, $(OBJDIR)/%.o, $(ALL_SRC))
ALL_INCLUDE += $(patsubst %, -I%, $(INCLUDE))
ALLTEST = $(wildcard $(BINDIR)/obj/src/test/*.o)
ALL_OBJ_T = $(filter-out $(ALLTEST),$(ALL_OBJ))
ALL_OBJ_T += $(BINDIR)/obj/src/test/$(TARGET).o

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
