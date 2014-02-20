TARGET = usetestMethod
CC = g++
CFLAGS = -ggdb $(shell pkg-config --cflags opencv)

LFLAGS = $(shell pkg-config --libs opencv)

SRCDIR = src
OBJDIR = obj
OUTDIR = out

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES = -I./inc
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
rm = rm -f

$(OUTDIR)/$(TARGET): $(OBJECTS)
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LFLAGS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONEY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONEY: remove
remove: clean
	@$(rm) $(OUTDIR)/$(TARGET)
	@echo "Executable removed!"
