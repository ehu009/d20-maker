#ifndef   NET_SLOT_H_
  #define NET_SLOT_H_

  #include "drawn-triangles.h"



  /*
   *  assigns slots in a net to triangles
   */

  struct slot
  {
    unsigned pinned;
    double x, y;
    //  possible screen locations
    triangle_t *positions[3];
    //  neighboring triangles
    struct slot *A, *B, *C;

    unsigned visited; //  might not need this one
    //  perhaps a mutex device
        //  depends on implementation
  };


  void init_slot (struct slot *ptr);

  int slot_index (struct slot *arr, struct slot *ptr);

  int slot_has_triangle (struct slot *slot, triangle_t *t);

  int slot_has_neighbor (struct slot *slot, struct slot *ptr);

  struct slot *neighbor_with_triangle (struct slot *s, triangle_t *t);


#endif  /*  NET_SLOT_H_  */
