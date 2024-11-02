#ifndef   DEBUG_H_
  #define DEBUG_H_

  #include <stdio.h>


  //  do not set this value above nine
  extern unsigned debug;
  

  #define DEBUG(lvl,x)  if (debug >= lvl) \
      {printf("%s, %d: %s\n", __FILE__, __LINE__, x);}

  #define LINE \
      printf("%s: %d\n", __FILE__, __LINE__); \
      fflush(stdout);


#endif    /*  DEBUG_H_  */