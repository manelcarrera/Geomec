/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef TET_GRAPH_H
#define TET_GRAPH_H

#include "tet_utils.h"

typedef struct TetGraph_t {
  void *activeTree;
  void *connTree;
} TetGraph_t;

extern TetGraph_t *TetGraphCreate(void);
extern TetGraph_t *TetGraphInit(TetGraph_t *, const int);
extern TetGraph_t *TetGraphDelete(TetGraph_t *);
extern TetGraph_t *TetGraphCopy(TetGraph_t *);
extern size_t TetGraphSize(TetGraph_t *);
extern void TetGraphAdd(TetGraph_t *, int, int);
extern int *TetGraphGetConnectivity(TetGraph_t *, int *);
extern void TetGraphPrint(TetGraph_t *, FILE *);
extern bool_t TetGraphExists(TetGraph_t *, int, int);

#endif
