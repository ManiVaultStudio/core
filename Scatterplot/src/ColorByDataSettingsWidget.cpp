#include "ColorByDataSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

using namespace hdps::gui;

ColorByDataSettingsWidget::ColorByDataSettingsWidget(QWidget* parent /*= nullptr*/) :
    ResponsiveToolBar::StatefulWidget(parent, "Color dimension"),
    _colorDataLineEdit(new QLineEdit()),
    _removeColorDataPushButton(new QPushButton())
{
    initializeUI();
}

void ColorByDataSettingsWidget::initializeUI()
{
    _colorDataLineEdit->setToolTip("Name of the assigned color data set");
    _colorDataLineEdit->setEnabled(false);
    _colorDataLineEdit->setReadOnly(true);
    _colorDataLineEdit->setText("No color data...");
    _colorDataLineEdit->setTextMargins(5, 0, 0, 0);

    _removeColorDataPushButton->setToolTip("Removed assigned color data");
    _removeColorDataPushButton->setIcon(Application::getIconFont("FontAwesome").getIcon("trash"));
}

void ColorByDataSettingsWidget::setScatterPlotPlugin(const ScatterplotPlugin& plugin)
{
}

QLayout* ColorByDataSettingsWidget::getLayout(const ResponsiveToolBar::WidgetState& state)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(_colorDataLineEdit);
    layout->addWidget(_removeColorDataPushButton);

    switch (state)
    {
        case ResponsiveToolBar::WidgetState::Compact:
            _colorDataLineEdit->setFixedWidth(80);
            break;

        case ResponsiveToolBar::WidgetState::Popup:
        case ResponsiveToolBar::WidgetState::Full:
            _colorDataLineEdit->setFixedWidth(120);
            break;

        default:
            break;
    }

    return layout;
}