#include "ColorByDimensionSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QHBoxLayout>
#include <QComboBox>

using namespace hdps::gui;

ColorByDimensionSettingsWidget::ColorByDimensionSettingsWidget(QWidget* parent /*= nullptr*/) :
    ResponsiveToolBar::StatefulWidget(parent, "Color dimension"),
    _colorDimensionComboBox(new QComboBox())
{
    initializeUI();
}

void ColorByDimensionSettingsWidget::initializeUI()
{
    _colorDimensionComboBox->setToolTip("Pick color dimension to color point data");
}

void ColorByDimensionSettingsWidget::setScatterPlotPlugin(const ScatterplotPlugin& plugin)
{
}

QLayout* ColorByDimensionSettingsWidget::getLayout(const ResponsiveToolBar::WidgetState& state)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(_colorDimensionComboBox);

    switch (state)
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

    return layout;
}