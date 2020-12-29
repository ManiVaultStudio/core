#pragma once

#include "widgets/DataSlot.h"

#include <QWidget>

class ScatterplotPlugin;

struct ColorDropSlotWidget : public QWidget
{
    Q_OBJECT
public:
    ColorDropSlotWidget(ScatterplotPlugin& plugin);

private:
    hdps::gui::DataSlot& _loadColorData;
};