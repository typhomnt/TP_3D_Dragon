/*  Created on: Jan 27, 2015
 *      Author: T.Delame (tdelame@gmail.com)
 */
# ifndef PROJECT_VEC_H_
# define PROJECT_VEC_H_

/** Surprisingly enough, Qt does not offer a pointer to float array functionality.
 * Thus we cannot rely on Qt vec and matrices types.
 * Also, using glm leads to compilation error on Mac OS.
 * We just need types to construct the data that will be passed to OpenGL.
 * So we define those silly types...*/
struct vec4 {
  union { float x, s, r; };
  union { float y, t, g; };
  union { float z, p, b; };
  union { float w, q, a; };

  const float* data() const {
    return &x;
  }

  vec4() :
    x(0),y(0),z(0),w(0)
  {}

  vec4( float x, float y, float z, float w ) :
    x(x),y(y),z(z),w(w)
  {}
};

# endif /* PROJECT_VEC_H_ */
