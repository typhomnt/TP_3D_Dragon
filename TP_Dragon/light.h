/* Created on: Jan 30, 2015
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef PROJECT_LIGHT_H_
# define PROJECT_LIGHT_H_

# include "vec.h"

struct Light {
  vec4 position;
  vec4 la;
  vec4 ld;
  vec4 ls;

  Light( const vec4& p, const vec4& a, const vec4& d, const vec4& s ) :
    position( p ), la( a ), ld( d ), ls( s )
  {}

  Light() :
     position( 0.0, 0.0, 0.0, 1.0 ),
     la( 0, 0, 0, 1 ),
     ld( 1, 1, 1, 1 ),
     ls( 1, 1, 1, 1 )
  {}
};

# endif 
