// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file exlib.license for terms.

//exlib_build_use exlib inlib GL GLX X11

// should compile with : build -inlib_gl -x11_no_gl

#include <inlib/mem>

#include <inlib/mathf>
#include <inlib/colorfs>

#include <inlib/sg/separator>
#include <inlib/sg/ortho>
#include <inlib/sg/matrix>
#include <inlib/sg/color>
#include <inlib/sg/cube>

#ifdef EXLIB_NO_GL
#include <exlib/X11/base_session>
#include <exlib/X11/viewer>
#else
#include <exlib/X11/session>
#include <exlib/sg/viewer>
#endif

#include <iostream>
#include <cstdlib>

int main(int,char**) {

#ifdef INLIB_MEM
  inlib::mem::set_check_by_class(true);{
#endif

  //////////////////////////////////////////////////////////
  /// create scene graph ///////////////////////////////////
  //////////////////////////////////////////////////////////
  inlib::sg::separator* sep = new inlib::sg::separator;

  inlib::sg::ortho* camera = new inlib::sg::ortho;
  camera->position.value(inlib::vec3f(0,0,4));    
  camera->height.value(2);    
  camera->znear.value(0.1);
  camera->zfar.value(100);
  sep->add(camera);

 {inlib::sg::matrix* m = new inlib::sg::matrix;
  m->set_rotate(0,1,0,inlib::fhalf_pi()/2.0f);
  m->mul_rotate(1,0,0,inlib::fhalf_pi()/2.0f);
  sep->add(m);} //done first.

  inlib::sg::color* color = new inlib::sg::color();
  color->rgb = inlib::colorf_green();
  sep->add(color); //sg takes ownership of color.

  inlib::sg::cube* node = new inlib::sg::cube();
  node->width.value(1.0f);
  node->height.value(1.0f);
  node->depth.value(1.0f);
  sep->add(node);

  //////////////////////////////////////////////////////////
  /// create window, attach to the viewer, steer ///////////
  //////////////////////////////////////////////////////////

#ifdef EXLIB_NO_GL
  exlib::X11::base_session x11(std::cout);
#else
  exlib::X11::session x11(std::cout);
#endif  
  if(!x11.display()) return EXIT_FAILURE;

  Window win = x11.create_window("win 1",0,0,400,200);
  if(win==0L) return EXIT_FAILURE;

  //////////////////////////////////////////////////////////
  /// create the viewer, set the scene graph ///////////////
  //////////////////////////////////////////////////////////
#ifdef EXLIB_NO_GL
  exlib::X11::viewer viewer(std::cout,x11.display(),win,400,200);
#else  
  exlib::sg::viewer viewer(std::cout,400,200);
#endif  
  viewer.sg().add(sep); //give sep ownership to the viewer.
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////

  
  x11.show_window(win);

  Atom atom = ::XInternAtom(x11.display(),"WM_DELETE_WINDOW",False);

  while(true) { 
      XEvent xevent;
      ::XNextEvent(x11.display(),&xevent);
      //std::cout << xevent.type << std::endl;
      //if((xevent.type==Expose)||(xevent.type==ConfigureNotify)) {
      if(xevent.type==ClientMessage) {
        if(xevent.xclient.data.l[0]==(long)atom) break;        
      } else if(xevent.type==Expose) {
        int width,height;
        x11.window_size(win,width,height);

        viewer.set_size(width,height);

#ifdef EXLIB_NO_GL
        viewer.render();
#else
        if(::glXMakeCurrent(x11.display(),win,x11.context())==False){
          std::cout << "glXMakeCurrent failed." << std::endl;
          break;
        }
        viewer.render();
        ::glXSwapBuffers(x11.display(),win);
#endif

      } else if(xevent.type==ButtonPress && xevent.xbutton.button==1) {
        break;
      }

  }


  x11.delete_window(win);

#ifdef INLIB_MEM
  }inlib::mem::balance(std::cout);
#endif

  return EXIT_SUCCESS;
}
