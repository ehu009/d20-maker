#ifndef   HASH256_H_
  #define HASH256_H_

  /*
   *
   *  SHA-256, implemented by Brad Conte
   *      as per published on July 25th 2006 at http://bradconte.com/sha256_c
   *
   *  An overview of the project it is part of, as well as licensing information, can be found below:
   *      https://github.com/B-Con/crypto-algorithms
   *
   */

  struct sha256_object
  {
    unsigned char data[64];
    unsigned int datalen;
    unsigned int bitlen[2];
    unsigned int state[8];
  };

  typedef struct sha256_object SHA256_CTX;


  //  SHA-256 prototypes

  void sha256_init (SHA256_CTX *ctx);

  void sha256_update (SHA256_CTX *ctx, unsigned char data[], unsigned int len);

  void sha256_final (SHA256_CTX *ctx, unsigned char hash[]);


#endif    /*  HASH256_H_  */