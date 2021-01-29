#include "SettingsWidget.h"
#include "ScatterplotPlugin.h"

#include "RenderModeWidget.h"
#include "PlotSettingsWidget.h"
#include "PositionSettingsWidget.h"
#include "ColorSettingsWidget.h"
#include "SubsetSettingsWidget.h"
#include "SelectionSettingsWidget.h"

#include "widgets/ResponsiveToolBar.h"

#include <QPushButton>
#include <QDialog>
#include <QPropertyAnimation>

using namespace hdps::gui;

const hdps::Vector3f SettingsWidget::DEFAULT_BASE_COLOR = hdps::Vector3f(255.f / 255, 99.f / 255, 71.f / 255);
const hdps::Vector3f SettingsWidget::DEFAULT_SELECTION_COLOR = hdps::Vector3f(72.f / 255, 61.f / 255, 139.f / 255);

SettingsWidget::SettingsWidget(const ScatterplotPlugin& plugin) :
    QWidget(static_cast<QWidget*>(&const_cast<ScatterplotPlugin&>(plugin))),
    _responsiveToolBar(new ResponsiveToolBar(this)),
    _baseColor(DEFAULT_BASE_COLOR),
    _selectionColor(DEFAULT_SELECTION_COLOR)
{
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

    ScatterplotSettingsWidget::scatterplotPlugin = &const_cast<ScatterplotPlugin&>(plugin);

    auto horizontalLayout = new QHBoxLayout();

    horizontalLayout->setMargin(0);
    horizontalLayout->setSpacing(0);
    horizontalLayout->addWidget(_responsiveToolBar);
    horizontalLayout->setSizeConstraint(QLayout::SetMaximumSize);

    setLayout(horizontalLayout);

    _responsiveToolBar->setListenWidget(&const_cast<ScatterplotPlugin&>(plugin));

    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    const auto initializeWidgetFunction = [&plugin](QWidget* widget) {
        auto scatterplotSettingsWidget = dynamic_cast<ScatterplotSettingsWidget*>(widget);

        if (!scatterplotSettingsWidget)
            return;

        scatterplotSettingsWidget->setScatterplotPlugin(&const_cast<ScatterplotPlugin&>(plugin));
    };

    _responsiveToolBar->addWidget<RenderModeWidget>(initializeWidgetFunction, "Render mode", fontAwesome.getIcon("toggle-on"), 10);
    _responsiveToolBar->addWidget<PlotSettingsWidget>(initializeWidgetFunction, "Plot", fontAwesome.getIcon("cogs"), 250);
    _responsiveToolBar->addWidget<PositionSettingsWidget>(initializeWidgetFunction, "Position", fontAwesome.getIcon("ruler-combined"), 250);
    _responsiveToolBar->addWidget<ColorSettingsWidget>(initializeWidgetFunction, "Color", fontAwesome.getIcon("palette"), 250);
    _responsiveToolBar->addWidget<SubsetSettingsWidget>(initializeWidgetFunction, "Color", fontAwesome.getIcon("crop"), 50);
    _responsiveToolBar->addWidget<SelectionSettingsWidget>(initializeWidgetFunction, "Selection", fontAwesome.getIcon("mouse-pointer"), 5);

    //setEnabled(false);

    connect(&plugin, &ScatterplotPlugin::currentDatasetChanged, [this](const QString& currentDataset) {
        setEnabled(!currentDataset.isEmpty());
    });
}

int SettingsWidget::getXDimension()
{
    return 0;
    //return _positionSettingsWidget->getDimensionX();
}

int SettingsWidget::getYDimension()
{
    return 0;
    //return _positionSettingsWidget->getDimensionY();
}

hdps::Vector3f SettingsWidget::getBaseColor()
{
    return _baseColor;
}

hdps::Vector3f SettingsWidget::getSelectionColor()
{
    return _selectionColor;
}

void SettingsWidget::initDimOptions(const unsigned int nDim)
{
    //_positionSettingsWidget->setDimensions(nDim);
}

void SettingsWidget::initDimOptions(const std::vector<QString>& dimNames)
{
    //_positionSettingsWidget->setDimensions(dimNames.size(), dimNames);
}

void SettingsWidget::initScalarDimOptions(const unsigned int nDim)
{
    //_colorSettingsWidget->setScalarDimensions(nDim);
}

void SettingsWidget::initScalarDimOptions(const std::vector<QString>& dimNames)
{
    //_colorSettingsWidget->setScalarDimensions(dimNames.size(), dimNames);
}