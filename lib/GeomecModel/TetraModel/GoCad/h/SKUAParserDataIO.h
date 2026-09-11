#pragma once

#include "StorageNode.h"

namespace gm_skua {

struct SKUAParseData;

void LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress,
                struct gm_skua::SKUAParseData &data);

void SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress,
                const struct gm_skua::SKUAParseData &data);

// LoadStreamLegacy is in GocadData

} // namespace gm_skua
