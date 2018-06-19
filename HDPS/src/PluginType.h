#pragma once

/**
 * Provides all possible types of plugins.
 *
 * Analysis       - A plugin to perform complex computation on a dataset.
 * Data           - A plugin that defines the structure of raw data.
 * Loader         - A plugin that loads a specific type of data into a dataset.
 * Writer         - A plugin that writes a specific type of data to disk.
 * Transformation - A plugin to transform data in a minimal way.
 * View           - A plugin to display a dataset on screen.
 */

namespace hdps
{
namespace plugin
{

enum Type { ANALYSIS, DATA, LOADER, WRITER, VIEW };

} // namespace plugin

} // namespace hdps
