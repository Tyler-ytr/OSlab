NAME := $(shell basename $(PWD))
export TASK := M5
all: $(NAME)-64 $(NAME)-32
run32:all
			@./frecov-32
run64:all
			@./frecov-64
run:run64 run32
include ../Makefile
include ../Makefile.lab
