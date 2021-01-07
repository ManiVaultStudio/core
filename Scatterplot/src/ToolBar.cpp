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
    _renderModeWidget(new hdps::gui::ResponsiveWidget<RenderModeWidget>(this)),
    _plotSettinsWidget(new PlotSettingsWidget(this)),
    _dimensionSettinsWidget(new DimensionSettingsWidget(this)),
    _subsetSettingsWidget(new SubsetSettingsWidget(this)),
    _baseColor(DEFAULT_BASE_COLOR),
    _selectionColor(DEFAULT_SELECTION_COLOR)
{
    ////_ui->setupUi(this);

    auto horizontalLayout = new QHBoxLayout();

    setLayout(horizontalLayout);

    const auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _renderModeWidget->setSourceWidget(static_cast<QWidget*>(&const_cast<ScatterplotPlugin&>(plugin)));// , 1200, fontAwesome.getIcon("eye"), "Render mode", "Render mode settings");
    _plotSettinsWidget->initialize(plugin);//, 400, fontAwesome.getIcon("cog"), "Plot settings", "Plot settings");
    _dimensionSettinsWidget->initialize(plugin);//, 700, fontAwesome.getIcon("layer-group"), "Dimension settings", "Dimension settings");
    _subsetSettingsWidget->initialize(plugin);//, 1100, fontAwesome.getIcon("vector-square"), "Subset settings", "Subset settings");

    _renderModeWidget->setCallback([this](const QSize& size, RenderModeWidget& renderModeWidget) {
        const auto width        = size.width();
        const auto state    = width < 300 ? RenderModeWidget::State::PopupButton : width < 600 ? RenderModeWidget::State::IconsOnly : RenderModeWidget::State::Full;

        switch (state)
        {
            case RenderModeWidget::State::PopupButton:
            {
                renderModeWidget.getPopupPushButton().setVisible(true);
                renderModeWidget.getScatterPlotPushButton().setVisible(false);
                renderModeWidget.getDensityPlotPushButton().setVisible(false);
                renderModeWidget.getContourPlotPushButton().setVisible(false);

                break;
            }

            case RenderModeWidget::State::IconsOnly:
            {
                renderModeWidget.getPopupPushButton().setVisible(false);
                
                renderModeWidget.getScatterPlotPushButton().setText(false);
                renderModeWidget.getDensityPlotPushButton().setText(false);
                renderModeWidget.getContourPlotPushButton().setText(false);

                renderModeWidget.getScatterPlotPushButton().setVisible(false);
                renderModeWidget.getDensityPlotPushButton().setVisible(false);
                renderModeWidget.getContourPlotPushButton().setVisible(false);

                break;
            }

            default:
                break;
        }
        


        /*
        renderModeWidget.getScatterPlotPushButton().setFixedWidth(buttonWidth);
        renderModeWidget.getScatterPlotPushButton().setFixedWidth(buttonWidth);
        renderModeWidget.getScatterPlotPushButton().setFixedWidth(buttonWidth);

        
        renderModeWidget.getScatterPlotPushButton().setVisible(buttonWidth);
        renderModeWidget.getScatterPlotPushButton().setVisible(buttonWidth);
        renderModeWidget.getScatterPlotPushButton().setVisible(buttonWidth);
        */
    });

    horizontalLayout->addWidget(_renderModeWidget);
    horizontalLayout->addWidget(_plotSettinsWidget);
    horizontalLayout->addWidget(_dimensionSettinsWidget);
    horizontalLayout->addWidget(_subsetSettingsWidget);

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