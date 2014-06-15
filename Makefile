
# Copyright 2010 Google Inc.
# Author: Markus Gutschke
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

VERSION := 1.0

.SUFFIXES: .so

ifeq ($(origin CC), default)
  CC := gcc
endif

DEF_CFLAGS := $(shell [ `uname` = SunOS ] &&                                  \
                echo ' -D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT')              \
              -fvisibility=hidden $(CFLAGS)
DEF_LDFLAGS := $(shell [ `uname` = SunOS ] && echo ' -mimpure-text') $(LDFLAGS)
LDL_LDFLAGS := -ldl

all: qrutf8

install: all
	/bin/true

clean:
	$(RM) *.o *.so core qrutf8

qrutf8: qrutf8.o
	$(CC) -g $(DEF_LDFLAGS) -o $@ $+ $(LDL_LDFLAGS)

qrutf8.o: qrutf8.c

.c.o:
	$(CC) --std=gnu99 -Wall -O2 -g -fPIC -c $(DEF_CFLAGS) -o $@ $<
.o.so:
	$(CC) -shared -g $(DEF_LDFLAGS) -o $@ $+
