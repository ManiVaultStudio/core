#pragma once

#include "actions/Actions.h"

class DataImportAction : public hdps::gui::WidgetAction
{
public:
    DataImportAction(QObject* parent);

    QMenu* getContextMenu();

protected:
    hdps::gui::TriggerAction    _imagesAction;
    hdps::gui::TriggerAction    _csvAction;
    hdps::gui::TriggerAction    _binaryAction;
    hdps::gui::TriggerAction    _hdf5Action;
};