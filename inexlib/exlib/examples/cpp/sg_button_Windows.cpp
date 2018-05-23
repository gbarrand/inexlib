// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file exlib.license for terms.

//exlib_build_use exlib inlib inlib_glutess freetype GL Windows kernel

#include <inlib/mem>

#include <inlib/sg/separator>
#include <inlib/sg/ortho>
#include <inlib/sg/matrix>
#include <inlib/sg/color>
#include <inlib/sg/text_button>

#include <exlib/sg/text_freetype>

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include <exlib/Windows/session>
#include <exlib/Windows/window>

#ifdef EXLIB_NO_GL
#include <exlib/Windows/viewer>
#else //!EXLIB_NO_GL
#include <exlib/sg/viewer>

#include <exlib/Windows/glarea>

class glarea : public exlib::Windows::glarea {
public:
  virtual void paint(unsigned int a_w,unsigned int a_h) {
    m_viewer.set_size(a_w,a_h);
    m_viewer.render();
  }
  virtual void left_button_down(unsigned int a_x,unsigned int a_y) {
    int width = m_viewer.width();
    int height = m_viewer.height();

    int x = a_x;
    int y = a_y;

    float hsize = 2;
    float l = x-hsize; //could be negative.
    float r = x+hsize;
    float b = y-hsize; //could be negative.
    float t = y+hsize;
    inlib::sg::pick_action action(m_viewer.out(),width,height,l,r,b,t);
    action.set_stop_at_first(true);

    m_viewer.sg().pick(action); //can trigger button callback.
    if(!action.end()) return;

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
public:
  glarea(HWND a_parent,exlib::sg::viewer& a_viewer)
  :exlib::Windows::glarea(a_parent)
  ,m_viewer(a_viewer)
  {}
private:
  exlib::sg::viewer& m_viewer;
};
#endif //EXLIB_NO_GL
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

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
    cbk(const cbk& a_from)
    :parent(a_from)/*,m_gv(a_from.m_gv)*/{}
    cbk& operator=(const cbk& a_from){
      parent::operator=(a_from);
      return *this;
    }
  };

  b->add_callback(new cbk());
  sep->add(b);}

  //////////////////////////////////////////////////////////
  /// create window, attach to the viewer, steer ///////////
  //////////////////////////////////////////////////////////

  exlib::Windows::session wdoz(std::cout);

  //////////////////////////////////////////////////////////
  /// create the viewer, set the scene graph ///////////////
  //////////////////////////////////////////////////////////
  unsigned int ww = 400;
  unsigned int wh = 200;
  
#ifdef EXLIB_NO_GL
  exlib::Windows::viewer viewer(std::cout,ww,wh);
  HWND hwnd = viewer.hwnd();
  if(!hwnd) return EXIT_FAILURE;
#else
  exlib::Windows::window win(ww,wh);
  win.set_client_area_size(ww,wh);
  HWND hwnd = win.hwnd();
  if(!hwnd) return EXIT_FAILURE;
  exlib::sg::viewer viewer(std::cout,ww,wh);
  glarea gla(hwnd,viewer);
  if(!gla.hwnd()) return EXIT_FAILURE;
#endif 
  viewer.sg().add(sep); //give sep ownership to the viewer.

  wdoz.show_window(hwnd);

  wdoz.steer();

#ifdef INLIB_MEM
  }inlib::mem::balance(std::cout);
  std::cout << "exit(mem)..." << std::endl;
#endif
  
  return EXIT_SUCCESS;
}
