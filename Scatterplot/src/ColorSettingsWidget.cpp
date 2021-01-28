#include "ColorSettingsWidget.h"
#include "ColorByDimensionSettingsWidget.h"
#include "ColorByDataSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

using namespace hdps::gui;

ColorSettingsWidget::ColorSettingsWidget(QWidget* parent /*= nullptr*/) :
    ResponsiveToolBar::StatefulWidget(parent, "Color"),
    _colorByLabel(new QLabel()),
    _colorByComboBox(new QComboBox()),
    _stackedWidget(new StackedWidget()),
    _colorByDimensionSettingsWidget(new ColorByDimensionSettingsWidget()),
    _colorByDataSettingsWidget(new ColorByDataSettingsWidget())
{
    initializeUI();
}

void ColorSettingsWidget::initializeUI()
{
    _colorByLabel->setToolTip("Color by");
    _colorByLabel->setText("Color by:");

    _colorByComboBox->setToolTip("Color by");
    _colorByComboBox->setFixedWidth(75);
    _colorByComboBox->addItem("Dimension");
    _colorByComboBox->addItem("Data");

    _stackedWidget->addWidget(_colorByDimensionSettingsWidget);
    _stackedWidget->addWidget(_colorByDataSettingsWidget);
}

void ColorSettingsWidget::setScatterPlotPlugin(const ScatterplotPlugin& plugin)
{
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
}

void ColorSettingsWidget::setState(const hdps::gui::ResponsiveToolBar::WidgetState& state, const bool& forceUpdate /*= false*/)
{
    StatefulWidget::setState(state);

    _colorByDimensionSettingsWidget->setState(state, true);
    _colorByDataSettingsWidget->setState(state, true);
}

void ColorSettingsWidget::setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names)
{
    auto& stringListModel = createStringListModel(numDimensions, names, *this);

    QSignalBlocker signalBlocker(_colorByDimensionSettingsWidget->getColorDimensionComboBox());

    _colorByDimensionSettingsWidget->getColorDimensionComboBox()->setModel(&stringListModel);
}

QLayout* ColorSettingsWidget::getLayout(const ResponsiveToolBar::WidgetState& state)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(_colorByLabel);
    layout->addWidget(_colorByComboBox);
    layout->addWidget(_stackedWidget);

    return layout;
}