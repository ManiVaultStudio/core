// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <PointData/pointdata_export.h>

#include <cstdint>
#include <optional>

namespace mv::gui
{

/** Returns current available physical memory when the operating system exposes it. */
POINTDATA_EXPORT std::optional<std::uint64_t> getAvailablePhysicalMemoryBytes();

}
