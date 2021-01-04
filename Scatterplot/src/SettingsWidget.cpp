#include "SettingsWidget.h"
#include "ScatterplotPlugin.h"

#include "RenderModeWidget.h"
#include "PlotSettingsWidget.h"
#include "DimensionSettingsWidget.h"
#include "SubsetSettingsWidget.h"

#include <QPainter>
#include <QStyleOption>

#include "ui_SettingsWidget.h"

const hdps::Vector3f SettingsWidget::DEFAULT_BASE_COLOR = hdps::Vector3f(255.f / 255, 99.f / 255, 71.f / 255);
const hdps::Vector3f SettingsWidget::DEFAULT_SELECTION_COLOR = hdps::Vector3f(72.f / 255, 61.f / 255, 139.f / 255);

SettingsWidget::SettingsWidget(const ScatterplotPlugin& plugin) :
    QWidget(static_cast<QWidget*>(&const_cast<ScatterplotPlugin&>(plugin))),
    _ui{ std::make_unique<Ui::SettingsWidget>() },
    _renderModeWidget(new ResponsiveSectionWidget<RenderModeWidget>(this)),
    _plotSettinsWidget(new ResponsiveSectionWidget<PlotSettingsWidget>(this)),
    _dimensionSettinsWidget(new ResponsiveSectionWidget<DimensionSettingsWidget>(this)),
    _subsetSettingsWidget(new ResponsiveSectionWidget<SubsetSettingsWidget>(this)),
    _baseColor(DEFAULT_BASE_COLOR),
    _selectionColor(DEFAULT_SELECTION_COLOR)
{
    _ui->setupUi(this);

    const auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _renderModeWidget->initialize(plugin, 1200, fontAwesome.getIcon("eye"), "Render mode", "Render mode settings");
    _plotSettinsWidget->initialize(plugin, 400, fontAwesome.getIcon("cog"), "Plot settings", "Plot settings");
    _dimensionSettinsWidget->initialize(plugin, 700, fontAwesome.getIcon("layer-group"), "Dimension settings", "Dimension settings");
    _subsetSettingsWidget->initialize(plugin, 1100, fontAwesome.getIcon("vector-square"), "Subset settings", "Subset settings");

    _ui->horizontalLayout->addWidget(_renderModeWidget);
    _ui->horizontalLayout->addWidget(_plotSettinsWidget);
    _ui->horizontalLayout->addWidget(_dimensionSettinsWidget);
    _ui->horizontalLayout->addWidget(_subsetSettingsWidget);

    setEnabled(false);

    QObject::connect(&plugin, &ScatterplotPlugin::currentDatasetChanged, [this](const QString& currentDataset) {
        setEnabled(!currentDataset.isEmpty());
    });
}

SettingsWidget::~SettingsWidget()
{
}

int SettingsWidget::getXDimension()
{
    return _dimensionSettinsWidget->getSectionWidget()->getDimensionX();
}

int SettingsWidget::getYDimension()
{
    return _dimensionSettinsWidget->getSectionWidget()->getDimensionY();
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
    return _dimensionSettinsWidget->getSectionWidget()->setDimensions(nDim);
}

void SettingsWidget::initDimOptions(const std::vector<QString>& dimNames)
{
    return _dimensionSettinsWidget->getSectionWidget()->setDimensions(dimNames.size(), dimNames);
}

void SettingsWidget::initScalarDimOptions(const unsigned int nDim)
{
    return _dimensionSettinsWidget->getSectionWidget()->setScalarDimensions(nDim);
}

void SettingsWidget::initScalarDimOptions(const std::vector<QString>& dimNames)
{
    return _dimensionSettinsWidget->getSectionWidget()->setScalarDimensions(dimNames.size(), dimNames);
}

void SettingsWidget::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}