CXX=g++ -m64
CXXFLAGS=-Iobjs/ -O3 -Wall -mavx2
ISPC=ispc
ISPCFLAGS=-O2 --target=avx1-i32x8 --arch=x86-64

APP_NAME=LevenshteinDistance_ispc

default: $(APP_NAME)

.PHONY: dirs clean

dirs:/bin/mkdir -p $(OBJDIR)/


