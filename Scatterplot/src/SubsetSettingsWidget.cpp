#include "SubsetSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QDebug>
#include <QPushButton>
#include <QCheckBox>

using namespace hdps::gui;

SubsetSettingsWidget::SubsetSettingsWidget(const hdps::gui::ResponsiveSectionWidget::WidgetState& widgetState, QWidget* parent /*= nullptr*/) :
    ScatterplotSettingsWidget(widgetState, parent),
    _createSubsetPushButton(new QPushButton()),
    _fromSourceCheckBox(new QCheckBox())
{
    _createSubsetPushButton->setToolTip("Create a subset from the selected data points");

    _fromSourceCheckBox->setToolTip("Create a subset from source or derived data");
    _fromSourceCheckBox->setText("From source");

    
    auto layout = new QHBoxLayout();

    applyLayout(layout); (layout);

    layout->addWidget(_createSubsetPushButton);
    layout->addWidget(_fromSourceCheckBox);

    _createSubsetPushButton->setText(isForm() ? "Create" : "Create subset");
    _fromSourceCheckBox->setText(isCompact() ? "Source" : "From source");
}

void SubsetSettingsWidget::setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin)
{
    _scatterplotPlugin = scatterplotPlugin;

    auto scatterPlotWidget  = _scatterplotPlugin->getScatterplotWidget();

    const auto updateUI = [this, scatterPlotWidget]() {
        const auto isScatterPlot = scatterPlotWidget->getRenderMode() == ScatterplotWidget::RenderMode::SCATTERPLOT;
        setEnabled(isScatterPlot && _scatterplotPlugin->getNumSelectedPoints() >= 1);
    };

    QObject::connect(_scatterplotPlugin, qOverload<>(&ScatterplotPlugin::selectionChanged), this, [this, updateUI]() {
        updateUI();
    });

    QObject::connect(scatterPlotWidget, &ScatterplotWidget::renderModeChanged, this, [this, updateUI](const ScatterplotWidget::RenderMode& renderMode) {
        updateUI();
    });

    updateUI();
}