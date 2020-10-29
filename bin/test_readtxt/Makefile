TARGET_ARCH = linux64
CC     = g++
OPT    = -O3
DEBUG  = -g
OTHER  = # -Wall -Wno-deprecated
CFLAGS = $(OPT)

VPATH = ../src:.
MODULE = noxim
SRCS = ./NoximMain.cpp 
OBJS_C = $(SRCS_C:.c=.o )
OBJS = $(SRCS:.cpp=.o)
OBJS_A = $(OBJS) 
OBJS_A += $(OBJS_C)
include ./Makefile.defs
