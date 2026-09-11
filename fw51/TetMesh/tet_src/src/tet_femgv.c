/*                                         Copyright (c) 2011 TNO DIANA BV */
#include "tet_femgv.h"
#include "tet_mesh.h"
#include <ctype.h>
#include <float.h>

#define FGV_FILE_NAME "fgvstr.str"
#define MODEL_FILE "geomec.dat"

typedef struct PointManager_t {
  PointSet_t *pointSet;
  int totalSize;  /* Total number of points in the model (without mesh) */
  int activeSize; /*Number of active points in the model (without mesh)*/
  RBTree_t *activeTree;
} PointManager_t;

#define PointManagerTotalSize(pointManager) ((pointManager)->totalSize)

static PointManager_t *PointManagerCreate(PointSet_t *pointSet, SurfaceSet_t *surfaceSet) {
  PointManager_t *result = DIMALLOC(sizeof(*result));
  UTIL_ZERO(result);
  result->pointSet = pointSet;
  result->totalSize = PointSetSize(pointSet);
  result->activeTree = RBTreeCreate(sizeof(int), CompareInt);

  /* Get the active points */
  {
    Iterator_t surfSetIter;
    Surface_t *surface = SurfaceSetFirst(surfaceSet, &surfSetIter);
    while (surface) {
      Iterator_t iter;
      Triangle_t *triangle = SurfaceFirst(surface, NULL, &iter);
      while (triangle) {
        const int *points = TrianglePoints(triangle);
        int n;
        for (n = 0; n < 3; n++) {
          RBTreeSearch(result->activeTree, points + n);
        }
        triangle = SurfaceNext(surface, NULL, &iter);
      }
      surface = SurfaceSetNext(surfaceSet, &surfSetIter);
    }
  }
  result->activeSize = (int)RBTreeSize(result->activeTree);
  return result;
}

static PointManager_t *PointManagerDelete(PointManager_t *pointManager) {
  if (pointManager) {
    RBTreeDelete(pointManager->activeTree);
    DIFREE(pointManager);
  }
  return NULL;
}

static void PointManagerGet(PointManager_t *pointManager, int totalId, double *x, double *y, double *z) {
  PointSetFind(pointManager->pointSet, totalId, x, y, z);
}

static int PointManagerGetActiveId(PointManager_t *pointManager, int totalId) {
  return RBTreeFind(pointManager->activeTree, &totalId);
}

static int PointManagerGetTotalId(PointManager_t *pointManager, int activeId) {
  int totalId = -1;
  if (activeId < pointManager->activeSize) {
    /* Point exists in model */
    const int *totalIdP = RBTreeGet(pointManager->activeTree, activeId);
    totalId = *totalIdP;
  } else {
    /* New point in the mesh */
    totalId = pointManager->totalSize + activeId - pointManager->activeSize;
  }
  return totalId;
}

/* Return value -1 indicates error */
static int PointManagerAddActivePoint(PointManager_t *pointManager, int activeId, double x, double y, double z) {
  int totalId = PointManagerGetTotalId(pointManager, activeId);
  if (totalId == PointSetSize(pointManager->pointSet)) {
    /* New point, not in model */
    totalId = PointSetAdd(pointManager->pointSet, x, y, z);
    /* Must be new point */
    if (totalId != PointSetSize(pointManager->pointSet) - 1) {
      totalId = -1;
    }
  }
  return totalId;
}

static void FemGvWritePoints(FILE *fp, PointManager_t *pointManager) {
  int activeId;
  const int nActive = (int)RBTreeSize(pointManager->activeTree);

  fprintf(fp, "%10d\n", nActive);
  for (activeId = 0; activeId < nActive; activeId++) {
    const int totalId = PointManagerGetTotalId(pointManager, activeId);
    double cor[3];
    PointManagerGet(pointManager, totalId, cor + 0, cor + 1, cor + 2);
    fprintf(fp, "%25.15e%25.15e%25.15e\n", cor[0], cor[1], cor[2]);
  }
}

static void FemGvWriteTriangles(FILE *fp, PointManager_t *pointManager, SurfaceSet_t *surfaceSet) {
  const int nTriangles = SurfaceSetTriangleSize(surfaceSet);

  Iterator_t surfSetIter;
  Surface_t *surface = SurfaceSetFirst(surfaceSet, &surfSetIter);

  fprintf(fp, "%10d\n", nTriangles);
  while (surface) {
    Iterator_t iter;
    Triangle_t *triangle = SurfaceFirst(surface, NULL, &iter);
    while (triangle) {
      const int *points = TrianglePoints(triangle);
      int activeId[3], n;
      for (n = 0; n < 3; n++) {
        activeId[n] = PointManagerGetActiveId(pointManager, points[n]);
        assert(activeId[n] >= 0 && activeId[n] < pointManager->activeSize);
      }
      /* Add 1 for fortran type numbering */
      fprintf(fp, "%10d%10d%10d\n", activeId[0] + 1, activeId[1] + 1, activeId[2] + 1);
      triangle = SurfaceNext(surface, NULL, &iter);
    }
    surface = SurfaceSetNext(surfaceSet, &surfSetIter);
  }
}

static bool_t FemGvWriteModel(const char *fileName, PointManager_t *pointManager, SurfaceSet_t *surfaceSet) {
  bool_t errFlag = FALSE;
  FILE *fp = fopen(fileName, "w");
  if (fp) {
    FemGvWritePoints(fp, pointManager);
    FemGvWriteTriangles(fp, pointManager, surfaceSet);
    fclose(fp);
  } else {
    errFlag = TRUE;
  }
  return errFlag;
}

static bool_t FemGvReadModel(const char *fileName, PointManager_t *pointManager, TetrahedronSet_t *tetSet) {
  FILE *file = fopen(fileName, "r");
  bool_t errFlag = (file == NULL);
  int nPoints = -1, nTet = -1, scanStatus, n;
  char line[BUFSIZ];

  if (errFlag)
    return errFlag;

  /* Read number of points */
  errFlag = (fgets(line, sizeof(line), file) == NULL);
  if (!errFlag) {
    scanStatus = sscanf(line, "%d", &(nPoints));
    errFlag = (scanStatus != 1);
  }

  /* Read and create new points */
  for (n = 0; n < nPoints && !errFlag; n++) {
    int totalId = -1;
    double cor[3];
    errFlag = (fgets(line, sizeof(line), file) == NULL);
    if (errFlag)
      break;
    scanStatus = sscanf(line, "%le %le %le", cor + 0, cor + 1, cor + 2);
    errFlag = (scanStatus != 3);
    if (errFlag)
      break;
    totalId = PointManagerAddActivePoint(pointManager, n, cor[0], cor[1], cor[2]);
    errFlag = (totalId < 0);
  }

  /* Read number of tetrahedra */
  if (!errFlag) {
    errFlag = (fgets(line, sizeof(line), file) == NULL);
    if (!errFlag) {
      scanStatus = sscanf(line, "%d", &(nTet));
      errFlag = (scanStatus != 1);
    }
  }

  /* Read the tetrahedra */
  for (n = 0; n < nTet && !errFlag; n++) {
    int active[4], total[4], tp;
    errFlag = (fgets(line, sizeof(line), file) == NULL);
    if (errFlag)
      break;
    scanStatus = sscanf(line, "%d %d %d %d", active + 0, active + 1, active + 2, active + 3);
    errFlag = (scanStatus != 4);
    if (errFlag)
      break;
    for (tp = 0; tp < 4; tp++) {
      total[tp] = PointManagerGetTotalId(pointManager, active[tp] - 1);
      errFlag = (total[tp] < 0);
      if (errFlag)
        break;
    }
    if (errFlag)
      break;
    TetrahedronSetAdd(tetSet, total[0], total[1], total[2], total[3]);
  }

  fclose(file);
  return errFlag;
}

bool_t RunFemGV(const FemGvPath_t *pPath) {
  bool_t bRet;
  const char *p;
  char *argv[4];
  const char *path = "${DCPB}/bin;${DIABIN};${DIAPATH}/release/bin;${DIAPATH}/bin";
  char *exe_name;
  int add_exe = 0;
#ifdef _MSC_VER
  int l = strlen(pPath->sFgvExeName);
  if ((l < 4) || !(strcmpcb(pPath->sFgvExeName, ".exe")))
    add_exe = 1;
#endif
  exe_name = vDiStrsave(pPath->sFgvExeName, add_exe ? ".exe" : NULL, NULL);
  p = GetPathName(exe_name, path);
  DIFREE(exe_name);

  /* If exename is not found, try to fall back to the users path */
  if (p)
    exe_name = DiStrsave(p);
  else
    exe_name = DiStrsave(pPath->sFgvExeName);

  argv[0] = DiStrsave(pPath->sFgvExeName);
  argv[1] = "-Alpha";
  argv[2] = "-NoConsoleWindow";
  argv[3] = 0;

  /* Set the environment */
  vDiSetenv("DIAPATH=%s", pPath->sDianaPath);
  vDiSetenv("DIASHARE=%s", pPath->sDiaSharePath);
  vDiSetenv("DIALIB=%s", pPath->sDianaLibPath);
  vDiSetenv("DIASLIB=%s", pPath->sDianaSLibPath);
  vDiSetenv("FGVKEY=%s", pPath->sDianaSLibPath);

  /* Run process */
  bRet = spawnvp(_P_WAIT, exe_name, argv) != -1;

  DIFREE(exe_name);
  DIFREE(argv[0]);

  return bRet;
}

extern TetrahedronSet_t *FemGvMeshBody(PointSet_t *pointSet, SurfaceSet_t *surfaceSet, Body_t *body,
                                       BodySet_t *cavities, const FemGvPath_t *FemGvPath) {
  TetrahedronSet_t *tetSet = TetrahedronSetCreate();
  PointManager_t *pointManager = PointManagerCreate(pointSet, surfaceSet);
  bool_t errFlag = FemGvWriteModel(FGV_FILE_NAME, pointManager, surfaceSet);

  UNUSED(body);
  UNUSED(cavities);

  /* Clean existing files */
  {
    FILE *fp = fopen(MODEL_FILE, "w");
    fclose(fp);
  }

  if (!errFlag) {
    RunFemGV(FemGvPath);
  }
  if (!errFlag) {
    errFlag = FemGvReadModel(MODEL_FILE, pointManager, tetSet);
  }
  /* Return empty tetset if error seen */
  if (errFlag) {
    TetrahedronSetDelete(tetSet);
    tetSet = TetrahedronSetCreate();
  }
  PointManagerDelete(pointManager);
  return tetSet;
}
