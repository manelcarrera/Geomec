/********************************************************************

  RescueLogicalOrder.cpp

  This object is used to declare the logical ordering of
  RescueUnit, RescueHorizon, and RescueEventDescr (unconformity
  and diapir).

  Rod Hanks,  June 2002

*********************************************************************/
#include "RescueLogicalOrder.h"
#include "RescueBuffer.h"
#include "RescueEventDescr.h"
#include "RescueModel.h"
#include <string.h>

RescueLogicalOrder::RescueLogicalOrder(RescueContext *context)
    : RescueHistoryObject(context), allocated(0), count(0), orderedItems(0) {
  isA = R_RescueLogicalOrder;
}

RescueLogicalOrder::RescueLogicalOrder(RescueModel *model, FILE *archiveFile)
    : RescueHistoryObject(model->Context()), allocated(0), count(0), orderedItems(0) {
  RescueContext *context = model->Context();
  myfscanf(context, archiveFile, &allocated);
  orderedItems = (RescueLogicalOrderEntry **)malloc(sizeof(RescueLogicalOrderEntry *) * (size_t)allocated);
  RESCUEINT32 ndx = 0;
  while (ndx < allocated) {
    RESCUEINT64 type;
    RESCUEINT64 id;
    myfscanf(context, archiveFile, &type);
    myfscanf(context, archiveFile, &id);
    ndx++;
    switch (type) {
    case R_RescueUnit: {
      RescueLogicalOrderEntry *unit = model->UnitIdentifiedBy(id);
      if (unit != 0) {
        orderedItems[count++] = unit;
      }
    } break;
    case R_RescueHorizon: {
      RescueLogicalOrderEntry *horiz = model->HorizonIdentifiedBy(id);
      if (horiz != 0) {
        orderedItems[count++] = horiz;
      }
    } break;
    case R_RescueEventDescr: {
      RescueLogicalOrderEntry *event = model->EventDescrIdentifiedBy(id);
      if (event != 0) {
        orderedItems[count++] = event;
      }
    } break;
    }
  }
  if (context->ReadFileVersion() >= 37) {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0) {
      RescueBuffer buf(context, archiveFile);
      myfgets(context, myString, 255, archiveFile);
    }
  }
}

void RescueLogicalOrder::Archive(RescueContext *context, FILE *archiveFile) {
  myfprintf(context, archiveFile, "; RescueLogicalOrder");
  myfprintf(context, archiveFile, count);
  RESCUEINT32 loop;
  for (loop = 0; loop < count; loop++) {
    myfprintf(context, archiveFile, (RESCUEINT64)orderedItems[loop]->IsA());
    myfprintf(context, archiveFile, orderedItems[loop]->Identifier());
  }
  if (context->FileVersion() >= 37) {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueLogicalOrder::~RescueLogicalOrder() {
  if (orderedItems != 0) {
    free(orderedItems);
  }
}

void RescueLogicalOrder::EnsureCapacity() {
  if (allocated == 0) {
    allocated = 20;
    orderedItems = (RescueLogicalOrderEntry **)malloc(sizeof(RescueLogicalOrderEntry *) * (size_t)allocated);
  } else if (allocated == count) {
    allocated += 20;
    orderedItems =
        (RescueLogicalOrderEntry **)realloc(orderedItems, sizeof(RescueLogicalOrderEntry *) * (size_t)allocated);
  }
}

RESCUEBOOL RescueLogicalOrder::Remove(RescueLogicalOrderEntry *existingObject) {
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT32 ndx = 0;
  RESCUEINT64 howMany = count;
  RESCUEINT32 loop;
  for (loop = 0; loop < howMany; loop++) {
    if (orderedItems[loop] == existingObject) {
      myReturn = TRUE;
      count--;
    } else if (myReturn == TRUE) {
      orderedItems[ndx++] = orderedItems[loop];
    } else {
      ndx++;
    }
  }
  return myReturn;
}

void RescueLogicalOrder::InsertAtBase(RescueLogicalOrderEntry *object) {
  Remove(object);
  EnsureCapacity();
  RESCUEINT64 loop;
  for (loop = count; loop > 0; loop--) {
    orderedItems[loop] = orderedItems[loop - 1];
  }
  orderedItems[0] = object;
  count++;
}

void RescueLogicalOrder::InsertAtTop(RescueLogicalOrderEntry *object) {
  Remove(object);
  EnsureCapacity();
  orderedItems[count++] = object;
}

RESCUEBOOL RescueLogicalOrder::InsertAbove(RescueLogicalOrderEntry *existingObject, RescueLogicalOrderEntry *object) {
  RESCUEBOOL myReturn = FALSE;
  Remove(object);
  RESCUEINT32 ndx = NdxOf(existingObject);
  if (ndx >= 0) {
    EnsureCapacity();
    RESCUEINT64 loop;
    for (loop = count; loop > ndx + 1; loop--) {
      orderedItems[loop] = orderedItems[loop - 1];
    }
    orderedItems[ndx + 1] = object;
    count++;
    myReturn = TRUE;
  }
  return myReturn;
}

RESCUEBOOL RescueLogicalOrder::InsertBelow(RescueLogicalOrderEntry *existingObject, RescueLogicalOrderEntry *object) {
  RESCUEBOOL myReturn = FALSE;
  Remove(object);
  RESCUEINT32 ndx = NdxOf(existingObject);
  if (ndx >= 0) {
    EnsureCapacity();
    RESCUEINT64 loop;
    for (loop = count; loop > ndx; loop--) {
      orderedItems[loop] = orderedItems[loop - 1];
    }
    orderedItems[ndx] = object;
    count++;
    myReturn = TRUE;
  }
  return myReturn;
}

RescueLogicalOrderEntry *RescueLogicalOrder::NthObject(RESCUEINT32 zeroBasedOrdinal) {
  RescueLogicalOrderEntry *myReturn = 0;
  if (zeroBasedOrdinal >= 0 && zeroBasedOrdinal < count) {
    myReturn = orderedItems[zeroBasedOrdinal];
  }
  return myReturn;
}

RESCUEINT32 RescueLogicalOrder::NdxOf(RescueLogicalOrderEntry *object) {
  RESCUEINT32 myReturn = -1;
  RESCUEINT32 loop;
  for (loop = 0; loop < count && myReturn < 0; loop++) {
    if (orderedItems[loop] == object) {
      myReturn = loop;
    }
  }
  return myReturn;
}

RescueUnit *RescueLogicalOrder::NthUnit(RESCUEINT32 zeroBasedOrdinal) {
  RescueUnit *myReturn = 0;
  RESCUEINT32 loop;
  for (loop = 0; loop < count && myReturn == 0; loop++) {
    if (orderedItems[loop]->IsOfType(R_RescueUnit)) {
      zeroBasedOrdinal--;
      if (zeroBasedOrdinal < 0) {
        myReturn = (RescueUnit *)orderedItems[loop];
      }
    }
  }
  return myReturn;
}

RescueHorizon *RescueLogicalOrder::NthHorizon(RESCUEINT32 zeroBasedOrdinal) {
  RescueHorizon *myReturn = 0;
  RESCUEINT32 loop;
  for (loop = 0; loop < count && myReturn == 0; loop++) {
    if (orderedItems[loop]->IsOfType(R_RescueHorizon)) {
      zeroBasedOrdinal--;
      if (zeroBasedOrdinal < 0) {
        myReturn = (RescueHorizon *)orderedItems[loop];
      }
    }
  }
  return myReturn;
}

RescueUnit *RescueLogicalOrder::BottomUnit() {
  RescueUnit *myReturn = 0;
  RESCUEINT32 loop;
  for (loop = 0; loop < count && myReturn == 0; loop++) {
    if (orderedItems[loop]->IsOfType(R_RescueUnit)) {
      myReturn = (RescueUnit *)orderedItems[loop];
    }
  }
  return myReturn;
}

RescueUnit *RescueLogicalOrder::TopUnit() {
  RescueUnit *myReturn = 0;
  RESCUEINT64 loop;
  for (loop = count - 1; loop >= 0 && myReturn == 0; loop--) {
    if (orderedItems[loop]->IsOfType(R_RescueUnit)) {
      myReturn = (RescueUnit *)orderedItems[loop];
    }
  }
  return myReturn;
}

RescueHorizon *RescueLogicalOrder::BottomHorizon() {
  RescueHorizon *myReturn = 0;
  RESCUEINT32 loop;
  for (loop = 0; loop < count && myReturn == 0; loop++) {
    if (orderedItems[loop]->IsOfType(R_RescueHorizon)) {
      myReturn = (RescueHorizon *)orderedItems[loop];
    }
  }
  return myReturn;
}

RescueHorizon *RescueLogicalOrder::TopHorizon() {
  RescueHorizon *myReturn = 0;
  RESCUEINT64 loop;
  for (loop = count - 1; loop >= 0 && myReturn == 0; loop--) {
    if (orderedItems[loop]->IsOfType(R_RescueHorizon)) {
      myReturn = (RescueHorizon *)orderedItems[loop];
    }
  }
  return myReturn;
}

RescueHorizon *RescueLogicalOrder::HorizonAbove(RescueLogicalOrderEntry *existingObject) {
  RescueHorizon *myReturn = 0;
  RESCUEINT32 ndx = NdxOf(existingObject);
  if (ndx >= 0) {
    RESCUEBOOL pleaseContinue = TRUE;
    RESCUEINT32 loop;
    for (loop = ndx + 1; loop < count && myReturn == 0 && pleaseContinue == TRUE; loop++) {
      if (orderedItems[loop]->IsOfType(R_RescueHorizon)) {
        myReturn = (RescueHorizon *)orderedItems[loop];
      } else if (orderedItems[loop]->IsA() == existingObject->IsA()) {
        pleaseContinue = FALSE;
      }
    }
  }
  return myReturn;
}

RescueHorizon *RescueLogicalOrder::HorizonBelow(RescueLogicalOrderEntry *existingObject) {
  RescueHorizon *myReturn = 0;
  RESCUEINT32 ndx = NdxOf(existingObject);
  if (ndx >= 0) {
    RESCUEBOOL pleaseContinue = TRUE;
    RESCUEINT32 loop;
    for (loop = ndx - 1; loop >= 0 && myReturn == 0 && pleaseContinue == TRUE; loop--) {
      if (orderedItems[loop]->IsOfType(R_RescueHorizon)) {
        myReturn = (RescueHorizon *)orderedItems[loop];
      } else if (orderedItems[loop]->IsA() == existingObject->IsA()) {
        pleaseContinue = FALSE;
      }
    }
  }
  return myReturn;
}

RescueUnit *RescueLogicalOrder::UnitAbove(RescueLogicalOrderEntry *existingObject) {
  RescueUnit *myReturn = 0;
  RESCUEINT32 ndx = NdxOf(existingObject);
  if (ndx >= 0) {
    RESCUEBOOL pleaseContinue = TRUE;
    RESCUEINT32 loop;
    for (loop = ndx + 1; loop < count && myReturn == 0 && pleaseContinue == TRUE; loop++) {
      if (orderedItems[loop]->IsOfType(R_RescueUnit)) {
        myReturn = (RescueUnit *)orderedItems[loop];
      } else if (orderedItems[loop]->IsA() == existingObject->IsA()) {
        pleaseContinue = FALSE;
      }
    }
  }
  return myReturn;
}

RescueUnit *RescueLogicalOrder::UnitBelow(RescueLogicalOrderEntry *existingObject) {
  RescueUnit *myReturn = 0;
  RESCUEINT32 ndx = NdxOf(existingObject);
  if (ndx >= 0) {
    RESCUEBOOL pleaseContinue = TRUE;
    RESCUEINT32 loop;
    for (loop = ndx - 1; loop >= 0 && myReturn == 0 && pleaseContinue == TRUE; loop--) {
      if (orderedItems[loop]->IsOfType(R_RescueUnit)) {
        myReturn = (RescueUnit *)orderedItems[loop];
      } else if (orderedItems[loop]->IsA() == existingObject->IsA()) {
        pleaseContinue = FALSE;
      }
    }
  }
  return myReturn;
}

RescueEventDescr *RescueLogicalOrder::EventAbove(RescueLogicalOrderEntry *existingObject) {
  RescueEventDescr *myReturn = 0;
  RESCUEINT32 ndx = NdxOf(existingObject);
  if (ndx >= 0) {
    RESCUEBOOL pleaseContinue = TRUE;
    RESCUEINT32 loop;
    for (loop = ndx + 1; loop < count && myReturn == 0 && pleaseContinue == TRUE; loop++) {
      if (orderedItems[loop]->IsOfType(R_RescueEventDescr)) {
        myReturn = (RescueEventDescr *)orderedItems[loop];
      } else if (orderedItems[loop]->IsA() == existingObject->IsA()) {
        pleaseContinue = FALSE;
      }
    }
  }
  return myReturn;
}

RescueEventDescr *RescueLogicalOrder::EventBelow(RescueLogicalOrderEntry *existingObject) {
  RescueEventDescr *myReturn = 0;
  RESCUEINT32 ndx = NdxOf(existingObject);
  if (ndx >= 0) {
    RESCUEBOOL pleaseContinue = TRUE;
    RESCUEINT32 loop;
    for (loop = ndx - 1; loop >= 0 && myReturn == 0 && pleaseContinue == TRUE; loop--) {
      if (orderedItems[loop]->IsOfType(R_RescueEventDescr)) {
        myReturn = (RescueEventDescr *)orderedItems[loop];
      } else if (orderedItems[loop]->IsA() == existingObject->IsA()) {
        pleaseContinue = FALSE;
      }
    }
  }
  return myReturn;
}

RescueLogicalOrderEntry *RescueLogicalOrder::UnconformityEventAbove(RescueEventDescr *existingObject) {
  RescueLogicalOrderEntry *myReturn = 0;
  RESCUEINT32 ndx = NdxOf(existingObject);
  if (ndx >= 0) {
    RESCUEBOOL pleaseContinue = TRUE;
    RESCUEINT32 loop;
    for (loop = ndx + 1; loop < count && myReturn == 0 && pleaseContinue == TRUE; loop++) {
      if (orderedItems[loop]->IsOfType(R_RescueEventDescr) || orderedItems[loop]->IsOfType(R_RescueHorizon)) {
        myReturn = (RescueEventDescr *)orderedItems[loop];
      } else if (orderedItems[loop]->IsOfType(R_RescueUnit)) {
        pleaseContinue = FALSE;
      }
    }
  }
  return myReturn;
}

RescueLogicalOrderEntry *RescueLogicalOrder::UnconformityEventBelow(RescueEventDescr *existingObject) {
  RescueLogicalOrderEntry *myReturn = 0;
  RESCUEINT32 ndx = NdxOf(existingObject);
  if (ndx >= 0) {
    RESCUEBOOL pleaseContinue = TRUE;
    RESCUEINT32 loop;
    for (loop = ndx - 1; loop >= 0 && myReturn == 0 && pleaseContinue == TRUE; loop--) {
      if (orderedItems[loop]->IsOfType(R_RescueEventDescr) || orderedItems[loop]->IsOfType(R_RescueHorizon)) {
        myReturn = (RescueEventDescr *)orderedItems[loop];
      } else if (orderedItems[loop]->IsOfType(R_RescueUnit)) {
        pleaseContinue = FALSE;
      }
    }
  }
  return myReturn;
}

void RescueLogicalOrder::BuildFrom(RescueModel *model) {
  RESCUEBOOL pleaseContinue = TRUE;
  RESCUEINT32 unitCount = model->UnitCount();
  RESCUEINT32 unitLoop;
  for (unitLoop = 0; unitLoop < unitCount && pleaseContinue == TRUE; unitLoop++) {
    pleaseContinue = FALSE;
    RESCUEINT32 uNdx = 0;
    RescueUnit *unit = model->NthRescueUnit(uNdx++);
    while (unit != 0) {
      if (NdxOf(unit) < 0) {
        RescueHorizon *horizAbove = model->HorizonIdentifiedBy(unit->horizonAboveID);
        RescueHorizon *horizBelow = model->HorizonIdentifiedBy(unit->horizonBelowID);
        if (count == 0) {
          InsertAtBase(unit);
          if (horizAbove != 0) {
            InsertAbove(unit, horizAbove);
          }
          if (horizBelow != 0) {
            InsertBelow(unit, horizBelow);
          }
        } else if (horizAbove == 0 && horizBelow == 0) {
          InsertAtTop(unit);
        }
        /*
          Above clause suggested by Alan Heibert, December, 2003. (v35.1)
        */
        else {
          RESCUEINT32 hANdx = NdxOf(horizAbove);
          if (hANdx >= 0) {
            InsertBelow(horizAbove, unit);
            if (horizBelow != 0) {
              if (NdxOf(horizBelow) < 0) {
                InsertBelow(unit, horizBelow);
              }
            }
          } else {
            RESCUEINT32 hBNdx = NdxOf(horizBelow);
            if (hBNdx >= 0) {
              InsertAbove(horizBelow, unit);
              if (horizAbove != 0) {
                if (NdxOf(horizAbove) < 0) {
                  InsertAbove(unit, horizAbove);
                }
              }
            } else {
              // SLB PROPOSED CHANGE - START

              // invalid model - more than one horizon between units. But try and recover an
              // ordering by matching unitAbove[Below] this unit's horizAbove[Below]
              RescueUnit *unitAboveHorizAbove = model->UnitIdentifiedBy(horizAbove->UnitAboveID());
              RESCUEINT32 uANdx = NdxOf(unitAboveHorizAbove);
              if (uANdx >= 0) {
                // unit above this one already in  ordering, so add this unit in below
                // that unit's horizBelow
                RescueHorizon *horizBelowUnit = model->HorizonIdentifiedBy(unitAboveHorizAbove->horizonBelowID);
                InsertBelow(horizBelowUnit, unit);
                // we know neither horizAbove or horizBelow were in the ordering already
                InsertBelow(unit, horizBelow);
                InsertAbove(unit, horizAbove);
              } else {
                RescueUnit *unitBelowHorizBelow = model->UnitIdentifiedBy(horizBelow->UnitBelowID());
                RESCUEINT32 uBNdx = NdxOf(unitBelowHorizBelow);
                if (uBNdx >= 0) {
                  // unit below this one already in  ordering, so add this unit in above
                  // that unit's horizAbove
                  RescueHorizon *horizAboveUnit = model->HorizonIdentifiedBy(unitBelowHorizBelow->horizonAboveID);
                  InsertBelow(horizAboveUnit, unit);
                  // we know neither horizAbove or horizBelow were in the ordering already
                  InsertBelow(unit, horizBelow);
                  InsertAbove(unit, horizAbove);
                } else {
                  // failed to find next unit
                  pleaseContinue = TRUE;
                }
                // pleaseContinue = TRUE;

                // SLB PROPOSED CHANGE - END
              }
            }
          }
        }
      }
      unit = model->NthRescueUnit(uNdx++);
    }
  }
  /*
    At this point we should have the units and the horizons in.
  */
  pleaseContinue = TRUE;
  RESCUEINT32 eventCount = model->EventDescrCount();
  RESCUEINT32 eventLoop;
  for (eventLoop = 0; eventLoop < eventCount && pleaseContinue == TRUE; eventLoop++) {
    pleaseContinue = FALSE;
    RESCUEINT32 eNdx = 0;
    RescueEventDescr *event = model->NthRescueEventDescr(eNdx++);
    while (event != 0) {
      if (NdxOf(event) < 0) {
        RescueHorizon *horizAbove = model->HorizonIdentifiedBy(event->horizonAboveID);
        RescueHorizon *horizBelow = model->HorizonIdentifiedBy(event->horizonBelowID);
        RESCUEINT32 hANdx = NdxOf(horizAbove);
        if (hANdx >= 0) {
          InsertBelow(horizAbove, event);
          if (horizBelow != 0) {
            if (NdxOf(horizBelow) < 0) {
              InsertBelow(event, horizBelow);
            }
          }
        } else {
          RESCUEINT32 hBNdx = NdxOf(horizBelow);
          if (hBNdx >= 0) {
            InsertAbove(horizBelow, event);
            if (horizAbove != 0) {
              if (NdxOf(horizAbove) < 0) {
                InsertAbove(event, horizAbove);
              }
            }
          } else {
            RescueEventDescr *unconformityAbove = model->EventDescrIdentifiedBy(event->eventDescrAboveID);
            RescueEventDescr *unconformityBelow = model->EventDescrIdentifiedBy(event->eventDescrBelowID);
            RESCUEINT32 uANdx = NdxOf(unconformityAbove);
            if (uANdx >= 0) {
              InsertBelow(unconformityAbove, event);
              if (unconformityBelow != 0) {
                if (NdxOf(unconformityBelow) < 0) {
                  InsertBelow(event, unconformityBelow);
                }
              }
            } else {
              RESCUEINT32 uBNdx = NdxOf(unconformityBelow);
              if (uBNdx >= 0) {
                InsertAbove(unconformityBelow, event);
                if (unconformityAbove != 0) {
                  if (NdxOf(unconformityAbove) < 0) {
                    InsertAbove(event, unconformityAbove);
                  }
                }
              } else {
                pleaseContinue = TRUE;
              }
            }
          }
        }
      }
      event = model->NthRescueEventDescr(eNdx++);
    }
  }
  /*
    Fill in the events in between.
  */
}
