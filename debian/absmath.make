include makefile.common

# This may change
INCLUDES += -I../Include/DirectXMath
TARGETDIR = ../Lib/x64/Release
TARGET = $(TARGETDIR)/libabsmath.a
SOURDEDIR = ../absmath/absmath
OBJDIR = obj/x64/Release/absmath
PCH = $(SOURDEDIR)/stdafx.h
# End changes

SRC_FILES = $(wildcard $(SOURDEDIR)/*.cpp)

CXXFLAGS += $(DEFINES) $(INCLUDES)

OBJ_FILES := $(patsubst $(SOURDEDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC_FILES))
#$(info $(OBJ_FILES))
GCH = $(PCH).gch

all: $(TARGET)

$(TARGET): $(GCH) $(OBJ_FILES)
	@$(MKDIR_P) $(@D)
	$(LINKCMD_LIB) $(OBJ_FILES)

$(OBJ_FILES): $(SRC_FILES)
	@$(MKDIR_P) $(@D)
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

# PCH
$(GCH): $(PCH)
	$(CXX) -x c++-header $(CXXFLAGS) -c $< -o $@

-include $(OBJ_FILES:.o=.d)

.PHONY: clean
clean:
	rm -f $(GCH) $(OBJ_FILES) $(TARGET)
