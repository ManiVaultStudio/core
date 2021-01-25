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

const hdps::Vector3f SettingsWidget::DEFAULT_BASE_COLOR = hdps::Vector3f(255.f / 255, 99.f / 255, 71.f / 255);
const hdps::Vector3f SettingsWidget::DEFAULT_SELECTION_COLOR = hdps::Vector3f(72.f / 255, 61.f / 255, 139.f / 255);

SettingsWidget::SettingsWidget(const ScatterplotPlugin& plugin) :
    QWidget(static_cast<QWidget*>(&const_cast<ScatterplotPlugin&>(plugin))),
    _responsiveToolBar(new hdps::gui::ResponsiveToolBar(this)),
    _renderModeWidget(new RenderModeWidget(this)),
    _plotSettingsWidget(new PlotSettingsWidget(this)),
    _positionSettingsWidget(new PositionSettingsWidget(this)),
    _colorSettingsWidget(new ColorSettingsWidget(this)),
    _subsetSettingsWidget(new SubsetSettingsWidget(this)),
    _selectionSettingsWidget(new SelectionSettingsWidget(this)),
    _baseColor(DEFAULT_BASE_COLOR),
    _selectionColor(DEFAULT_SELECTION_COLOR)
{
    setAutoFillBackground(true);
    auto horizontalLayout = new QHBoxLayout();

    horizontalLayout->setMargin(0);
    horizontalLayout->setSpacing(0);
    horizontalLayout->addWidget(_responsiveToolBar);

    setLayout(horizontalLayout);

    _responsiveToolBar->setListenWidget(&const_cast<ScatterplotPlugin&>(plugin));

    _renderModeWidget->setScatterPlotPlugin(plugin);
    _plotSettingsWidget->setScatterPlotPlugin(plugin);
    _positionSettingsWidget->setScatterPlotPlugin(plugin);
    _colorSettingsWidget->setScatterPlotPlugin(plugin);
    _subsetSettingsWidget->setScatterPlotPlugin(plugin);
    _selectionSettingsWidget->setScatterPlotPlugin(plugin);

    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    _responsiveToolBar->addSection(_renderModeWidget, fontAwesome.getIcon("toggle-on"));
    _responsiveToolBar->addSection(_plotSettingsWidget, fontAwesome.getIcon("cogs"));
    _responsiveToolBar->addSection(_positionSettingsWidget, fontAwesome.getIcon("ruler-combined"));
    _responsiveToolBar->addSection(_colorSettingsWidget, fontAwesome.getIcon("palette"));
    _responsiveToolBar->addSection(_subsetSettingsWidget, fontAwesome.getIcon("crop"));
    _responsiveToolBar->addSection(_selectionSettingsWidget, fontAwesome.getIcon("mouse-pointer"));
    _responsiveToolBar->addStretch();

    setEnabled(false);

    connect(&plugin, &ScatterplotPlugin::currentDatasetChanged, [this](const QString& currentDataset) {
        setEnabled(!currentDataset.isEmpty());
    });
}

int SettingsWidget::getXDimension()
{
    return _positionSettingsWidget->getDimensionX();
}

int SettingsWidget::getYDimension()
{
    return _positionSettingsWidget->getDimensionY();
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
    _positionSettingsWidget->setDimensions(nDim);
}

void SettingsWidget::initDimOptions(const std::vector<QString>& dimNames)
{
    _positionSettingsWidget->setDimensions(dimNames.size(), dimNames);
}

void SettingsWidget::initScalarDimOptions(const unsigned int nDim)
{
    _colorSettingsWidget->setScalarDimensions(nDim);
}

void SettingsWidget::initScalarDimOptions(const std::vector<QString>& dimNames)
{
    _colorSettingsWidget->setScalarDimensions(dimNames.size(), dimNames);
}