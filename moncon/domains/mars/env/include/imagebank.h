#ifndef DOM_MARS_IMAGEBANK
#define DOM_MARS_IMAGEBANK

#include "substrate/basic_observables.h"
#include "substrate/goalspec.h"

#include <stdio.h>

using namespace MonCon;

namespace mcMars {

typedef unsigned int imagetype;

// image is unsigned int:
// bits  0- 7 source
// bits  8-15 target
// bits 16-18 series ID
// bits 16-29 unused
// bit  30    panoramic
// bit  31    valid/junk

enum img_bits { _IMG_PANO_BIT=30,_IMG_INVALID_BIT };

inline imagetype create_img(int src,int trg) {
  return (src & 0xFF) | ((trg & 0xFF) << 8);
}

inline imagetype create_img(int src,int trg,int series) {
  return (src & 0xFF) | ((trg & 0xFF) << 8) | ((series & 0x07) << 16);
}

inline imagetype create_empty_img() { return 1 << _IMG_INVALID_BIT; }
inline bool is_empty_img(imagetype img) 
{ return img & (1 << _IMG_INVALID_BIT); }
inline int img_src(imagetype img) { return img & 0xFF; }
inline int img_trg(imagetype img) { return (img >> 8)  & 0xFF; }
inline int img_serial(imagetype img) { return (img >> 16) & 0x07; }
inline void img_set_serial(imagetype& img,int serial) 
  { img &= !(0x07 << 16); img |= ((serial & 0x07) << 16); };
inline void img_set_pano(imagetype& img) { img |= (1 << _IMG_PANO_BIT); };
inline void img_unset_pano(imagetype& img) { img &= !(1 << _IMG_PANO_BIT); };
inline bool img_is_pano(imagetype img) { return (img & (1 << _IMG_PANO_BIT)); };
inline bool img_is_junk(imagetype img) { return (img & (1 << _IMG_INVALID_BIT)); };
inline bool img_is_match(imagetype img,goalspec gs) { return (img_trg(img) == (int)gs_arg(gs)); };

inline void describe_imagetype(imagetype poo) {
  printf("img(s=%d,t=%d,#=%d,%c,%c)",
	 img_src(poo), img_trg(poo), img_serial(poo),
	 (img_is_pano(poo) ? 'p' : '-'),
	 (img_is_junk(poo) ? 'X' : '-'));
}

// bank class

class image_bank {
 public:
  image_bank(int size) : banksize(size),imptr(0) { 
    images = new imagetype[size]; 
  };
  ~image_bank() { delete[] images; };

  bool add_image(imagetype img) { 
    cout << "image added[" << hex << img << "] ";
    describe_imagetype(img);
    cout  << endl;
    if (imptr < banksize) {
      images[imptr] = img; 
      imptr++; 
      return true;
    }
    else return false;
  };

  void clear() { 
    imptr = 0; 
    for (int i=0;i<banksize;i++) 
      images[i]=create_empty_img();
  };

  imagetype get_image(int ptr) { 
    if (imptr > ptr)
      return images[ptr];
    else return create_empty_img();
  };

  imagetype get_image() { 
    if (imptr > 0)
      return images[imptr-1];
    else return create_empty_img();
  };
  
  imagetype pop_image() { 
    if (imptr > 0) {
      imagetype rv = images[imptr-1];
      images[imptr-1] = create_empty_img();
      imptr--;
      return rv;
    }
    else return create_empty_img();
  };

  int get_remaining_capacity() { return banksize - imptr; };
  int get_banksize() { return banksize; };
  int get_numimgs() { return imptr; };
  
 private:
  unsigned int* images;
  int banksize,imptr;

};

class ib_capacity_sensor : public basic_observable_resource {
 public:
  ib_capacity_sensor(string name,image_bank& ib,sensorimotor_agent* sma) : 
    basic_observable_resource(name,sma,0.0),bank_of(ib) { };

  virtual double double_value() { return bank_of.get_remaining_capacity(); };
  virtual int    int_value()  { return (int)double_value(); };

  virtual string class_name() { return "ib_capacity_sensor"; };

 private:
  image_bank& bank_of;

};

};

#endif

