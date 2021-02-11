#include "ColoringAction.h"
#include "Application.h"

#include "ScatterplotPlugin.h"

using namespace hdps::gui;

ColoringAction::ColoringAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin),
    _colorByAction(this, "Color by"),
    _colorByConstantColorAction("Color by constant color"),
    _colorByDimensionAction("Color by dimension"),
    _colorByColorDataAction("Color by color data"),
    _colorByActionGroup(this),
    _constantColorAction(this, "Constant color"),
    _dimensionAction(this, "Color dimension"),
    _colorDataAction(this, "Color data"),
    _removeColorDataAction("Remove"),
    _resetAction("Reset")
{
    scatterplotPlugin->addAction(&_colorByAction);
    scatterplotPlugin->addAction(&_colorByDimensionAction);
    scatterplotPlugin->addAction(&_colorByColorDataAction);
    scatterplotPlugin->addAction(&_dimensionAction);
    scatterplotPlugin->addAction(&_colorDataAction);
    scatterplotPlugin->addAction(&_removeColorDataAction);
    scatterplotPlugin->addAction(&_resetAction);

    _colorByAction.setOptions(QStringList() << "Constant color" << "Dimension" << "Color data");

    _colorByAction.setToolTip("Color by");
    _colorByConstantColorAction.setToolTip("Color data points with a constant color");
    _colorByDimensionAction.setToolTip("Color data points by chosen dimension");
    _colorByColorDataAction.setToolTip("Color data points with a color data set");
    _constantColorAction.setToolTip("Constant color");
    _dimensionAction.setToolTip("Color dimension");
    _colorDataAction.setToolTip("Color data");
    _removeColorDataAction.setToolTip("Remove color data");
    _resetAction.setToolTip("Reset color settings");

    _colorByConstantColorAction.setCheckable(true);
    _colorByDimensionAction.setCheckable(true);
    _colorByColorDataAction.setCheckable(true);

    _colorDataAction.setEnabled(false);

    _colorByActionGroup.addAction(&_colorByConstantColorAction);
    _colorByActionGroup.addAction(&_colorByDimensionAction);
    _colorByActionGroup.addAction(&_colorByColorDataAction);

    connect(&_dimensionAction, &OptionAction::currentIndexChanged, this, [this](const std::uint32_t& currentIndex) {
        _scatterplotPlugin->setColorDimension(currentIndex);
    });

    const auto renderModeChanged = [this]() -> void {
        setEnabled(getScatterplotWidget()->getRenderMode() == ScatterplotWidget::SCATTERPLOT);
    };

    connect(getScatterplotWidget(), &ScatterplotWidget::renderModeChanged, this, [this, renderModeChanged](const ScatterplotWidget::RenderMode& renderMode) {
        renderModeChanged();
    });

    connect(&_colorByAction, &OptionAction::currentIndexChanged, this, [this](const std::uint32_t& currentIndex) {
        getScatterplotWidget()->setColoringMode(static_cast<ScatterplotWidget::ColoringMode>(currentIndex));
    });

    connect(&_colorByConstantColorAction, &QAction::triggered, this, [this]() {
        getScatterplotWidget()->setColoringMode(ScatterplotWidget::ColoringMode::ConstantColor);
    });

    connect(&_colorByDimensionAction, &QAction::triggered, this, [this]() {
        getScatterplotWidget()->setColoringMode(ScatterplotWidget::ColoringMode::Dimension);
    });

    connect(&_colorByColorDataAction, &QAction::triggered, this, [this]() {
        getScatterplotWidget()->setColoringMode(ScatterplotWidget::ColoringMode::ColorData);
    });

    const auto coloringModeChanged = [this]() -> void {
        const auto coloringMode = getScatterplotWidget()->getColoringMode();

        _colorByAction.setCurrentIndex(static_cast<std::int32_t>(coloringMode));

        _colorByConstantColorAction.setChecked(coloringMode == ScatterplotWidget::ColoringMode::ConstantColor);
        _colorByDimensionAction.setChecked(coloringMode == ScatterplotWidget::ColoringMode::Dimension);
        _colorByColorDataAction.setChecked(coloringMode == ScatterplotWidget::ColoringMode::ColorData);
    };

    connect(getScatterplotWidget(), &ScatterplotWidget::coloringModeChanged, this, [this, coloringModeChanged](const ScatterplotWidget::ColoringMode& coloringMode) {
        coloringModeChanged();
    });

    renderModeChanged();
    coloringModeChanged();
}

QMenu* ColoringAction::getContextMenu()
{
    auto menu = new QMenu("Color");

    const auto addActionToMenu = [menu](QAction* action) -> void {
        auto actionMenu = new QMenu(action->text());

        actionMenu->addAction(action);

        menu->addMenu(actionMenu);
    };

    menu->setEnabled(_scatterplotPlugin->getScatterplotWidget()->getRenderMode() == ScatterplotWidget::RenderMode::SCATTERPLOT);

    menu->addAction(&_colorByConstantColorAction);
    menu->addAction(&_colorByDimensionAction);
    menu->addAction(&_colorByColorDataAction);
    
    menu->addSeparator();

    /*
    switch (renderMode)
    {
        case ScatterplotWidget::RenderMode::SCATTERPLOT:
        {
            break;
        }

        case ScatterplotWidget::RenderMode::DENSITY:
        {
            break;
        }

        case ScatterplotWidget::RenderMode::SCATTERPLOT:
        {
            break;
        }

        default:
            break;
    }
    */

    addActionToMenu(&_constantColorAction);
    addActionToMenu(&_dimensionAction);
    addActionToMenu(&_colorDataAction);



    /*
    auto xDimensionMenu = new QMenu("X dimension");
    auto yDimensionMenu = new QMenu("Y dimension");

    xDimensionMenu->addAction(&_xDimensionAction);
    yDimensionMenu->addAction(&_yDimensionAction);

    menu->addMenu(xDimensionMenu);
    menu->addMenu(yDimensionMenu);
    menu->addSeparator();
    menu->addAction(&_resetAction);
    */

    return menu;
}

void ColoringAction::setDimensions(const std::uint32_t& numberOfDimensions, const std::vector<QString>& dimensionNames /*= std::vector<QString>()*/)
{
    _dimensionAction.setOptions(common::getDimensionNamesStringList(numberOfDimensions, dimensionNames));
}

void ColoringAction::setDimensions(const std::vector<QString>& dimensionNames)
{
    setDimensions(dimensionNames.size(), dimensionNames);
}

ColoringAction::Widget::Widget(QWidget* parent, ColoringAction* coloringAction) :
    WidgetAction::Widget(parent, coloringAction),
    _layout(),
    _colorByLabel("Color by:")
{
    _layout.addWidget(&_colorByLabel);
    _layout.addWidget(coloringAction->_colorByAction.createWidget(this));
    _layout.addWidget(coloringAction->_constantColorAction.createWidget(this));
    _layout.addWidget(coloringAction->_dimensionAction.createWidget(this));
    _layout.addWidget(coloringAction->_colorDataAction.createWidget(this));

    setLayout(&_layout);

    const auto coloringModeChanged = [this, coloringAction]() -> void {
        const auto coloringMode = static_cast<ScatterplotWidget::ColoringMode>(coloringAction->_colorByAction.getCurrentIndex());

        //_colorByConstantColorAction.setVisible(coloringMode == ScatterplotWidget::ColoringMode::ConstantColor);
        coloringAction->_dimensionAction.setVisible(coloringMode == ScatterplotWidget::ColoringMode::Dimension);
        coloringAction->_colorDataAction.setVisible(coloringMode == ScatterplotWidget::ColoringMode::ColorData);
    };

    connect(&coloringAction->_colorByAction, &OptionAction::currentIndexChanged, this, [this, coloringModeChanged](const std::uint32_t& currentIndex) {
        coloringModeChanged();
    });

    coloringModeChanged();
}