// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

//  This program produces a wroot.root file.
//
//  See rroot.C for an example of how to manipulate
// (and check !) the content of this file with CERN-ROOT.

//inlib_build_use kernel

#ifndef EXLIB_DONT_HAVE_ZLIB
#define EXLIB_DONT_HAVE_ZLIB
#endif

#ifdef INLIB_MEM
#include <inlib/mem>
#endif //INLIB_MEM

#include <inlib/wroot/file>
#include <inlib/wroot/to>

#include <inlib/histo/h1d>
#include <inlib/histo/h2d>
#include <inlib/histo/h3d>
#include <inlib/histo/p1d>
#include <inlib/histo/p2d>
#include <inlib/wroot/ntuple>

#include <inlib/histo/h1df>

#include <inlib/randd>
#include <inlib/randf>

#include <inlib/rroot/file>
#include <inlib/rroot/ntuple>
#include <inlib/rroot/fac>
#include <inlib/eqT>

#ifdef EXLIB_DONT_HAVE_ZLIB
#else
#include <exlib/zlib>
#endif

#include <inlib/args>
#include <iostream>
#include <cstdlib>

int main(int argc,char** argv) {

#ifdef INLIB_MEM
  inlib::mem::set_check_by_class(true);{
#endif //INLIB_MEM
  inlib::args args(argc,argv);

  bool verbose = args.is_arg("-verbose");

  bool row_wise = args.is_arg("-row_wise");
  
  //////////////////////////////////////////////////////////
  /// create a .root file : ////////////////////////////////
  //////////////////////////////////////////////////////////
  if(verbose) std::cout << (row_wise?"row_wise":"column_wise") << std::endl;
  
  std::string file = "wroot.root";
  
 {inlib::wroot::file rfile(std::cout,file);
#ifdef EXLIB_DONT_HAVE_ZLIB
#else
  if(args.is_arg("-noz")){
  } else {
    rfile.add_ziper('Z',exlib::compress_buffer);
    rfile.set_compression(9);
  }
#endif

  bool osc_stream = args.is_arg("-osc");

  inlib::wroot::directory* dir = rfile.dir().mkdir("histo");
  if(!dir) {
    std::cout << "can't create diectory." << std::endl;
    return EXIT_FAILURE;
  }

  //inlib::wroot::directory* dxxx = dir->mkdir("xxx");
  //if(!dxxx) {
  //  std::cout << "can't create diectory." << std::endl;
  //  return EXIT_FAILURE;
  //}

  //////////////////////////////////////////////////////////
  /// create some histos : /////////////////////////////////
  //////////////////////////////////////////////////////////

  unsigned int num_megas;
  args.find<unsigned int>("-megas",num_megas,1);

  unsigned int entries = num_megas*1000000;

  inlib::rgaussd rg(1,2);
  inlib::rbwd rbw(0,1);

 {inlib::histo::h1d h("Gauss",100,-5,5);
  for(unsigned int count=0;count<entries;count++) h.fill(rg.shoot(),1.4);
  // plotting hints :
  h.add_annotation(inlib::histo::key_axis_x_title(),"rand gauss");
  h.add_annotation(inlib::histo::key_axis_y_title(),"1.4*entries");
  if(verbose) {
    std::cout << "h1d : " << h.title()
              << ", all entries " << h.all_entries()
              << ", entries " << h.entries()
              << ", mean " << h.mean() << ", rms " << h.rms()
              << std::endl;
  }
  // write :
  if(osc_stream) {
    if(!inlib::wroot::to_osc(*dir,h,"rg")) return EXIT_FAILURE;
  } else {
    if(!inlib::wroot::to(*dir,h,"rg")) return EXIT_FAILURE;
  }}

 {inlib::rgaussf rf(1,2);
  inlib::histo::h1df h("GaussF",100,-5,5);
  for(unsigned int count=0;count<entries;count++) h.fill(rf.shoot(),1.4f);
  // plotting hints :
  h.add_annotation(inlib::histo::key_axis_x_title(),"rand gauss");
  h.add_annotation(inlib::histo::key_axis_y_title(),"1.4*entries");
  if(verbose) {
    std::cout << "h1df : " << h.title()
              << ", all entries " << h.all_entries()
              << ", entries " << h.entries()
              << ", mean " << h.mean() << ", rms " << h.rms()
              << std::endl;
  }
  // write :
  if(!inlib::wroot::to(*dir,h,"rf")) return EXIT_FAILURE;}

 {inlib::histo::p1d h("Profile",100,-5,5,-2,2);
  for(unsigned int count=0;count<entries;count++) h.fill(rg.shoot(),rbw.shoot(),1);
  if(verbose) {
    std::cout << "p1d : " << h.title()
              << ", all entries " << h.all_entries()
              << ", entries " << h.entries()
              << ", mean " << h.mean() << ", rms " << h.rms()
              << std::endl;
  }
  if(osc_stream) {
    if(!inlib::wroot::to_osc(*dir,h,"prof")) return EXIT_FAILURE;
  } else {
    if(!inlib::wroot::to(*dir,h,"prof")) return EXIT_FAILURE;
  }}

 {inlib::histo::h2d h("Gauss_BW",20,-5,5,20,-2,2);
  for(unsigned int count=0;count<entries;count++) h.fill(rg.shoot(),rbw.shoot(),0.8);
  //plotting hints :
  h.add_annotation(inlib::histo::key_axis_x_title(),"rand gauss");
  h.add_annotation(inlib::histo::key_axis_y_title(),"rand bw");
  h.add_annotation(inlib::histo::key_axis_z_title(),"0.8*entries");
  if(verbose) {
    std::cout << "h2d : " << h.title()
              << ", all entries " << h.all_entries()
              << ", entries " << h.entries()
              << ", mean_x " << h.mean_x() << ", rms_x " << h.rms_x()
              << ", mean_y " << h.mean_y() << ", rms_y " << h.rms_y()
              << std::endl;
  }
  // write :
  if(osc_stream) {
    if(!inlib::wroot::to_osc(*dir,h,"rgbw")) return EXIT_FAILURE;
  } else {
    if(!inlib::wroot::to(*dir,h,"rgbw")) return EXIT_FAILURE;
  }}

 {inlib::histo::p2d h("Profile2D",100,-5,5,100,-5,5,-2,2);
  for(unsigned int count=0;count<entries;count++) h.fill(rg.shoot(),rg.shoot(),rbw.shoot(),1);
  if(verbose) {
    std::cout << "p2d : " << h.title()
              << ", all entries " << h.all_entries()
              << ", entries " << h.entries()
              << ", mean_x " << h.mean_x() << ", rms_x " << h.rms_x()
              << ", mean_y " << h.mean_y() << ", rms_y " << h.rms_y()
              << std::endl;
  }
  if(osc_stream) {
    if(!inlib::wroot::to_osc(*dir,h,"prof2D")) return EXIT_FAILURE;
  } else {
    if(!inlib::wroot::to(*dir,h,"prof2D")) return EXIT_FAILURE;
  }}

 {inlib::histo::h3d h("Gauss_Gauss_BW",20,-5,5,20,-5,5,20,-2,2);
  for(unsigned int count=0;count<entries;count++) h.fill(rg.shoot(),rg.shoot(),rbw.shoot(),0.8);
  //plotting hints :
  h.add_annotation(inlib::histo::key_axis_x_title(),"rand gauss");
  h.add_annotation(inlib::histo::key_axis_y_title(),"rand gauss");
  h.add_annotation(inlib::histo::key_axis_z_title(),"rand bw");
  if(verbose) {
    std::cout << "h3d : " << h.title()
              << ", all entries " << h.all_entries()
              << ", entries " << h.entries()
              << ", mean_x " << h.mean_x() << ", rms_x " << h.rms_x()
              << ", mean_y " << h.mean_y() << ", rms_y " << h.rms_y()
              << ", mean_z " << h.mean_z() << ", rms_z " << h.rms_z()
              << std::endl;
  }
  // write :
  if(osc_stream) {
    if(!inlib::wroot::to_osc(*dir,h,"rggbw")) return EXIT_FAILURE;
  } else {
    if(!inlib::wroot::to(*dir,h,"rggbw")) return EXIT_FAILURE;
  }}

  //////////////////////////////////////////////////////////
  /// create and fill a ntuple : ///////////////////////////
  //////////////////////////////////////////////////////////
 {//WARNING : the ntuple can't be on the stack. It is owned by the directory.
  inlib::wroot::ntuple* ntu = new inlib::wroot::ntuple(rfile.dir(),"rg_rbw","Randoms",row_wise);
  inlib::wroot::ntuple::column<int>* col_count = ntu->create_column<int>("count");
  inlib::wroot::ntuple::column<double>* col_rgauss = ntu->create_column<double>("rgauss");
  inlib::wroot::ntuple::column<float>* col_rbw = ntu->create_column<float>("rbw");
  inlib::wroot::ntuple::column_string* col_str = ntu->create_column_string("string");

  std::vector<float> user_vec_f;
  ntu->create_column_vector_ref<float>("vec_float",user_vec_f); //pass the ref of user_vec_f.
  inlib::wroot::ntuple::std_vector_column<double>* col_vec_d = ntu->create_column_vector<double>("vec_d");
  std::vector<std::string> user_vec_s;
  ntu->create_column_vector_string_ref("vec_s",user_vec_s,'\n');
  
  if(args.is_arg("-large")){
    entries = 300000000; //to test >2Gbytes file.
    ntu->set_basket_size(1000000);
  }

  inlib::rtausmed rflat;
  unsigned int vec_float_count = 0;
  unsigned int vec_double_count = 0;
  
  inlib::rbwf rbwf(0,1);
  std::string stmp;
  for(unsigned int count=0;count<entries;count++) {    
    if(!col_count->fill(count)) {
      std::cout << "col_count fill failed." << std::endl;
      break;
    }
    if(!col_rgauss->fill(rg.shoot())) {
      std::cout << "col_rgauss fill failed." << std::endl;
      break;
    }
    if(!col_rbw->fill(rbwf.shoot())) {
      std::cout << "col_rbw fill failed." << std::endl;
      break;
    }
    if(!inlib::num2s(count,stmp)){}
    if(!col_str->fill("str "+stmp)) {
      std::cout << "col_str fill failed." << std::endl;
      break;
    }

   {user_vec_f.clear();
    unsigned int number = (unsigned int)(10*rflat.shoot());
    for(unsigned int i=0;i<number;i++) {
      user_vec_f.push_back(rbwf.shoot());
    }
    vec_float_count += number;}

   {std::vector<double>& user_vec_d = col_vec_d->variable();
    user_vec_d.clear();
    unsigned int number = row_wise ? count%100 : (unsigned int)(10*rflat.shoot());
    for(unsigned int i=0;i<number;i++) {
      user_vec_d.push_back(rg.shoot());
    }
    vec_double_count += number;}

   {user_vec_s.clear();
    unsigned int number = row_wise ? count%5 : (unsigned int)(5*rflat.shoot());
    for(unsigned int i=0;i<number;i++) {
      if(!inlib::num2s(i,stmp)){}
      user_vec_s.push_back(stmp);
    }}
     
    if(!ntu->add_row()) {
      std::cout << "ntuple fill failed." << std::endl;
      break;
    }
  }
  if(verbose) {
    std::cout << "vec_float_count " << vec_float_count << std::endl;
    std::cout << "vec_double_count " << vec_double_count << std::endl;
  }}
  
  //////////////////////////////////////////////////////////
  /// create a ntuple from a ntuple_booking object. ////////
  //////////////////////////////////////////////////////////
 {inlib::ntuple_booking nbk("rg_rbw_2","Randoms");
  nbk.add_column<double>("rgauss");
  nbk.add_column<float>("rbw");
  nbk.add_column<std::string>("string");
  //nbk.add_column<bool>("not_handled");

  inlib::wroot::ntuple* ntu = new inlib::wroot::ntuple(rfile.dir(),nbk);
  if(ntu->columns().size()) {

    inlib::wroot::ntuple::column<double>* col_rgauss = ntu->find_column<double>("rgauss");
    inlib::wroot::ntuple::column<float>* col_rbw = ntu->find_column<float>("rbw");
    inlib::wroot::ntuple::column_string* col_str = ntu->find_column_string("string");

    inlib::rbwf rbwf(0,1);
    std::string stmp;
    for(unsigned int count=0;count<1000;count++) {    
      if(!col_rgauss->fill(rg.shoot())) {
        std::cout << "col_rgauss fill failed." << std::endl;
        break;
      }
      if(!col_rbw->fill(rbwf.shoot())) {
        std::cout << "col_rbw fill failed." << std::endl;
        break;
      }
      if(!inlib::num2s(count,stmp)){}
      if(!col_str->fill("str "+stmp)) {
        std::cout << "col_str fill failed." << std::endl;
        break;
      }
      if(!ntu->add_row()) {
        std::cout << "ntuple fill failed." << std::endl;
        break;
      }
    }
  }}

  //////////////////////////////////////////////////////////
  /// consistency check : create an empty ntuple : /////////
  //////////////////////////////////////////////////////////
 {inlib::wroot::ntuple* ntu = new inlib::wroot::ntuple(rfile.dir(),"empty","empty");
  ntu->create_column<int>("empty");}

  //////////////////////////////////////////////////////////
  /// write and close file : ///////////////////////////////
  //////////////////////////////////////////////////////////
 {unsigned int n;
  if(!rfile.write(n)) {
    std::cout << "file write failed." << std::endl;
  }}
  
  rfile.close();}

  //////////////////////////////////////////////////////////
  /// read the file : //////////////////////////////////////
  //////////////////////////////////////////////////////////
 
#include "read_root.icc"
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////

#ifdef INLIB_MEM
  }inlib::mem::balance(std::cout);
#endif //INLIB_MEM

  return EXIT_SUCCESS;
}
