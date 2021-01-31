#include "SubsetSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QDebug>
#include <QPushButton>
#include <QCheckBox>

using namespace hdps::gui;

SubsetSettingsWidget::SubsetSettingsWidget(const hdps::gui::ResponsiveSectionWidget::State& state, QWidget* parent /*= nullptr*/) :
    ScatterplotSettingsWidget(state, parent),
    _createSubsetPushButton(new QPushButton()),
    _fromSourceCheckBox(new QCheckBox())
{
    _createSubsetPushButton->setToolTip("Create a subset from the selected data points");

    _fromSourceCheckBox->setToolTip("Create a subset from source or derived data");
    _fromSourceCheckBox->setText("From source");

    auto layout = new QHBoxLayout();

    switch (_state)
    {
        case ResponsiveSectionWidget::State::Popup:
            _createSubsetPushButton->setText("Create");
            _fromSourceCheckBox->setText("From source dataset");
            break;

        case ResponsiveSectionWidget::State::Compact:
        case ResponsiveSectionWidget::State::Full:
            _createSubsetPushButton->setText("Create subset");
            _fromSourceCheckBox->setText(_state == ResponsiveSectionWidget::State::Compact ? "Source" : "From source");
            break;

        default:
            break;
    }

    layout->setMargin(0);
    layout->setSpacing(4);

    layout->addWidget(_createSubsetPushButton);
    layout->addWidget(_fromSourceCheckBox);

    setLayout(layout);
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