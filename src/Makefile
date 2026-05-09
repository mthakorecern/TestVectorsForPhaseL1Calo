CXX := g++
HLS_ROOT := /opt/Xilinx/Vitis_HLS/2023.1

CXXFLAGS := -O2 -std=c++11

COMMON_INCLUDES := \
	-I$(HLS_ROOT)/include \
	-I../test/Tester \
	-I../test/Adapters 
	
IP1_INCLUDES :=	-I../scripts/FW/IP1
IP21_INCLUDES := -I../scripts/FW/IP21
IP22_INCLUDES := -I../scripts/FW/IP22
IP3_INCLUDES := -I../scripts/FW/IP3


all: ip1_tester ip21_tester ip22_tester ip3_tester rct_fw_tester
# TARGET := ip1_tester
# TARGET := zerotester



## Just IP1
IP1_SRCS := 	\
	../test/Tester/ip1_tester.cpp \
	../test/Adapters/fw_adapter_IP1_5x6.cpp \
	../test/Adapters/fw_adapter_IP1_2x6.cpp

IP1_OBJS := $(IP1_SRCS:.cpp=.o)

ip1_tester: $(IP1_OBJS)
	$(CXX) $(CXXFLAGS) $(COMMON_INCLUDES) $(IP1_INCLUDES) -o $@ $(IP1_OBJS)

## Just IP21
IP21_SRCS := \
	../test/Tester/ip21_tester.cpp \
	../test/Adapters/fw_adapter_IP21.cpp \
	../scripts/FW/IP21/IP21.cpp \
	../scripts/FW/IP21/oe.cpp \
	../scripts/FW/IP21/bitonicSort32.cpp
	
IP21_OBJS := $(IP21_SRCS:.cpp=.o)

ip21_tester: $(IP21_OBJS)
	$(CXX) $(CXXFLAGS) $(COMMON_INCLUDES) $(IP21_INCLUDES) -o $@ $(IP21_OBJS)

## Just IP22
IP22_SRCS := \
	../test/Tester/ip22_tester.cpp \
	../test/Adapters/fw_adapter_IP22.cpp \
	../scripts/FW/IP22/IP22.cpp
	
IP22_OBJS := $(IP22_SRCS:.cpp=.o)

ip22_tester: $(IP22_OBJS)
	$(CXX) $(CXXFLAGS) $(COMMON_INCLUDES) $(IP22_INCLUDES) -o $@ $(IP22_OBJS)

## Just IP3

IP3_SRCS := \
	../test/Tester/ip3_tester.cpp \
	../test/Adapters/fw_adapter_IP3.cpp \
	../scripts/FW/IP3/IP3algo_top.cpp

IP3_OBJS := $(IP3_SRCS:.cpp=.o)

ip3_tester: $(IP3_OBJS)
	$(CXX) $(CXXFLAGS) $(COMMON_INCLUDES) $(IP3_INCLUDES) -o $@ $(IP3_OBJS)

## Total RCT Tester

RCT_FW_SRCS := ../test/Tester/rct_fw_tester.cpp
RCT_FW_OBJS := $(RCT_FW_SRCS:.cpp=.o)

rct_fw_tester: $(RCT_FW_OBJS)
	$(CXX) $(CXXFLAGS) $(COMMON_INCLUDES) -o $@ $(RCT_FW_OBJS)




# For zerotester, use:
# SRCS := \
# 	../test/Tester/zerotester.cpp \
# 	../test/Adapters/fw_adapter_IP1_5x6.cpp \
# 	../test/Adapters/fw_adapter_IP1_2x6.cpp


%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(COMMON_INCLUDES) $(IP1_INCLUDES) $(IP21_INCLUDES) $(IP22_INCLUDES) $(IP3_INCLUDES) -c $< -o $@


clean:
	rm -f ip1_tester ip21_tester ip22_tester ip3_tester rct_fw_tester \
	$(IP1_OBJS) $(IP21_OBJS) $(IP22_OBJS) $(IP3_OBJS) $(RCT_FW_OBJS)