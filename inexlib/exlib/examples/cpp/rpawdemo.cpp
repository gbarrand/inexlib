// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file exlib.license for terms.

//exlib_build_use exlib inlib csz

#include <inlib/mem>

#include <inlib/args>
#include <inlib/rroot/file>
#include <inlib/rroot/streamers>

#include <iostream>
#include <cstdlib>

int main(int argc,char** argv) {

#ifdef INLIB_MEM
  inlib::mem::set_check_by_class(true);{
#endif

  inlib::args args(argc,argv);

  bool verbose = args.is_arg("-verbose");

  std::string file = "pawdemo.root";
  inlib::rroot::file rfile(std::cout,file,verbose);

  inlib::rroot::key* key = rfile.dir().find_key("h10");
  if(!key) {
    std::cout << "key for h10 not found." << std::endl;
    return EXIT_FAILURE;
  }
  unsigned int sz;
  char* buf = key->get_object_buffer(rfile,sz);
  if(!buf) {
    std::cout << "can't get data buffer for h10." << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "size of h10 : " << sz << std::endl;

  inlib::rroot::buffer b(std::cout,rfile.byte_swap(),sz,buf,key->key_length(),verbose);
  inlib::histo::h1d* h = inlib::rroot::TH1F_stream(b); //we get ownership on h.
  if(!h) {
    std::cout << "streaming failed for h10." << std::endl;
    return EXIT_FAILURE;
  }

  h->hprint(std::cout);

  delete h;
  
#ifdef INLIB_MEM
  }inlib::mem::balance(std::cout);
#endif

  return EXIT_SUCCESS;
}
