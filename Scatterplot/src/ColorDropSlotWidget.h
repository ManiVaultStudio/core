#pragma once

#include "widgets/DataSlot.h"

#include <QWidget>

class ScatterplotPlugin;

struct ColorDropSlotWidget : public QWidget
{
public:
    ColorDropSlotWidget(const ScatterplotPlugin& plugin);

private:
    hdps::gui::DataSlot& _loadColorData;
};