#ifndef   D20_MODEL_H_
  #define   D20_MODEL_H_

  #include  "net_slot.h"

  //  amount of triangles in an icosahedron
  #define   TRIANGLES_TOTAL 20

  /*
   *  neighbor relations between triangles on an icosahedron
   */

  /*
   *  relationships are stored as integer values, such that
   *    they have three components, each are an index to an array
   *    that represents the net of the icosahedron.
   */

  //  five bits needed to count to 20
  #define MASK 0b011111

  //  so we need an integer type for 3 numbers, each up to 20
  //  =3*5=15 bits, +1 bit => one short
  #define NET_MODEL_TYPE short

  //  shift amounts for each number composing the net model type
  enum {SHAMT_A=10, SHAMT_B=5, SHAMT_C=0};


  //  these macros compose integers storing three numbers

  #define NEIGHBOR_A(x) (-1 + (((MASK << SHAMT_A) & x) >> SHAMT_A))
  #define NEIGHBOR_B(x) (-1 + (((MASK << SHAMT_B) & x) >> SHAMT_B))
  #define NEIGHBOR_C(x) (-1 + (((MASK << SHAMT_C) & x) >> SHAMT_C))

  #define NET_MODEL_ENTRY(a,b,c)  (NET_MODEL_TYPE) 0 + (a<<SHAMT_A) + (b<<SHAMT_B) + (c<<SHAMT_C)

  //  integers that model relationship between itself and other integers
    //  as if edging surfaces on an icosahedron
  NET_MODEL_TYPE net_model [TRIANGLES_TOTAL] =
  {
    NET_MODEL_ENTRY(3,2,4),
    NET_MODEL_ENTRY(1,6,5),
    NET_MODEL_ENTRY(1,8,7),
    NET_MODEL_ENTRY(1,10,9),
    NET_MODEL_ENTRY(2,11,10),
    NET_MODEL_ENTRY(2,7,12),
    NET_MODEL_ENTRY(3,13,6),
    NET_MODEL_ENTRY(3,9,14),
    NET_MODEL_ENTRY(4,15,8),
    NET_MODEL_ENTRY(4,5,16),
    NET_MODEL_ENTRY(5,12,17),
    NET_MODEL_ENTRY(6,18,11),
    NET_MODEL_ENTRY(7,14,18),
    NET_MODEL_ENTRY(8,19,13),
    NET_MODEL_ENTRY(9,16,19),
    NET_MODEL_ENTRY(10,17,15),
    NET_MODEL_ENTRY(11,20,16),
    NET_MODEL_ENTRY(12,13,20),
    NET_MODEL_ENTRY(14,15,20),
    NET_MODEL_ENTRY(17,18,19)
  };


#endif  /*  D20_MODEL_H_  */
