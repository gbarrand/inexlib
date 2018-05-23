// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file exlib.license for terms.

//exlib_build_use exlib inlib freetype inlib_glutess gl2ps GL Windows kernel

#define EXLIB_SCREEN_MGR Windows

#include <exlib/Windows/plotter>

#include "plotter_common.icc"

#ifndef __CLING__
int main(int argc,char** argv) {return plotter_Windows(argc,argv);}
#endif
