#include "ColorByDimensionSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QHBoxLayout>
#include <QComboBox>

using namespace hdps::gui;

ColorByDimensionSettingsWidget::ColorByDimensionSettingsWidget(const hdps::gui::ResponsiveToolBar::WidgetState& state, QWidget* parent /*= nullptr*/) :
    ScatterplotSettingsWidget(state, parent),
    _colorDimensionComboBox(new QComboBox())
{
    _colorDimensionComboBox->setToolTip("Pick color dimension to color point data");

    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(4);

    layout->addWidget(_colorDimensionComboBox);

    switch (_state)
    {
        case ResponsiveToolBar::WidgetState::Compact:
            _colorDimensionComboBox->setFixedWidth(80);
            break;

        case ResponsiveToolBar::WidgetState::Popup:
        case ResponsiveToolBar::WidgetState::Full:
            _colorDimensionComboBox->setFixedWidth(120);
            break;

        default:
            break;
    }

    setLayout(layout);
}

void ColorByDimensionSettingsWidget::setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin)
{
}