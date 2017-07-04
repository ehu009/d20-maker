#ifndef   CHAINS_H_
  #define CHAINS_H_


  /*
   *  Ring-linked list ADT
   */

  typedef struct linked_chain chain_t;


  /*
   *  Ring-linked list prototypes
   */

  chain_t *make_chain (void *start_item);
  void free_chain (chain_t *chain);

  int chain_size (chain_t *chain);



  /*
   *  Ring-linked list manipulator ADT
   */

  typedef struct chain_iterator chainslider_t;


  /*
   * Ring-linked list manipulator prototypes
   */

  chainslider_t *make_chainslider (chain_t *linked_chain);
  void free_chainslider (chainslider_t *slider);

  void slider_procede (chainslider_t *slider);
  void slider_recede (chainslider_t *slider);

  void *slider_current (chainslider_t *slider);

  int slider_insert_before (chainslider_t *slider, void *item);
  int slider_insert_after (chainslider_t *slider, void *item);

  void slider_remove_prev (chainslider_t *slider);
  void slider_remove_next (chainslider_t *slider);


#endif  /*  CHAINS_H_ */
