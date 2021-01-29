#include "ColorSettingsWidget.h"
#include "ColorByDimensionSettingsWidget.h"
#include "ColorByDataSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

using namespace hdps::gui;

ColorSettingsWidget::ColorSettingsWidget(const hdps::gui::ResponsiveToolBar::WidgetState& state, QWidget* parent /*= nullptr*/) :
    ScatterplotSettingsWidget(state, parent),
    _colorByLabel(new QLabel()),
    _colorByComboBox(new QComboBox()),
    _stackedWidget(new StackedWidget()),
    _colorByDimensionSettingsWidget(new ColorByDimensionSettingsWidget(state)),
    _colorByDataSettingsWidget(new ColorByDataSettingsWidget(state))
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

    layout->setMargin(0);
    layout->setSpacing(4);

    layout->addWidget(_colorByLabel);
    layout->addWidget(_colorByComboBox);
    layout->addWidget(_stackedWidget);

    setLayout(layout);
}

void ColorSettingsWidget::connectToPlugin()
{
    /*
    _scatterplotPlugin = &const_cast<ScatterplotPlugin&>(plugin);

    _colorByDimensionSettingsWidget->setScatterPlotPlugin(plugin);
    _colorByDataSettingsWidget->setScatterPlotPlugin(plugin);

    connect(_colorByComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
        _stackedWidget->setCurrentIndex(index);
        computeSizeHints();
    });

    connect(_colorByDimensionSettingsWidget->getColorDimensionComboBox(), qOverload<int>(&QComboBox::currentIndexChanged), [this, &plugin](int index) {
        const_cast<ScatterplotPlugin&>(plugin).cDimPicked(index);
    });

    auto scatterPlotWidget = const_cast<ScatterplotPlugin&>(plugin).getScatterplotWidget();

    const auto renderModeChanged = [this, scatterPlotWidget]() {
        setEnabled(scatterPlotWidget->getRenderMode() == ScatterplotWidget::RenderMode::SCATTERPLOT);
    };

    QObject::connect(scatterPlotWidget, &ScatterplotWidget::renderModeChanged, [this, renderModeChanged](const ScatterplotWidget::RenderMode& renderMode) {
        renderModeChanged();
    });

    computeSizeHints();

    renderModeChanged();
    */
}