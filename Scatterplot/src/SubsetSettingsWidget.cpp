#include "SubsetSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QDebug>
#include <QPushButton>
#include <QCheckBox>

using namespace hdps::gui;

SubsetSettingsWidget::SubsetSettingsWidget(const hdps::gui::ResponsiveToolBar::WidgetState& state, QWidget* parent /*= nullptr*/) :
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
        case ResponsiveToolBar::WidgetState::Popup:
            _createSubsetPushButton->setText("Create");
            _fromSourceCheckBox->setText("From source dataset");
            break;

        case ResponsiveToolBar::WidgetState::Compact:
        case ResponsiveToolBar::WidgetState::Full:
            _createSubsetPushButton->setText("Create subset");
            _fromSourceCheckBox->setText(_state == ResponsiveToolBar::WidgetState::Compact ? "Source" : "From source");
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

void SubsetSettingsWidget::connectToPlugin()
{
    /*
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
    */
}