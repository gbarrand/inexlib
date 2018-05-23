// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file exlib.license for terms.

//exlib_build_use exlib inlib csz zlib

#ifdef INLIB_MEM
#include <inlib/mem>
#endif //INLIB_MEM

#include <inlib/args>
#include <inlib/fileis>

#include <inlib/rroot/file>
#include <inlib/rroot/rall>

#include <inlib/rroot/ntuple>
#include <inlib/ntuple_binding>

#ifdef EXLIB_DONT_HAVE_ZLIB
#else
#include <exlib/zlib>
#endif

#include <iostream>
#include <cstdlib>

int main(int argc,char** argv) {

#ifdef INLIB_MEM
  inlib::mem::set_check_by_class(true);{
#endif //INLIB_MEM

  inlib::args args(argc,argv);

  std::string file;
  if(!args.file(file)) {
    std::cout << "give a root file." << std::endl;
    return EXIT_FAILURE;
  }

  bool verbose = args.is_arg("-verbose");
  bool ls = args.is_arg("-ls");
  bool dump = args.is_arg("-dump");

 {bool is;
  inlib::file::is_root(file,is);
  if(!is) {
    std::cout << " file is not a root file." << std::endl;
    return EXIT_FAILURE;
  }}

  inlib::rroot::file rfile(std::cout,file,verbose);
#ifdef EXLIB_DONT_HAVE_ZLIB
#else
  rfile.add_unziper('Z',exlib::decompress_buffer);
#endif

  if(ls) {
    std::cout << "format version " << rfile.version() << std::endl;
  }
      
  const std::vector<inlib::rroot::key*>& keys = rfile.dir().keys();
  inlib::rroot::read(std::cout,rfile,keys,true,ls,dump,0);

  ///////////////////////////////////////////////////////////////
  /// if reading the wroot.root produced with wroot.cpp : ///////
  ///////////////////////////////////////////////////////////////
 {inlib::rroot::TDirectory* dir = inlib::rroot::find_dir(rfile.dir(),"histo");
  if(dir) {
   {inlib::rroot::key* key = dir->find_key("rg");
    if(key) {
      inlib::histo::h1d* h = inlib::rroot::key_to_h1d(rfile,*key);
      if(h) {
        std::cout << "h1d : " << h->title()
                  << ", all_entries " << h->all_entries()
                  << ", entries " << h->entries()
                  << ", mean " << h->mean() << ", rms " << h->rms()
                  << std::endl;
        delete h;
      }
    }}
   {inlib::rroot::key* key = dir->find_key("rf");
    if(key) {
      inlib::histo::h1d* h = inlib::rroot::key_to_h1d(rfile,*key);
      if(h) {
        std::cout << "h1d : " << h->title()
                  << ", all_entries " << h->all_entries()
                  << ", entries " << h->entries()
                  << ", mean " << h->mean() << ", rms " << h->rms()
                  << std::endl;
        delete h;
      }
    }}
   {inlib::rroot::key* key = dir->find_key("rgbw");
    if(key) {
      inlib::histo::h2d* h = inlib::rroot::key_to_h2d(rfile,*key);
      if(h) {
        std::cout << "h2d : " << h->title()
                  << ", all_entries " << h->all_entries()
                  << ", entries " << h->entries()
                  << ", mean_x " << h->mean_x() << ", rms_x " << h->rms_x()
                  << ", mean_y " << h->mean_y() << ", rms_y " << h->rms_y()
                  << std::endl;
        delete h;
      }
    }}
   {inlib::rroot::key* key = dir->find_key("prof");
    if(key) {
      inlib::histo::p1d* h = inlib::rroot::key_to_p1d(rfile,*key);
      if(h) {
        std::cout << "p1d : " << h->title()
                  << ", all_entries " << h->all_entries()
                  << ", entries " << h->entries()
                  << ", mean " << h->mean() << ", rms " << h->rms()
                  << std::endl;
        delete h;
      }
    }}
   {inlib::rroot::key* key = dir->find_key("prof2D");
    if(key) {
      inlib::histo::p2d* h = inlib::rroot::key_to_p2d(rfile,*key);
      if(h) {
        std::cout << "p2d : " << h->title()
                  << ", all_entries " << h->all_entries()
                  << ", entries " << h->entries()
                  << ", mean_x " << h->mean_x() << ", rms_x " << h->rms_x()
                  << ", mean_y " << h->mean_y() << ", rms_y " << h->rms_y()
                  << std::endl;
        delete h;
      }
    }}
   {inlib::rroot::key* key = dir->find_key("rggbw");
    if(key) {
      inlib::histo::h3d* h = inlib::rroot::key_to_h3d(rfile,*key);
      if(h) {
        std::cout << "h3d : " << h->title()
                  << ", all_entries " << h->all_entries()
                  << ", entries " << h->entries()
                  << ", mean_x " << h->mean_x() << ", rms_x " << h->rms_x()
                  << ", mean_y " << h->mean_y() << ", rms_y " << h->rms_y()
                  << ", mean_z " << h->mean_z() << ", rms_z " << h->rms_z()
                  << std::endl;
        delete h;
      }
    }}
    delete dir; 
  }}
 
  ////////////////////////////////////////////////////////////////////////////////////////
  // read an ntuple from inlib/examples/cpp/wroot.cpp, wroot_pntuple.cpp, pwroot.cpp : ///
  ////////////////////////////////////////////////////////////////////////////////////////
 {inlib::rroot::key* key = rfile.dir().find_key("rg_rbw");
  if(key) {
    unsigned int sz;
    char* buf = key->get_object_buffer(rfile,sz);
    if(!buf) {
      std::cout << "can't get data buffer for ntuple." << std::endl;
      return EXIT_FAILURE;
    }
    inlib::rroot::buffer b(std::cout,rfile.byte_swap(),sz,buf,key->key_length(),verbose);
    inlib::rroot::fac fac(std::cout);
    inlib::rroot::tree tree(rfile,fac);
    if(!tree.stream(b)) {
      std::cout << "TTree streaming failed." << std::endl;
      return EXIT_FAILURE;
    }
    tree.dump(std::cout,"","  ");
    //inlib::uint64 entries = tree.entries();
   {for(inlib::uint32 i=0;i<5;i++){
      if(!tree.show(std::cout,i)) {
        std::cout << "show failed for entry " << i << std::endl;
        return EXIT_FAILURE;
      }
    }}
   {inlib::uint64 entries = tree.entries();  
    for(inlib::uint64 i=inlib::mx<inlib::int64>(5,entries-5);i<entries;i++){
      if(!tree.show(std::cout,(inlib::uint32)i)) {
        std::cout << "show failed for entry " << i << std::endl;
        return EXIT_FAILURE;
      }
    }}

    // read with the flat ntuple API :
   {inlib::rroot::ntuple ntu(tree); //use the flat ntuple API.
    double user_rgauss;
    std::string user_string;
    
    inlib::ntuple_binding nbd;
    nbd.add_column("rgauss",user_rgauss);
    nbd.add_column("string",user_string);
    if(!ntu.initialize(std::cout,nbd)) {
      std::cout << "can't initialize ntuple with ntuple_binding." << std::endl;
      return EXIT_FAILURE;
    }
    inlib::histo::h1d hg("rgauss",100,-5,5);
    ntu.start();
    unsigned int count = 0;
    while(ntu.next()){
      if(!ntu.get_row()) {
        std::cout << "get_row() failed." << std::endl;
        return EXIT_FAILURE;
      }
      hg.fill(user_rgauss);
      if(count<5) std::cout << "user_string " << user_string << std::endl;
      count++;
    }
    std::cout << "ntuple_binding(rgauss) : " << hg.mean() << " " << hg.rms() << std::endl;}

  }}

  ///////////////////////////////////////////////////////////////
  /// if reading the pawdemo.root : /////////////////////////////
  ///////////////////////////////////////////////////////////////
 {inlib::rroot::key* key = rfile.dir().find_key("h10");
  if(key) {
    inlib::histo::h1d* h = inlib::rroot::key_to_h1d(rfile,*key);
    if(h) {
      std::cout << "h1d : h10"
                << ", all_entries " << h->all_entries()
                << ", entries " << h->entries()
                << ", mean " << h->mean() << ", rms " << h->rms()
                << std::endl;
      delete h;
    }
  }}

  /////////////////////////////////////////////////////////////////////
  /// if reading the prof.root produced with croot_TProfile.cpp : /////
  /////////////////////////////////////////////////////////////////////
 {inlib::rroot::key* key = rfile.dir().find_key("prof");
  if(key) {
    inlib::histo::p1d* h = inlib::rroot::key_to_p1d(rfile,*key);
    if(h) {
      std::cout << "p1d : prof"
                << ", all_entries " << h->all_entries()
                << ", entries " << h->entries()
                << ", mean " << h->mean() << ", rms " << h->rms()
                << std::endl;
      delete h;
    }
  }}
 {inlib::rroot::key* key = rfile.dir().find_key("prof2D");
  if(key) {
    inlib::histo::p2d* h = inlib::rroot::key_to_p2d(rfile,*key);
    if(h) {
      std::cout << "p2d : prof"
                << ", all_entries " << h->all_entries()
                << ", entries " << h->entries()
                << ", mean_x " << h->mean_x() << ", rms_x " << h->rms_x()
                << ", mean_y " << h->mean_y() << ", rms_y " << h->rms_y()
                << std::endl;
      delete h;
    }
  }}

#ifdef INLIB_MEM
  }inlib::mem::balance(std::cout);
#endif //INLIB_MEM

  return EXIT_SUCCESS;
}
