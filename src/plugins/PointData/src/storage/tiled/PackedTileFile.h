// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <util/Serialization.h>

namespace mv::pointdata::tiled
{

// Compatibility aliases for the existing PointData tests and callers. Packed
// blob storage is implemented by the shared core serialization layer.
using PackedTileBlock      = mv::util::PackedBlobBlock;
using PackedTileFileWriter = mv::util::PackedBlobFileWriter;
using PackedTileFileReader = mv::util::PackedBlobFileReader;

}
