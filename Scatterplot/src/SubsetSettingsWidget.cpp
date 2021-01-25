#include "SubsetSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QDebug>
#include <QPushButton>
#include <QCheckBox>

using namespace hdps::gui;

SubsetSettingsWidget::SubsetSettingsWidget(QWidget* parent /*= nullptr*/) :
    ResponsiveToolBar::StatefulWidget(parent, "Subset"),
    _createSubsetPushButton(new QPushButton()),
    _fromSourceCheckBox(new QCheckBox())
{
    initializeUI();
}

void SubsetSettingsWidget::initializeUI()
{
    _createSubsetPushButton->setToolTip("Create a subset from the selected data points");

    _fromSourceCheckBox->setToolTip("Create a subset from source or derived data");
    _fromSourceCheckBox->setText("From source");
}

void SubsetSettingsWidget::setScatterPlotPlugin(const ScatterplotPlugin& plugin)
{
    auto& scatterplotPlugin = const_cast<ScatterplotPlugin&>(plugin);
    auto scatterPlotWidget  = const_cast<ScatterplotPlugin&>(plugin).getScatterplotWidget();

    const auto updateUI = [this, &scatterplotPlugin, scatterPlotWidget]() {
        const auto isScatterPlot = scatterPlotWidget->getRenderMode() == ScatterplotWidget::RenderMode::SCATTERPLOT;
        setEnabled(isScatterPlot && scatterplotPlugin.getNumSelectedPoints() >= 1);
        qDebug() << isEnabled();
    };

    QObject::connect(&plugin, qOverload<>(&ScatterplotPlugin::selectionChanged), [this, updateUI]() {
        updateUI();
    });

    QObject::connect(scatterPlotWidget, &ScatterplotWidget::renderModeChanged, [this, updateUI](const ScatterplotWidget::RenderMode& renderMode) {
        updateUI();
    });

    updateUI();
}

QLayout* SubsetSettingsWidget::getLayout(const ResponsiveToolBar::WidgetState& state)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(_createSubsetPushButton);
    layout->addWidget(_fromSourceCheckBox);

    switch (state)
    {
        case ResponsiveToolBar::WidgetState::Popup:
            _createSubsetPushButton->setText("Create");
            _fromSourceCheckBox->setText("From source dataset");
            break;

        case ResponsiveToolBar::WidgetState::Compact:
        case ResponsiveToolBar::WidgetState::Full:
            _createSubsetPushButton->setText("Create subset");
            _fromSourceCheckBox->setText(state == ResponsiveToolBar::WidgetState::Compact ? "Source" : "From source");
            break;

        default:
            break;
    }

    return layout;
}