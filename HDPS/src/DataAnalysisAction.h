#pragma once

#include "actions/Actions.h"

namespace hdps {

class Core;

class DataAnalysisAction : public hdps::gui::WidgetAction
{
public:
    DataAnalysisAction(QObject* parent, Core* core, const QString& datasetName);

    QMenu* getContextMenu();

protected:
    hdps::gui::TriggerAction    _meanShiftAction;
    hdps::gui::TriggerAction    _tsneAction;
};

}