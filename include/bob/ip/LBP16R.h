/**
 * @file bob/ip/LBP16R.h
 * @date Mon Feb 27 18:25:47 CET 2012
 * @author Ivana Chingovska <ivana.chingovska@idiap.ch>
 *
 * This file defines a function to compute the LBP16R.
 *
 * Copyright (C) 2011-2013 Idiap Research Institute, Martigny, Switzerland
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BOB_IP_LBP16R_H
#define BOB_IP_LBP16R_H

#include <math.h>
#include <blitz/array.h>
#include <algorithm>
#include "bob/core/array_assert.h"
#include "bob/core/cast.h"
#include "bob/ip/Exception.h"
#include "bob/ip/LBP.h"
#include "bob/sp/interpolate.h"

namespace bob { namespace ip {

  /**
   * This class allows to extract Local Binary Pattern-like features
   * based on 16 neighbour pixels. For more information, please refer to the
   * following article: "Face Recognition with Local Binary Patterns", from T.
   * Ahonen, A. Hadid and M. Pietikainen in the proceedings of the European
   * Conference on Computer Vision (ECCV'2004), p. 469-481
   */
  class LBP16R: public LBP {

    public: //api

      /**
       * Complete constructor
       */
      LBP16R(const double R=1., 
          const bool circular=true, 
          const bool to_average=false, 
          const bool add_average_bit=false, 
          const bool uniform=false, 
          const bool rotation_invariant=false, 
          const int eLBP_type=0);

      /**
       * Constructor with two radius (not optional). This will permit Eliptical and Retangular navigation. 
        This second radio will work in the X coordinate
       */
      LBP16R(const double R,
          const double R2, 
          const bool circular=true, 
          const bool to_average=false, 
          const bool add_average_bit=false, 
          const bool uniform=false, 
          const bool rotation_invariant=false, 
          const int eLBP_type=0);


      /**
       * Copy constructor
       */
      LBP16R(const LBP16R& other);

      /**
       * Destructor
       */
      virtual ~LBP16R();

      /**
       * Assignment
       */
      LBP16R& operator= (const LBP16R& other);

      /**
       * Clone self into a boost::shared_ptr<LBP>
       */
      virtual boost::shared_ptr<LBP> clone() const;

      /**
       * Return the maximum number of labels for the current LBP variant
       */
      virtual int getMaxLabel() const;

      /**
       * Extract LBP features from a 2D blitz::Array, and save the resulting
       * LBP codes in the dst 2D blitz::Array.
       */
      template <typename T> 
        void operator()(const blitz::Array<T,2>& src, 
            blitz::Array<uint16_t,2>& dst) const;
      void 
        operator()(const blitz::Array<uint8_t,2>& src, 
            blitz::Array<uint16_t,2>& dst) const 
        { operator()<uint8_t>(src, dst); }
      void 
        operator()(const blitz::Array<uint16_t,2>& src, 
            blitz::Array<uint16_t,2>& dst) const 
        { operator()<uint16_t>(src, dst); }
      void 
        operator()(const blitz::Array<double,2>& src, 
            blitz::Array<uint16_t,2>& dst) const 
        { operator()<double>(src, dst); }

      /**
       * Extract the LBP code of a 2D blitz::Array at the given location, and
       * return it.
       */
      template <typename T> 
        uint16_t operator()(const blitz::Array<T,2>& src, int y, int x) const;
      uint16_t 
        operator()(const blitz::Array<uint8_t,2>& src, int y, int x) const 
        { return operator()<uint8_t>( src, y, x); }
      uint16_t 
        operator()(const blitz::Array<uint16_t,2>& src, int y, int x) const 
        { return operator()<uint16_t>( src, y, x); }
      uint16_t 
        operator()(const blitz::Array<double,2>& src, int y, int x) const 
        { return operator()<double>( src, y, x); }

      /**
       * Get the required shape of the dst output blitz array, before calling
       * the operator() method.
       */
      template <typename T>
        const blitz::TinyVector<int,2> 
        getLBPShape(const blitz::Array<T,2>& src) const;
      const blitz::TinyVector<int,2> 
        getLBPShape(const blitz::Array<uint8_t,2>& src) const
        { return getLBPShape<uint8_t>(src); }
      const blitz::TinyVector<int,2> 
        getLBPShape(const blitz::Array<uint16_t,2>& src) const
        { return getLBPShape<uint16_t>(src); }
      const blitz::TinyVector<int,2> 
        getLBPShape(const blitz::Array<double,2>& src) const
        { return getLBPShape<double>(src); }

    private:
      /**
       * Extract the LBP code of a 2D blitz::Array at the given location, and
       * return it, without performing any check.
       */
      template <typename T, bool circular>
        uint16_t processNoCheck(const blitz::Array<T,2>& src, int y, int x) 
        const;

      /**
       * Initialize the conversion table for rotation invariant and uniform LBP
       * patterns
       */
      virtual void init_lut_RI();
      virtual void init_lut_U2();
      virtual void init_lut_U2RI();
      virtual void init_lut_add_average_bit();
      virtual void init_lut_normal();
  };

  template <typename T>
    void bob::ip::LBP16R::operator()(const blitz::Array<T,2>& src,  
        blitz::Array<uint16_t,2>& dst) const
    {
      bob::core::array::assertZeroBase(src);
      bob::core::array::assertZeroBase(dst);
      bob::core::array::assertSameShape(dst, getLBPShape(src) );
      if( m_circular)
      {
        for(int y=0; y<dst.extent(0); ++y)
          for(int x=0; x<dst.extent(1); ++x)
            dst(y,x) = 
              bob::ip::LBP16R::processNoCheck<T,true>(src,
                  static_cast<int>(ceil(m_R))+y, static_cast<int>(ceil(m_R2))+x);
      }
      else //no possibility for non-circular LBP16
      {
        throw bob::ip::Exception();
      }
    }

  template <typename T> 
    uint16_t bob::ip::LBP16R::operator()(const blitz::Array<T,2>& src, 
        int yc, int xc) const
    {
      bob::core::array::assertZeroBase(src);
      if( m_circular)
      {
        if( yc<ceil(m_R) )
          throw ParamOutOfBoundaryError("yc", false, yc, ceil(m_R));
        if( yc>=src.extent(0)-ceil(m_R) )
          throw ParamOutOfBoundaryError("yc", true, yc, src.extent(0)-ceil(m_R)-1);
        if( xc<ceil(m_R2) )
          throw ParamOutOfBoundaryError("xc", false, xc, ceil(m_R2));
        if( xc>=src.extent(1)-ceil(m_R2) )
          throw ParamOutOfBoundaryError("xc", true, xc, src.extent(1)-ceil(m_R2)-1);
        return bob::ip::LBP16R::processNoCheck<T,true>( src, yc, xc);
      }
      else // there is no possibility for non-circular LBP16
      {
        throw bob::ip::Exception();
      }
    }

  template <typename T, bool circular> 
    uint16_t bob::ip::LBP16R::processNoCheck( const blitz::Array<T,2>& src,
        int yc, int xc) const
    {
      double tab[16];
      if(circular)
      {
        const double PI = 4.0*atan(1.0);
        const double alpha = 2 * PI / 16; // the angle between the points ont he circle of radius m_R

        /*A real circle*/
        if(m_R==m_R2)
        {
          const double R_sqrt2 = m_R / sqrt(2);
          const double long_cath = m_R * cos(alpha);
          const double short_cath = m_R * sin(alpha);

          tab[0] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-m_R,xc);
          tab[1] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-long_cath,xc+short_cath);
          tab[2] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-R_sqrt2,xc+R_sqrt2);
          tab[3] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-short_cath,xc+long_cath);

          tab[4] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc,xc+m_R);
          tab[5] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc+short_cath,xc+long_cath);
          tab[6] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc+R_sqrt2,xc+R_sqrt2);
          tab[7] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc+long_cath,xc+short_cath);

          tab[8] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc+m_R,xc);
          tab[9] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc+long_cath,xc-short_cath);
          tab[10] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc+R_sqrt2,xc-R_sqrt2);
          tab[11] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc+short_cath,xc-long_cath);

          tab[12] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc,xc-m_R);
          tab[13] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-short_cath,xc-long_cath);
          tab[14] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-R_sqrt2,xc-R_sqrt2);
          tab[15] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-long_cath,xc-short_cath);
        }
        else
        {
          /*An elipse*/
          tab[0] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-m_R,xc);
          tab[1] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-m_R*cos(2*PI*1/m_P),xc+m_R2*sin(2*PI*1/m_P));
          tab[2] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-m_R*cos(2*PI*2/m_P),xc+m_R2*sin(2*PI*2/m_P));
          tab[3] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-m_R*cos(2*PI*3/m_P),xc+m_R2*sin(2*PI*3/m_P));

          tab[4] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc,xc+m_R2);
          tab[5] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-m_R*cos(2*PI*5/m_P),xc+m_R2*sin(2*PI*5/m_P));
          tab[6] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-m_R*cos(2*PI*6/m_P),xc+m_R2*sin(2*PI*6/m_P));
          tab[7] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-m_R*cos(2*PI*7/m_P),xc+m_R2*sin(2*PI*7/m_P));

          tab[8] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc+m_R,xc);
          tab[9] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-m_R*cos(2*PI*9/m_P),xc+m_R2*sin(2*PI*9/m_P));
          tab[10] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-m_R*cos(2*PI*10/m_P),xc+m_R2*sin(2*PI*10/m_P));
          tab[11] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-m_R*cos(2*PI*11/m_P),xc+m_R2*sin(2*PI*11/m_P));

          tab[12] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc,xc-m_R2);
          tab[13] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-m_R*cos(2*PI*13/m_P),xc+m_R2*sin(2*PI*13/m_P));
          tab[14] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-m_R*cos(2*PI*14/m_P),xc+m_R2*sin(2*PI*14/m_P));
          tab[15] = bob::sp::detail::bilinearInterpolationNoCheck(src,yc-m_R*cos(2*PI*15/m_P),xc+m_R2*sin(2*PI*15/m_P));

        }
      }
      else // there is no possibility for non-circular LBP16
      {
        throw bob::ip::Exception();
      }

      const T center = src(yc,xc);
      double cmp_point = center;
      if (m_to_average)
      {
        for (int i=0; i < 16; i++)
        {
          cmp_point += tab[i];
        }
        cmp_point *= 0.058823529; // 1/17 (averaged over 17 points)
      } 

      // calculating the lbp
      uint16_t lbp = 0;

      if(m_eLBP_type == 0) 
      {
        for (int i=0; i < 16; i++)
        {
          lbp = lbp << 1;
          if(tab[i] >= cmp_point) ++lbp;
        }

        if(m_add_average_bit && !m_rotation_invariant && !m_uniform)
        {
          lbp = lbp << 1;
          if(center > cmp_point) ++lbp;
        }
      }

      if (m_eLBP_type == 1) // transitional LBP
      {
        for(int i=0; i<=15; i++)
        {
          lbp = lbp << 1;
          if(i==15)
          {
            if(tab[i] >= tab[0]) ++lbp;
          }
          else    
            if(tab[i] >= tab[i+1]) ++lbp;
        }
      }

      if (m_eLBP_type == 2) //directional coded LBP
      {
        for(int i=0; i<=7; i++)
        {
          lbp = lbp << 2;
          if((tab[i] >= cmp_point && tab[i+8] >= cmp_point) || (tab[i] < cmp_point && tab[i+8] < cmp_point))
            if (fabs(tab[i]-cmp_point) > fabs(tab[i+8]-cmp_point)) lbp+=3;
            else lbp+=2;
          else
            if (fabs(tab[i]-cmp_point) > fabs(tab[i+8]-cmp_point)) lbp+=0;
            else lbp+=1;
        }
      }

      return m_lut_current(lbp);
    }

  template <typename T>
    const blitz::TinyVector<int,2> 
    bob::ip::LBP16R::getLBPShape(const blitz::Array<T,2>& src) const
    {
      blitz::TinyVector<int,2> res;
      res(0) = std::max(0, src.extent(0)-2*static_cast<int>(ceil(m_R)));
      res(1) = std::max(0, src.extent(1)-2*static_cast<int>(ceil(m_R2)));
      return res;
    }

} }

#endif /* BOB_IP_LBP16R_H */
