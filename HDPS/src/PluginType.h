#ifndef HDPS_PLUGINTYPE_H
#define HDPS_PLUGINTYPE_H

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
    enum class Type { ANALYSIS, DATA, LOADER, WRITER, VIEW };

    struct TypeHash
    {
        std::size_t operator()(Type type) const
        {
            return static_cast<std::size_t>(type);
        }
    };

} // namespace plugin

} // namespace hdps

#endif // HDPS_PLUGINTYPE_H
