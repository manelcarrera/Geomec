/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef BODYSET_H
#define BODYSET_H

#include "Set.h"
#include "tet_body.h"
#include "tet_utils.h"

typedef struct BodySet_t {
  Set_t *body;
} BodySet_t;

extern BodySet_t *BodySetCreate(void);
extern BodySet_t *BodySetDelete(BodySet_t *);
extern BodySet_t *BodySetClear(BodySet_t *);
extern void BodySetAdd(BodySet_t *, Body_t *);
extern Body_t *BodySetFirst(BodySet_t *, Iterator_t *);
extern Body_t *BodySetNext(BodySet_t *, Iterator_t *);
extern void BodySetPrint(BodySet_t *, FILE *);
extern Body_t *BodySetGet(BodySet_t *bodySet, int id);
extern int BodySetSize(const BodySet_t *bodyset);
extern BoundBox_t *BodySetBoundBox(BodySet_t *, PointSet_t *, BoundBox_t *);
extern void BodySetHierarchy(void *, BodySet_t *, PointSet_t *);

#endif
