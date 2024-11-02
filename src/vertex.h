#ifndef   VERTICES_H_
  #define VERTICES_H_


  /*
   *  Two-point vertex types
   */
  struct vertex_2point_integer
  {
    int pts[2];
  };

  struct vertex_2point_double
  {
    double pts[2];
  };

  typedef struct vertex_2point_integer  vtx2i_t;
  typedef struct vertex_2point_double   vtx2d_t;

  /*
   *  Two-point vertex prototypes
   */
  void get_vtx2i_from_vtx2d (vtx2d_t *src, vtx2i_t *dst);

  vtx2i_t *make_vtx2i_from_vtx2d (vtx2d_t *src);

  int equal_vertices (vtx2d_t *A, vtx2d_t *B, double acc);
  vtx2d_t *find_vector_opposing (vtx2d_t *anchor1, vtx2d_t *anchor2, vtx2d_t *point);


  /*
   *  Five-point vertex types
   */
  struct vertex_5point_integer
  {
    int pts[5];
  };

  typedef struct vertex_5point_integer  vtx5i_t;


#endif    /*  VERTICES_H_ */
