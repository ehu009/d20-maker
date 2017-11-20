#ifndef DEBUG_H_
  #define DEBUG_H_

  #include <stdio.h>

  extern int debug;

  #define DEBUG(lvl,x) if(debug >= lvl){printf("%s, %d: %s\n", __FILE__, __LINE__, x);}

  #define LINE printf("line: %d\n", __LINE__);fflush(stdout);


#endif /* DEBUG_H_ */
