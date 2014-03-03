##目标文件
TARGET = libtestMethod.so

PWD = $(shell pwd)
CC = g++
#编译动态库
CFLAGS = -ggdb -fPIC -shared -Wall

#头文件
INCLUDES = -Iinc $(shell pkg-config --cflags opencv)

#需要的动态库文件
LFLAGS = $(shell pkg-config --libs opencv)

SRCDIR = src
OBJDIR = obj
OUTDIR = $(PWD)/out

#SOURCES := $(wildcard $(SRCDIR)/*.cpp)
SOURCES = src/mathTest.cpp \
			src/testMethod.cpp \
			src/histRedistribute.cpp \
			src/threshold.cpp

#获取.cpp对应的.o文件名称
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
rm = rm -f






$(OUTDIR)/$(TARGET): $(OBJECTS)
	@$(CC) $(CFLAGS) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LFLAGS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CC) $(CFLAGS) $(INCLUDES) $(SOCFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

#使用make test编译出使用libtestMethod.so的应用
test: $(OUTDIR)/$(TARGET)
	g++ -ggdb -Wall $(INCLUDES) -o $(OUTDIR)/test $(SRCDIR)/usetestMethod.cpp $(OUTDIR)/$(TARGET) $(LFLAGS)



.PHONEY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONEY: remove
remove: clean
	@$(rm) $(OUTDIR)/$(TARGET) $(OUTDIR)/test
	@echo "Executable removed!"
