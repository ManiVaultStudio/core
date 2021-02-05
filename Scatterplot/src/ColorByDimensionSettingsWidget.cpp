#include "ColorByDimensionSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QHBoxLayout>
#include <QComboBox>

using namespace hdps::gui;

ColorByDimensionSettingsWidget::ColorByDimensionSettingsWidget(const ResponsiveSectionWidget::WidgetState& widgetState, QWidget* parent /*= nullptr*/) :
    ScatterplotSettingsWidget(widgetState, parent),
    _colorDimensionComboBox(new QComboBox())
{
    _colorDimensionComboBox->setToolTip("Pick color dimension to color point data");

    auto layout = new QHBoxLayout();

    applyLayout(layout);

    layout->addWidget(_colorDimensionComboBox);

    if (isForm())
        _colorDimensionComboBox->setFixedWidth(120);

    if (isSequential()) {
        if (isCompact())
            _colorDimensionComboBox->setFixedWidth(80);

        if (isFull())
            _colorDimensionComboBox->setFixedWidth(120);
    }
}

void ColorByDimensionSettingsWidget::setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin)
{
}