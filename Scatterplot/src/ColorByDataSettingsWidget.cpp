#include "ColorByDataSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

using namespace hdps::gui;

ColorByDataSettingsWidget::ColorByDataSettingsWidget(const ResponsiveSectionWidget::WidgetState& widgetState, QWidget* parent /*= nullptr*/) :
    ScatterplotSettingsWidget(widgetState, parent),
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

    applyLayout(layout);

    layout->addWidget(_colorDataLineEdit);
    layout->addWidget(_removeColorDataPushButton);

    if (isForm())
        _colorDataLineEdit->setFixedWidth(120);

    if (isSequential()) {
        if (isCompact())
            _colorDataLineEdit->setFixedWidth(80);

        if (isFull())
            _colorDataLineEdit->setFixedWidth(120);
    }
}

void ColorByDataSettingsWidget::setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin)
{
}