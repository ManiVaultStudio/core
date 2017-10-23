#ifndef HDPS_PLUGIN_SELECTION_LISTENER_H
#define HDPS_PLUGIN_SELECTION_LISTENER_H

#include <QObject>

#include <vector>

namespace hdps
{
namespace plugin
{

class SelectionListener
{
public:
    /**
    * Callback function which gets triggered when a new dataset is created.
    * @param name - The name of the dataset which was created.
    */
    virtual void onSelection(const std::vector<unsigned int> selection) const = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::SelectionListener, "cytosplore.SelectionListener")

#endif // HDPS_PLUGIN_SELECTION_LISTENER_H
