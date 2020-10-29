TARGET_ARCH = linux64
CC     = g++
OPT    = -O3
DEBUG  = -g
OTHER  = # -Wall -Wno-deprecated
CFLAGS = $(OPT)

VPATH = ../src:.
MODULE = noxim
SRCS = ./NoximNoC.cpp ./NoximRouter.cpp ./NoximProcessingElement.cpp ./NoximBuffer.cpp \
	./NoximStats.cpp ./NoximGlobalStats.cpp ./NoximGlobalRoutingTable.cpp \
	./NoximLocalRoutingTable.cpp ./NoximGlobalTrafficTable.cpp ./NoximReservationTable.cpp \
	./NoximPower.cpp ./NoximCmdLineParser.cpp ./NoximMain.cpp ./thermal_IF.cpp \
	./NNModel.cpp
SRCS_C = ./util.c ./temperature_grid.c ./temperature_block.c ./temperature.c \
        ./shape.c ./RCutil.c ./npe.c ./package.c ./flp.c
OBJS_C = $(SRCS_C:.c=.o )
OBJS = $(SRCS:.cpp=.o)
OBJS_A = $(OBJS) 
OBJS_A += $(OBJS_C)
include ./Makefile.defs
