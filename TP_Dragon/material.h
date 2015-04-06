/* Created on: Jan 30, 2015
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef PROJECT_MATERIAL_H_
# define PROJECT_MATERIAL_H_

# include "vec.h"

struct Material {
  vec4 ka;
  vec4 kd;
  vec4 ks;
  float shininess;

  Material()
  : ka( 0.19225 , 0.19225 , 0.19225  , 1),
    kd( 0.50754 , 0.50754 , 0.50754  , 1),
    ks( 0.508273, 0.508273, 0.508273 , 1),
    shininess(51.2f)
  {}

  Material( const vec4& ambient, const vec4& diffuse, const vec4& specular,
      float shininess ) :
    ka( ambient ), kd( diffuse ), ks( specular ), shininess( shininess )
  {}
};

# endif 
