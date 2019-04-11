NAME := $(shell basename $(PWD))
export TASK := M3
all: $(NAME)-64 $(NAME)-32
run32:all
			@./sperf-32
run64:all
			@./sperf-64
run:run64 run32

include ../Makefile
include ../Makefile.lab
