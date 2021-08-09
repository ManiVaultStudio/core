#pragma once

#include "actions/Actions.h"

namespace hdps {

class DataHierarchyItem;

class AnalyzeDataAction : public hdps::gui::WidgetAction
{
public:
    AnalyzeDataAction(QObject* parent, DataHierarchyItem* dataHierarchyItem);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr);

protected:
    DataHierarchyItem*          _dataHierarchyItem;     /** Pointer to data hierarchy item */
    hdps::gui::TriggerAction    _meanShiftAction;       /** Mean-shift clustering action */
    hdps::gui::TriggerAction    _tsneAction;            /** TSNE action */
    hdps::gui::TriggerAction    _hsneAction;            /** HSNE action */
};

}