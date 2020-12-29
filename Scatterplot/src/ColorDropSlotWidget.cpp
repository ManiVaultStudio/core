#include "ColorDropSlotWidget.h"
#include "ScatterplotPlugin.h"

ColorDropSlotWidget::ColorDropSlotWidget(ScatterplotPlugin& plugin) :
    _loadColorData(*new hdps::gui::DataSlot(plugin.supportedColorTypes))
{
    auto dropPushButton = new QPushButton();

    _loadColorData.addWidget(dropPushButton);

    connect(&_loadColorData, &hdps::gui::DataSlot::onDataInput, &plugin, &ScatterplotPlugin::onColorDataInput);

    auto layout = new QHBoxLayout();

    layout->addWidget(&_loadColorData);

    setLayout(layout);
}