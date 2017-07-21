

struct hsv_colour
{
  double h, s, v;
};
typedef struct hsv_colour hsv_t;



void colour_divert (uint32_t *src, uint32_t *dst);


extern double colour_divert_factor;
