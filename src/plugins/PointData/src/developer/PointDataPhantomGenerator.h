#pragma once

class QMenu;
class QWidget;

namespace mv::pointdata::developer
{
/**
 * @brief Adds developer-menu actions for creating large synthetic datasets.
 *
 * The generator is kept in the developer menu because its purpose is to
 * exercise storage, serialization, and view performance with data sizes that
 * are inconvenient to prepare manually. It can create deterministic or
 * random point data and reports the estimated raw and tiled memory footprint
 * before creating the dataset.
 *
 * @author Thomas Kroes
 */
class PointDataPhantomGenerator final
{
public:
    /** Adds the phantom point-data action to the supplied menu. */
    static void populateMenu(QMenu& menu, QWidget* owner);
};
}
