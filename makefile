CC = g++
CFLAGS = -Wall -pthread  -g -std=c++11
LIBDIR = lib
BINDIR = bin
OBJDIR = $(BINDIR)/obj
TARGET = main

INCLUDE += 	include/

SRCDIR += src/util
SRCDIR += src/pthread
SRCDIR += src/test
SRCDIR += src/socket
SRCDIR += src/event
#SRCDIR += src/http

TARGET_T = $(BINDIR)/$(TARGET)
include allrules.mk
