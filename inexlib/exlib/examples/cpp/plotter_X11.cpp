// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file exlib.license for terms.

//exlib_build_use exlib inlib freetype inlib_glutess gl2ps GL GLX X11

#define EXLIB_SCREEN_MGR X11

#include <exlib/X11/plotter>

#include "plotter_common.icc"

#ifndef __CLING__
int main(int argc,char** argv) {return plotter_X11(argc,argv);}
//int main(int argc,char** argv) {return plotter_EXLIB_SCREEN_MGR(argc,argv);}
#endif
