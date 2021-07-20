#pragma once

#include "actions/Actions.h"

class DataAnalysisAction : public hdps::gui::WidgetAction
{
public:
    DataAnalysisAction(QObject* parent);

    QMenu* getContextMenu();

protected:
    hdps::gui::TriggerAction    _meanShiftAction;
    hdps::gui::TriggerAction    _tsneAction;
};