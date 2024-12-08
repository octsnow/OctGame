encode = $(subst $() ,*,$(1))
decode = $(subst *,$() ,$(1))
quotes = $(addprefix ",$(addsuffix ",$(1)))
includes = $(addprefix -I,$(1))

RM = cmd.exe /C del

SRC_DIR = src
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.obj,$(SRCS))
OBJDS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%_d.obj,$(SRCS))
INC_DIR = includes ..\..\Wav\includes C:\glut\freeglut-3.2.1\install\include $(call encode,C:\Program Files (x86)\OpenAL 1.1 SDK\include)

INC_DIR := $(call decode,$(call includes,$(call quotes,$(INC_DIR))))

TARGET = lib\oct_game

CXXFLAGS = -EHsc -std:c++20 -c
PRE =

.SUFFIXES:.obj .cpp

.PHONY: all debug release build
all: debug release

debug: $(OBJDS)
	lib $(OBJDS) -OUT:$(TARGET)_d.lib

release: $(OBJS)
	lib $(OBJS) -OUT:$(TARGET).lib

$(OBJ_DIR)/%_d.obj: $(SRC_DIR)/%.cpp
	cl $(CXXFLAGS) -DOCT_DEBUG $< -Fo$@ $(INC_DIR)

$(OBJ_DIR)/%.obj: $(SRC_DIR)/%.cpp
	cl $(CXXFLAGS) $< -Fo$@ $(INC_DIR)

clean:
	-$(RM) $(subst /,\,$(OBJS)) $(subst /,\,$(OBJDS)) $(subst /,\,$(TARGET).lib) $(subst /,\,$(TARGET)_d.lib)
