
#ifndef   ICOSAHEDRON_H_
  #define   ICOSAHEDRON_H_

  #include  "vertex.h"


  #define NUM_D20_VTX   12

  /*
   *  model of vertices in an icosahedron
   */

  vtx5i_t d20_model[NUM_D20_VTX] =
  {
   {.pts = {1,2,3,5,6}},
   {.pts = {0,2,3,4,7}},
   {.pts = {0,1,4,5,8}},
   {.pts = {0,1,6,7,9}},
   {.pts = {1,2,7,8,10}},
   {.pts = {0,2,6,8,11}},
   {.pts = {0,3,5,9,11}},
   {.pts = {1,3,4,9,10}},
   {.pts = {2,4,5,10,11}},
   {.pts = {3,6,7,10,11}},
   {.pts = {4,7,8,9,11}},
   {.pts = {5,6,8,9,10}}
  };



#endif
