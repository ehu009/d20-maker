
#ifndef   ICOSAHEDRON_H_
  #define   ICOSAHEDRON_H_


  #include "vertex.h"


  /*
   *  model of vertices in an icosahedron
   */
  #define NUM_D20_VTX   12

  vtx5i_t d20_model[NUM_D20_VTX];


  #define NUM_VTX_POS   5

  struct vertex_slot
  {
    struct vertex_slot *links[NUM_VTX_POS];
  };
  typedef struct vertex_slot slot_t;


  void init_d20 (slot_t *root);


  slot_t *find_slot_opposing (slot_t *a1, slot_t *a2, slot_t *opposer);




  struct d20_slot
  {
    slot_t *sA, *sB, *sC;
    void *item;
  };

  typedef struct d20_slot face_t;


  int equal_faces (face_t *A, face_t *B);

  face_t *copy_face (face_t *ptr);



#endif
