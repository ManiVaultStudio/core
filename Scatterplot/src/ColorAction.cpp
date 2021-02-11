#include "ColorAction.h"
#include "Application.h"

#include "ScatterplotPlugin.h"

using namespace hdps::gui;

ColorAction::ColorAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin),
    _colorByAction(this, "Color by"),
    _colorByDimensionAction("Color by dimension"),
    _colorByDataAction("Color by data"),
    _colorByActionGroup(this),
    _colorDimensionAction(this, "Color dimension"),
    _colorDataAction(this, "Color data"),
    _removeColorDataAction("Remove"),
    _resetAction("Reset")
{
    scatterplotPlugin->addAction(&_colorByAction);
    scatterplotPlugin->addAction(&_colorByDimensionAction);
    scatterplotPlugin->addAction(&_colorByDataAction);
    scatterplotPlugin->addAction(&_colorDimensionAction);
    scatterplotPlugin->addAction(&_colorDataAction);
    scatterplotPlugin->addAction(&_removeColorDataAction);
    scatterplotPlugin->addAction(&_resetAction);

    _colorByAction.setOptions(QStringList() << "By dimension" << "By data");

    _colorByAction.setToolTip("Color by");
    _colorDimensionAction.setToolTip("Color dimension");
    _colorDataAction.setToolTip("Color data");
    _removeColorDataAction.setToolTip("Remove color data");
    _resetAction.setToolTip("Reset color settings");

    _colorByDimensionAction.setCheckable(true);
    _colorByDataAction.setCheckable(true);

    _colorDataAction.setEnabled(false);

    _colorByActionGroup.addAction(&_colorByDimensionAction);
    _colorByActionGroup.addAction(&_colorByDataAction);

    connect(&_colorDimensionAction, &OptionAction::currentIndexChanged, this, [this](const std::uint32_t& currentIndex) {
        _scatterplotPlugin->setColorDimension(currentIndex);
    });

    const auto renderModeChanged = [this]() -> void {
        setEnabled(getScatterplotWidget()->getRenderMode() == ScatterplotWidget::SCATTERPLOT);
    };

    connect(getScatterplotWidget(), &ScatterplotWidget::renderModeChanged, this, [this, renderModeChanged](const ScatterplotWidget::RenderMode& renderMode) {
        renderModeChanged();
    });

    renderModeChanged();
}

QMenu* ColorAction::getContextMenu()
{
    auto menu = new QMenu("Color");

    const auto addActionToMenu = [menu](QAction* action) -> void {
        auto actionMenu = new QMenu(action->text());

        actionMenu->addAction(action);

        menu->addMenu(actionMenu);
    };

    menu->setEnabled(_scatterplotPlugin->getScatterplotWidget()->getRenderMode() == ScatterplotWidget::RenderMode::SCATTERPLOT);

    menu->addAction(&_colorByDimensionAction);
    menu->addAction(&_colorByDataAction);
    
    menu->addSeparator();

    addActionToMenu(&_colorDimensionAction);
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

void ColorAction::setDimensions(const std::uint32_t& numberOfDimensions, const std::vector<QString>& dimensionNames /*= std::vector<QString>()*/)
{
    _colorDimensionAction.setOptions(common::getDimensionNamesStringList(numberOfDimensions, dimensionNames));
}

void ColorAction::setDimensions(const std::vector<QString>& dimensionNames)
{
    setDimensions(dimensionNames.size(), dimensionNames);
}

ColorAction::Widget::Widget(QWidget* parent, ColorAction* colorAction) :
    WidgetAction::Widget(parent, colorAction),
    _layout()
{
    _layout.addWidget(colorAction->_colorByAction.createWidget(this));
    _layout.addWidget(colorAction->_colorDimensionAction.createWidget(this));
    _layout.addWidget(colorAction->_colorDataAction.createWidget(this));

    setLayout(&_layout);
}