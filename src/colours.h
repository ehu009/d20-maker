#ifndef   COLOURS_H_
  #define   COLOURS_H_

  #define   COLOUR   uint32_t

  extern unsigned alpha_mask;

  #define   FONT_COLOUR_WHITE   (SDL_Color) {0xff,0xff,0xff}
  #define   FONT_COLOUR_BLACK   (SDL_Color) {0,0,0}

  #define   COLOUR_BACKGROUND   0x0dbdbdb


  #define   TRIANGLE_COLOUR_PINNED   0xff000000
  #define   TRIANGLE_COLOUR_UNPINNED 0xffff0000


  #define   BUTTON_COLOUR_DEFAULT 0x303030
  #define   BUTTON_COLOUR_OVER    0x595959
  #define   BUTTON_COLOUR_DOWN    0x0ff0000


  #define   FADER_COLOUR_BAR      0xffffff

  #define   FADER_COLOUR_DEFAULT  0x303030
  #define   FADER_COLOUR_OVER     0x595959
  #define   FADER_COLOUR_DOWN     0x0ff0000




#endif  /*  COLOURS_H_  */
