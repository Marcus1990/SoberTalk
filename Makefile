#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mygame'
# 'make clean'  removes all .o and executable files
#

UNAME := $(shell uname)
# Define the C compiler to use
CC	= g++
LD	= g++

# Define any compile-time flags
CFLAGS = -Wall -std=c++17 -pedantic -g -pthread
MONGO_CFLAGS = $(shell pkg-config --cflags libmongocxx)
CFLAGS += $(MONGO_CFLAGS)

INC_DIR = inc
SRC_DIR = src
OBJ_DIR = obj
DEP_DIR = .depsrc

# INC_MODULES are added to the include paths
INC_MODULES = modules

INC_DIRS = $(INC_DIR) 
INC_DIRS += $(addprefix $(INC_DIR)/,$(INC_MODULES))

# Define any directories containing header files other than /usr/include
INCLUDES= $(addprefix -I,$(INC_DIRS))

# Define the C source files
SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')

# Define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS = -L/usr/lib

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:

MONGO_LIBS = $(shell pkg-config --libs libmongocxx)
BOOST_LIBS = -lboost_system -lboost_filesystem

LIBS += $(MONGO_LIBS)
LIBS += $(BOOST_LIBS)

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPENDSRC = $(SOURCES:$(SRC_DIR)/%.cpp=%.cpp)

# define the executable file 
TARGETS = talkie #unittest

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean

debug: CFLAGS += -DDEBUG
debug: $(TARGETS)

release: $(TARGETS)

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.Td
COMPILE = $(CC) $(DEPFLAGS) $(CFLAGS) $(INCLUDES) -c 

talkie: $(filter-out $(OBJ_DIR)/unittest/%.o,$(OBJECTS))
	$(LD) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LFLAGS) $(LIBS)
	@echo $@ has been compiled

#unittest: $(filter-out $(OBJ_DIR)/game/main.o,$(OBJECTS))
#	$(LD) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LFLAGS) $(LIBS)
#	@echo $@ has been compiled

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEP_DIR)/%.d
	mkdir -p $(dir $@)
	mkdir -p $(subst $(SRC_DIR), $(DEP_DIR), $(dir $<))
	$(eval add_inc=$(addprefix -I,$(subst $(SRC_DIR), $(INC_DIR), $(dir $<))))
	$(COMPILE) $(add_inc) -o $@ $< 
	mv -f $(DEP_DIR)/$*.Td $(DEP_DIR)/$*.d

$(DEP_DIR)/%.d: ;
.PRECIOUS: $(DEP_DIR)/%.d

-include $(SOURCES:$(SRC_DIR)/%.cpp=$(DEP_DIR)/%.d))

clean:
	$(RM) -r $(DEP_DIR) $(OBJ_DIR) *~ $(TARGETS)
