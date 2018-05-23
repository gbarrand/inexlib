// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file exlib.license for terms.

//exlib_build_use exlib inlib inlib_glutess freetype GL GLX X11

#include <inlib/mem>

#include <inlib/sg/separator>
#include <inlib/sg/ortho>
#include <inlib/sg/matrix>
#include <inlib/sg/color>
#include <inlib/sg/text_button>

#include <exlib/sg/text_freetype>
#include <exlib/sg/viewer>
#include <exlib/X11/session>

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
  camera->znear.value(0.1f);
  camera->zfar.value(100);
  sep->add(camera);

  inlib::sg::color* color = new inlib::sg::color();
  color->rgb = inlib::colorf_green();
  sep->add(color); //sg takes ownership of color.

  exlib::sg::text_freetype ttf;

 {inlib::sg::text_button* b = new inlib::sg::text_button(ttf);
  b->width = 3;
  b->height = 1;
  b->font = inlib::sg::font_arialbd_ttf();
  b->front_face = inlib::sg::winding_cw;

  b->back_area::color = inlib::colorf_orange();
  b->color = inlib::colorf_black();
  b->arm_color = inlib::colorf_yellow();

  b->strings.add("click me !");
  b->confine = true;

  class cbk : public inlib::sg::bcbk {
    typedef inlib::sg::bcbk parent;
  public:
    virtual inlib::sg::return_action action() {
      std::cout << "hello inlib::sg::text_button callback." << std::endl;
      return inlib::sg::return_none;     
    }
    virtual inlib::sg::bcbk* copy() const {return new cbk(*this);}
  public:
    cbk():parent(){}
    virtual ~cbk(){}
  private:
    cbk(const cbk& a_from):parent(a_from)/*,m_gv(a_from.m_gv)*/{}
    cbk& operator=(const cbk& a_from){
      parent::operator=(a_from);
      return *this;
    }
  };

  b->add_callback(new cbk());
  sep->add(b);}

  //////////////////////////////////////////////////////////
  /// create the viewer, set the scene graph ///////////////
  //////////////////////////////////////////////////////////
  unsigned int ww = 400;
  unsigned int wh = 200;
  
  exlib::sg::viewer viewer(std::cout,ww,wh);
  viewer.sg().add(sep); //give sep ownership to the viewer.

  //////////////////////////////////////////////////////////
  /// create window, attach to the viewer, steer ///////////
  //////////////////////////////////////////////////////////

  exlib::X11::session x11(std::cout);
  if(!x11.display()) return EXIT_FAILURE;

  Window win = x11.create_window("win 1",0,0,ww,wh);
  if(win==0L) return EXIT_FAILURE;
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

        if(::glXMakeCurrent(x11.display(),win,x11.context())==False){
          std::cout << "glXMakeCurrent failed." << std::endl;
          break;
        }

        viewer.render();

        ::glXSwapBuffers(x11.display(),win);
        ::glXMakeCurrent(x11.display(),None,NULL);

      } else if(xevent.type==ButtonPress && xevent.xbutton.button==1) {

        int width,height;
        x11.window_size(win,width,height);

        int x = xevent.xbutton.x;
        int y = height-xevent.xbutton.y;

        float hsize = 2;
        float l = x-hsize; //could be negative.
        float r = x+hsize;
        float b = y-hsize; //could be negative.
        float t = y+hsize;
        inlib::sg::pick_action action(std::cout,width,height,l,r,b,t);
        action.set_stop_at_first(true);

        sep->pick(action); //can trigger button callback.
        if(!action.end()) {
	  std::cout << "main :" 
                    << " bad pick_action end."
                    << std::endl;
          return EXIT_FAILURE;
        }

        //execute callbacks of picked button :
        inlib::sg::node* node = action.node();
        if(node) {
          if(inlib::sg::base_button* btn = 
             inlib::safe_cast<inlib::sg::node,inlib::sg::base_button>(*node)){
            const std::vector<inlib::sg::bcbk*>& cbks = btn->cbks().callbacks();
            std::vector<inlib::sg::bcbk*>::const_iterator it;
            for(it=cbks.begin();it!=cbks.end();++it) (*it)->action();
          }
        }

      }

  }


  x11.delete_window(win);

#ifdef INLIB_MEM
  }inlib::mem::balance(std::cout);
#endif

  return EXIT_SUCCESS;
}
