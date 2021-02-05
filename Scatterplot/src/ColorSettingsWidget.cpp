#include "ColorSettingsWidget.h"
#include "ColorByDimensionSettingsWidget.h"
#include "ColorByDataSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

using namespace hdps::gui;

ColorSettingsWidget::ColorSettingsWidget(const ResponsiveSectionWidget::WidgetState& widgetState, QWidget* parent /*= nullptr*/) :
    ScatterplotSettingsWidget(widgetState, parent),
    _colorByLabel(new QLabel()),
    _colorByComboBox(new QComboBox()),
    _stackedWidget(new StackedWidget()),
    _colorByDimensionSettingsWidget(new ColorByDimensionSettingsWidget(widgetState)),
    _colorByDataSettingsWidget(new ColorByDataSettingsWidget(widgetState))
{
    _colorByLabel->setToolTip("Color by");
    _colorByLabel->setText("Color by:");

    _colorByComboBox->setToolTip("Color by");
    _colorByComboBox->setFixedWidth(75);
    _colorByComboBox->addItem("Dimension");
    _colorByComboBox->addItem("Data");

    _stackedWidget->addWidget(_colorByDimensionSettingsWidget);
    _stackedWidget->addWidget(_colorByDataSettingsWidget);

    auto layout = new QHBoxLayout();

    applyLayout(layout);

    layout->addWidget(_colorByLabel);
    layout->addWidget(_colorByComboBox);
    layout->addWidget(_stackedWidget);
}

void ColorSettingsWidget::setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin)
{
    _scatterplotPlugin = scatterplotPlugin;

    _colorByDimensionSettingsWidget->setScatterplotPlugin(_scatterplotPlugin);
    _colorByDataSettingsWidget->setScatterplotPlugin(_scatterplotPlugin);

    connect(_colorByComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
        _stackedWidget->setCurrentIndex(index);
    });

    connect(_colorByDimensionSettingsWidget->getColorDimensionComboBox(), qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
        _scatterplotPlugin->cDimPicked(index);
    });

    auto scatterPlotWidget = _scatterplotPlugin->getScatterplotWidget();

    const auto renderModeChanged = [this, scatterPlotWidget]() {
        setEnabled(scatterPlotWidget->getRenderMode() == ScatterplotWidget::RenderMode::SCATTERPLOT);
    };

    QObject::connect(scatterPlotWidget, &ScatterplotWidget::renderModeChanged, this, [this, renderModeChanged](const ScatterplotWidget::RenderMode& renderMode) {
        renderModeChanged();
    });

    renderModeChanged();
}