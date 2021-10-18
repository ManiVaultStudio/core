#ifndef HDPS_SELECTIONLISTENER_H
#define HDPS_SELECTIONLISTENER_H

#include "graphics/Selection.h"

#include <QObject>

namespace hdps
{
namespace plugin
{

class SelectionListener
{
public:
    /**
    * Callback function which gets triggered when a selection is being made.
    * @param selection - The box selection which is being made
    */
    virtual void onSelecting(Selection selection) = 0;

    /**
    * Callback function which gets triggered when a selection is made.
    * @param selection - The box selection which was made
    */
    virtual void onSelection(Selection selection) = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::SelectionListener, "hdps.SelectionListener")

#endif // HDPS_SELECTIONLISTENER_H
