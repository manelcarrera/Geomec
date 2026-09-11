/*                                         Copyright (c) 2011 TNO DIANA BV */
#include "tet_graph.h"
#include "RBTree.h"
#include "tet_utils.h"

typedef struct TetGraphEntry_t {
  int row;
  int col;
} TetGraphEntry_t;

static int CompareTetGraphEntry(const void *data0, const void *data1) {
  const TetGraphEntry_t *i0 = (const TetGraphEntry_t *)data0;
  const TetGraphEntry_t *i1 = (const TetGraphEntry_t *)data1;
  int result = UTIL_COMPARE(i0->row, i1->row);
  if (!result) {
    result = UTIL_COMPARE(i0->col, i1->col);
  }
  return result;
}

extern TetGraph_t *TetGraphInit(TetGraph_t *graph, const int dim) {
  int n;
  assert(graph != NULL && dim >= 0);
  graph->connTree = RBTreeDelete(graph->connTree);
  graph->activeTree = RBTreeDelete(graph->activeTree);
  graph->connTree = RBTreeCreate(sizeof(TetGraphEntry_t), CompareTetGraphEntry);
  graph->activeTree = RBTreeCreate(sizeof(int), CompareInt);
  for (n = 0; n < dim; n++) {
    TetGraphAdd(graph, n, n);
  }
  return graph;
}

extern TetGraph_t *TetGraphCreate(void) {
  TetGraph_t *graph = DIMALLOC(sizeof(*graph));
  UTIL_ZERO(graph);
  TetGraphInit(graph, 0);
  return graph;
}

extern TetGraph_t *TetGraphDelete(TetGraph_t *graph) {
  if (graph) {
    RBTreeDelete(graph->connTree);
    RBTreeDelete(graph->activeTree);
    DIFREE(graph);
  }
  return graph;
}

extern TetGraph_t *TetGraphCopy(TetGraph_t *graph) {
  TetGraph_t *result = TetGraphCreate();
  if (graph) {
    Iterator_t actIter, connIter;
    const int *active = RBTreeFirst(graph->activeTree, &actIter);
    const TetGraphEntry_t *ge = RBTreeFirst(graph->connTree, &connIter);
    while (active) {
      TetGraphAdd(result, *active, *active);
      active = RBTreeNext(graph->activeTree, &actIter);
    }
    while (ge) {
      TetGraphAdd(result, ge->row, ge->col);
      ge = RBTreeNext(graph->connTree, &connIter);
    }
  }
  return graph;
}

extern size_t TetGraphSize(TetGraph_t *graph) {
  int result = 0;
  Iterator_t iter;
  const int *entry = RBTreeFirst(graph->activeTree, &iter);
  while (entry) {
    result = MAX(result, *entry + 1);
    entry = RBTreeNext(graph->activeTree, &iter);
  }
  return result;
}

extern void TetGraphAdd(TetGraph_t *graph, int row, int col) {
  assert(row >= 0 && col >= 0);
  RBTreeSearch(graph->activeTree, &row);
  RBTreeSearch(graph->activeTree, &col);
  if (row != col) {
    TetGraphEntry_t ge;
    ge.row = row;
    ge.col = col;
    RBTreeSearch(graph->connTree, &ge);
    ge.row = col;
    ge.col = row;
    RBTreeSearch(graph->connTree, &ge);
  }
}

extern bool_t TetGraphExists(TetGraph_t *graph, int row, int col) {
  TetGraphEntry_t ge;
  ge.row = row;
  ge.col = col;
  return (RBTreeFind(graph->connTree, &ge) != RBTREE_INVALID_ID);
}

extern void TetGraphPrint(TetGraph_t *graph, FILE *fp) {
  Iterator_t iter;
  TetGraphEntry_t *ge = RBTreeFirst(graph->connTree, &iter);
  fprintf(fp, "\nTetGraph: dim=%d\n", (int)TetGraphSize(graph));
  while (ge) {
    fprintf(fp, "  row=%d\tcol=%d\n", ge->row, ge->col);
    ge = RBTreeNext(graph->connTree, &iter);
  }
  fprintf(fp, "\n");
}

static int TetGraphPrepare(TetGraph_t *graph, int **offsetP, int **indxP) {
  const int dim = (int)TetGraphSize(graph);
  const int siz = (int)RBTreeSize(graph->connTree);
  int d;
  int *indx = DIMALLOC(siz * sizeof(*indx));
  int *offset = DIMALLOC((dim + 1) * sizeof(*offset));
  Iterator_t iter;
  const TetGraphEntry_t *ge = RBTreeFirst(graph->connTree, &iter);

  *indxP = indx;
  while (ge) {
    *indx = ge->col;
    indx += 1;
    ge = RBTreeNext(graph->connTree, &iter);
  }

  *offsetP = offset;
  UTIL_SET(offset, 0, dim + 1);
  ge = RBTreeFirst(graph->connTree, &iter);
  while (ge) {
    offset[ge->row + 1] += 1;
    ge = RBTreeNext(graph->connTree, &iter);
  }
  for (d = 1; d <= dim; d++) {
    offset[d] += offset[d - 1];
  }

  return dim;
}

/* Auxilary routine to TetGraphGetConnectivity */
static int TetGraphGetTrueColor(const int *color, const int node) {
  int trueColor = node;
  assert(node >= 0);
  while (trueColor > color[trueColor]) {
    trueColor = color[trueColor];
  }
  assert(trueColor == color[trueColor]);
  return trueColor;
}

static int TetGraphSetTrueColor(int *color, const int trueColor, const int node) {
  int curColor = color[node];
  assert(node >= 0);
  /* Give this node its true color */
  assert(color[node] >= trueColor);
  color[node] = trueColor;
  /* ... and all related colors */
  while (color[curColor] > trueColor) {
    const int nextColor = color[curColor];
    color[curColor] = trueColor;
    curColor = nextColor;
  }
  assert(color[curColor] == trueColor);
  return trueColor;
}

/*
 *ddoc TetGraphGetConnectivity: Determine the connectivity of a graph
 *ddoc
 *ddoc Arguments:
 *ddoc    graph    TetGraph_t  In   - graph
 *ddoc    numColor int      Out  - Number of disconnected sub-graphs
 *ddoc
 *ddoc  Return value:
 *ddoc    List with sub-graph number per node (must be freed by caller!)
 *ddoc
 *ddoc Description:
 *ddoc   Given the graph connectivity disconnected sub-graphs are detected,
 *ddoc   and the sub-graph number (0...) is returned
 *ddoc
 */
extern int *TetGraphGetConnectivity(TetGraph_t *graph, int *numColor) {
  int *graphOffset = NULL, *graphIndex = NULL;
  const int dim = TetGraphPrepare(graph, &graphOffset, &graphIndex);
  int *color = DIMALLOC(dim * sizeof(*color));
  int row;

  /* Each dof in its own segment */
  UTIL_TRIVIAL_PERM(color, dim);

  /* Loop over rows, and assign the lowest color to all connected nodes */
  for (row = 0; row < dim; row++) {
    const int *indx = graphIndex + graphOffset[row];
    const int numIndex = graphOffset[row + 1] - graphOffset[row];
    int minColor = TetGraphGetTrueColor(color, row);
    int c;

    /* Determine the lowest color in the cluster */
    for (c = 0; c < numIndex; c++) {
      minColor = MIN(minColor, TetGraphGetTrueColor(color, indx[c]));
    }

    /* Join all nodes in the to the cluster */
    TetGraphSetTrueColor(color, minColor, row);
    for (c = 0; c < numIndex; c++) {
      TetGraphSetTrueColor(color, minColor, indx[c]);
    }
  }

  /* Set segments */
  for (row = 0; row < dim; row++) {
    color[row] = TetGraphGetTrueColor(color, row);
    assert(color[row] >= 0);
  }

  /* Renumber consecutively */
  UTIL_RESEQUENCE(color, dim);

  if (dim > 0) {
    const int numNotUsed = (int)(dim - RBTreeSize(graph->activeTree));

    UTIL_GET_MAXIMUM(color, dim, numColor);
    *numColor += 1 - numNotUsed;

    /* There may be holes in the numbering, give unused entries color -1 */
    if (numNotUsed) {
      int n;
      for (n = 0; n < dim; n++) {
        const int *entry = RBTreeFindGet(graph->activeTree, &n);
        if (!entry)
          color[n] = -1;
      }
      UTIL_RESEQUENCE(color, dim);
      UTIL_ARRAY_ADD(color, -1, dim);
    }
  } else {
    *numColor = 1;
  }

  DIFREE(graphOffset);
  DIFREE(graphIndex);
  return color;
}
