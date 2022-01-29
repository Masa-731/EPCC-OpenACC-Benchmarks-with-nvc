# Copyright (c) 2013 The University of Edinburgh.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Makefile for OpenACC Benchmarks

# default compiler: nvc
# nvc's `-fast` option seems to break some benchmarks for some reason, so it is not recommended for now (version 22.1).
ifneq ($(COMP), gcc)
CC = nvc
CFLAGS = -acc -mp=gpu -Minfo=accel -mcmodel=medium -gpu=cc86 -tp=host -g -lm
LFLAGS =
else
# gcc currently has some syntax problems and fails to compile (version 11.0), with reasons unknown.
CC = gcc
CFLAGS = -march=native -fopenacc -O2 -mcmodel=medium -g -lm
LFLAGS =
endif

objects = common.o main.o level0.o level1.o 27stencil.o le_core.o himeno.o

default: oa

%.o : %.c
	$(CC) $(CFLAGS) -c $<

oa : $(objects)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)
	mv $(objects) ../bin

.PHONY: clean

clean:
	rm -f *.o oa __hmpp* *.lst *.cub *.ptx