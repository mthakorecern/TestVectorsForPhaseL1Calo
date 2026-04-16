CXX := g++
HLS_ROOT := /opt/Xilinx/Vitis_HLS/2023.1

CXXFLAGS := -O2 -std=c++11
INCLUDES := \
	-I$(HLS_ROOT)/include \
	-I../test/Tester \
	-I../test/Adapters \
	-I../scripts/FW/IP1

TARGET := /outputs/ip1_tester
# TARGET := zerotester

SRCS := \
	../test/Tester/ip1_tester.cpp \
	../test/Adapters/fw_adapter_IP1_5x6.cpp \
	../test/Adapters/fw_adapter_IP1_2x6.cpp

# For zerotester, use:
# SRCS := \
# 	../test/Tester/zerotester.cpp \
# 	../test/Adapters/fw_adapter_IP1_5x6.cpp \
# 	../test/Adapters/fw_adapter_IP1_2x6.cpp

OBJS := $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)