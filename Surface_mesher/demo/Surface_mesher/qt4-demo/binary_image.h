// Copyright (c) 2005-2008  INRIA Sophia-Antipolis (France).
//               2008 GeometryFactory
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Author(s)     : Laurent RINEAU, Pierre ALLIEZ

#ifndef BINARY_IMAGE_3
#define BINARY_IMAGE_3

#include <CGAL/basic.h>

#include <boost/shared_ptr.hpp>

#include <boost/format.hpp>
#include <CGAL/ImageIO.h>

#include <limits>

#include <CGAL/Image_3.h>

template <typename FT_, typename Point>
class CBinary_image_3 : public CGAL::Image_3
{
  bool interpolate_;

public:
  float min_value;
  float max_value;

  typedef FT_ FT;

public:
  CBinary_image_3() : Image_3(), interpolate_(true)
  {
  }

  CBinary_image_3(const CBinary_image_3& bi)
    : Image_3(bi), interpolate_(bi.interpolate_)
  {
    std::cerr << "CBinary_image_3::copy_constructor\n";
    min_value = bi.min_value;
    max_value = bi.max_value;
  }

  ~CBinary_image_3()
  {
  }

  void compute_min_max()
  {
    if(image() == 0) {
      min_value = 0;
      max_value = 0;
      return;
    }
    min_value = max_value = evaluate(image(),0,0,0);
    for(unsigned int i = 0; i < xdim(); ++i) {
      for(unsigned int j = 0; j < ydim(); ++j)
        for(unsigned int k = 0; k < zdim(); ++k) {
          const float v = evaluate(image(), i, j, k);
          if(v > max_value) max_value = v;
          if(v < min_value) min_value = v;
        }
    }
  }

  float xmax() const
  {
    return (float)(((image_ptr->xdim) - 1.0)*(image_ptr->vx));
  }

  float ymax() const
  {
    return (float)(((image_ptr->ydim) - 1.0)*(image_ptr->vy));
  }

  float zmax() const
  {
    return (float)(((image_ptr->zdim) - 1.0)*(image_ptr->vz));
  }

  Point center() 
  {
    FT cx = 0.5 * xmax();
    FT cy = 0.5 * ymax();
    FT cz = 0.5 * zmax();
    return Point(cx,cy,cz);
  }

  FT radius()
  {
    return (std::max)((std::max)(xmax(),ymax()),zmax());
  }

  Point point(const unsigned int i,
              const unsigned int j,
              const unsigned int k) const
  {
    return Point(i * (image_ptr->vx),
                 j * (image_ptr->vy),
                 k * (image_ptr->vz));
  }

public:

  unsigned int threshold(const unsigned char value,
                         const unsigned char equal,
                         const unsigned char diff)
  {
    if(image_ptr.get() == NULL)
      return 0;

    unsigned int nb = 0;
    unsigned char *pData = (unsigned char*)image_ptr->data;
    unsigned int xy = image_ptr->xdim * image_ptr->ydim;
    for(unsigned int i=0;i<image_ptr->xdim;i++)
      for(unsigned int j=0;j<image_ptr->ydim;j++)
        for(unsigned int k=0;k<image_ptr->zdim;k++)
        {
          unsigned char voxel = pData[xy*k + j*image_ptr->xdim + i];
          if(voxel == value)
          {
            pData[xy*k + j*image_ptr->xdim + i] = equal;
            nb++;
          }
          else
            pData[xy*k + j*image_ptr->xdim + i] = diff;
        }
    return nb;
  }
  bool inside(const float x,
              const float y, 
              const float z) const
  {
    return ( x >= 0.0f && 
             y >= 0.0f && 
             z >= 0.0f && 
             x <= xmax() &&
             y <= ymax() &&
             z <= zmax() );
  }

  float rand_x() { return (float)rand() / (float)RAND_MAX * xmax(); }
  float rand_y() { return (float)rand() / (float)RAND_MAX * ymax(); }
  float rand_z() { return (float)rand() / (float)RAND_MAX * zmax(); }

  void set_interpolation(const bool b)
  {
    interpolate_ = b;
  }

  bool interpolation() const {
    return interpolate_;
  }

  FT operator()(Point p) const
  {
    const float x = static_cast<float>(CGAL::to_double(p.x()));
    const float y = static_cast<float>(CGAL::to_double(p.y()));
    const float z = static_cast<float>(CGAL::to_double(p.z()));
      
    if(interpolation()) {
      std::cerr << "interpolation\n";
      if(inside(x,y,z))
	return FT(::trilinear_interpolation(image_ptr.get(),x,y,z));
      else
	return 0;
    }
    else {
      const int i = static_cast<int>(x/image()->vx + 0.5f);
      const int j = static_cast<int>(y/image()->vy + 0.5f);
      const int k = static_cast<int>(z/image()->vz + 0.5f);
      if( i < 0 ||
	  j < 0 ||
	  k < 0 )
      {
	return 0;
      }
      else
      {    
	const unsigned int ui = static_cast<unsigned int>(i);
	const unsigned int uj = static_cast<unsigned int>(j);
	const unsigned int uk = static_cast<unsigned int>(k);
	if( ui >= image()->xdim ||
	    uj >= image()->ydim ||
	    uk >= image()->zdim )
	{
	  return 0;
	}
	else
	{
	  return this->value(ui, uj, uk);
	}
      }
    }
  }
}; // end CBinary_image_3
 
#endif // BINARY_IMAGE_3
