HLS_ROOT := /opt/Xilinx/Vitis_HLS/2023.1
CXX      := g++
CXXFLAGS := -O2 -std=c++11 -I$(HLS_ROOT)/include

# -----------------------------
# Files
# -----------------------------
SRC      := IP22.cpp
TB       := IP22_tb.cpp
TARGET   := sim_binary
HDR      := IP22algo_top.h

# -----------------------------
# Targets
# -----------------------------
all: $(TARGET)

$(TARGET): $(SRC) $(TB) $(HDR)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC) $(TB)
clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
