#include "ColorByDataSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

using namespace hdps::gui;

ColorByDataSettingsWidget::ColorByDataSettingsWidget(const hdps::gui::ResponsiveToolBar::WidgetState& state, QWidget* parent /*= nullptr*/) :
    ScatterplotSettingsWidget(state, parent),
    _colorDataLineEdit(new QLineEdit()),
    _removeColorDataPushButton(new QPushButton())
{
    _colorDataLineEdit->setToolTip("Name of the assigned color data set");
    _colorDataLineEdit->setEnabled(false);
    _colorDataLineEdit->setReadOnly(true);
    _colorDataLineEdit->setText("No color data...");
    _colorDataLineEdit->setTextMargins(5, 0, 0, 0);

    _removeColorDataPushButton->setToolTip("Removed assigned color data");
    _removeColorDataPushButton->setIcon(Application::getIconFont("FontAwesome").getIcon("trash"));

    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(4);

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

    setLayout(layout);
}

void ColorByDataSettingsWidget::connectToPlugin()
{
}