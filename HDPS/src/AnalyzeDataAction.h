#pragma once

#include "actions/Actions.h"

namespace hdps {

class Core;

class AnalyzeDataAction : public hdps::gui::WidgetAction
{
public:
    AnalyzeDataAction(QObject* parent, Core* core, const QString& datasetName);

    QMenu* getContextMenu();

protected:
    hdps::gui::TriggerAction    _meanShiftAction;
    hdps::gui::TriggerAction    _tsneAction;
};

}