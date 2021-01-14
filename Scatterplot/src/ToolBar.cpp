#include "ToolBar.h"
#include "ScatterplotPlugin.h"

#include "RenderModeWidget.h"
#include "PlotSettingsWidget.h"
#include "DimensionSettingsWidget.h"
#include "SubsetSettingsWidget.h"

#include <QPainter>
#include <QStyleOption>
#include <QPushButton>

#include "ui_SettingsWidget.h"

const hdps::Vector3f SettingsWidget::DEFAULT_BASE_COLOR = hdps::Vector3f(255.f / 255, 99.f / 255, 71.f / 255);
const hdps::Vector3f SettingsWidget::DEFAULT_SELECTION_COLOR = hdps::Vector3f(72.f / 255, 61.f / 255, 139.f / 255);

SettingsWidget::SettingsWidget(const ScatterplotPlugin& plugin) :
    QWidget(static_cast<QWidget*>(&const_cast<ScatterplotPlugin&>(plugin))),
    _ui{ std::make_unique<Ui::SettingsWidget>() },
    _renderModeWidget(new StateWidget<RenderModeWidget>(this)),
    _plotSettinsWidget(new PlotSettingsWidget(this)),
    _dimensionSettinsWidget(),
    _subsetSettingsWidget(),
    _baseColor(DEFAULT_BASE_COLOR),
    _selectionColor(DEFAULT_SELECTION_COLOR)
{
    ////_ui->setupUi(this);
    setAutoFillBackground(true);
    auto horizontalLayout = new QHBoxLayout();

    setLayout(horizontalLayout);

    const auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    //_renderModeWidget->initialize(plugin);//, 400, fontAwesome.getIcon("cog"), "Plot settings", "Plot settings");
    _plotSettinsWidget->initialize(plugin);//, 400, fontAwesome.getIcon("cog"), "Plot settings", "Plot settings");
    //_dimensionSettinsWidget->initialize(plugin);//, 700, fontAwesome.getIcon("layer-group"), "Dimension settings", "Dimension settings");
    //_subsetSettingsWidget->initialize(plugin);//, 1100, fontAwesome.getIcon("vector-square"), "Subset settings", "Subset settings");

    _renderModeWidget->setListenWidget(&const_cast<ScatterplotPlugin&>(plugin));
    _renderModeWidget->getWidget()->initialize(plugin);

    horizontalLayout->addWidget(_renderModeWidget);
    horizontalLayout->addWidget(_plotSettinsWidget);
    horizontalLayout->addStretch(1);
    //horizontalLayout->addWidget(_dimensionSettinsWidget);
    //horizontalLayout->addWidget(_subsetSettingsWidget);

    //setEnabled(false);

    QObject::connect(&plugin, &ScatterplotPlugin::currentDatasetChanged, [this](const QString& currentDataset) {
        setEnabled(!currentDataset.isEmpty());
    });
}

SettingsWidget::~SettingsWidget()
{
}

int SettingsWidget::getXDimension()
{
    return _dimensionSettinsWidget->getDimensionX();
}

int SettingsWidget::getYDimension()
{
    return _dimensionSettinsWidget->getDimensionY();
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
    return _dimensionSettinsWidget->setDimensions(nDim);
}

void SettingsWidget::initDimOptions(const std::vector<QString>& dimNames)
{
    return _dimensionSettinsWidget->setDimensions(dimNames.size(), dimNames);
}

void SettingsWidget::initScalarDimOptions(const unsigned int nDim)
{
    return _dimensionSettinsWidget->setScalarDimensions(nDim);
}

void SettingsWidget::initScalarDimOptions(const std::vector<QString>& dimNames)
{
    return _dimensionSettinsWidget->setScalarDimensions(dimNames.size(), dimNames);
}