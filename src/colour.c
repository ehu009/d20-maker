#include "colour.h"


void hsv_fromHex(uint32_t src, hsv_t *dst)
{
  double  R = (double)((src & 0x0ff0000)>> 16),
      G = (double)((src & 0x0ff00) >> 8),
      B = (double)(src & 0x0ff),
      M, m, d,
      H = 0.0,
      S = 0.0;
  R /= 255;
  G /= 255;
  B /= 255;
  M = R;
  m = R;
  if (M < G)
    M = G;
  if (M < B)
    M = B;
  if (m > G)
    m = G;
  if (m > B)
    m = B;
  d = M - m;

  if (M != 0.0)
    S = d;
  if (d != 0.0)
  {
    S /= M;
    if (R == M)
    {
      H = G - B;
      H /= d;
      H = fmod (H, 6.0);
    }
    else if (G == M)
    {
      H = B - R;
      H /= d;
      H += 2.0;
    }
    else if (B == M)
    {
      H = R - G;
      H /= d;
      H += 4.0;
    }
    H *= 60.0;
  }
  buf->h = H;
  buf->s = S;
  buf->v = M;
}



void hsv_getHex(hsv_t *src, uint32_t *dst)
{
  double  C = color->v * color->s,
      H = (color->h / 60),
      X = fmod(H, 2.0) - 1,
      m = color->v - C,
      r = m,
      b = m,
      g = m;
  if (X < 0)
    X *= -1;
  X = (1 - X)*C;
  if (H >= 0 && H <2)
  {
    if (H < 1)
    { r += C; g += X; }
    else
    { r += X; g += C; }
  }
  else if (H >= 2 && H < 4)
  {
    if (H < 3)
    { g += C; b += X; }
    else
    { g += X; b += C; }
  }
  else if (H >= 4 && H < 6)
  {
    if (H < 5)
    { r += X; b += C; }
    else
    { r += C; b += X; }
  }
  r *= 255;
  g *= 255;
  b *= 255;
  *dst = r + 0.5;
  *dst <<= 8;
  *dst |= g + 0.5;
  *dst <<= 8;
  *dst |= b + 0.5;
}



void colour_divert (uint32_t *src, uint32_t *dst)
{
  hsv_t clr;
  double diff;
  hsv_fromHex(*src, &clr);

  diff = 180.0;
  diff *= colour_divert_factor;
  clr.h += diff;

  diff = clr.v - clr.s;
  diff *= colour_divert_factor;
  clr.s += diff;

  diff = clr.s - clr.v;
  diff *= colour_divert_factor;
  clr.v += diff;

  while (clr.h > 360.0)
    clr.h -= 360.0;

  while (clr.s < 0)
    clr.s += 1;
  while (clr.s > 1)
    clr.s -= 1;

  while (clr.v < 0)
    clr.v += 1;
  while (clr.v > 1)
    clr.v -= 1;

  hsv_getHex(&clr, dst);
}
