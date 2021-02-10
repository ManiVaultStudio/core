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

    _colorByActionGroup.addAction(&_colorByDimensionAction);
    _colorByActionGroup.addAction(&_colorByDataAction);

    /*
    _xDimensionAction.setToolTip("X dimension");
    _yDimensionAction.setToolTip("Y dimension");
    _resetAction.setToolTip("Reset position settings");

    const auto updateResetAction = [this, scatterplotPlugin]() {
        _resetAction.setEnabled(!scatterplotPlugin->getCurrentDataset().isEmpty() && !(_xDimensionAction.getCurrentIndex() == 0 && _yDimensionAction.getCurrentIndex() == 1));
    };

    connect(&_xDimensionAction, &OptionAction::currentIndexChanged, [this, scatterplotPlugin, updateResetAction](const std::int32_t& currentIndex) {
        scatterplotPlugin->xDimPicked(currentIndex);
        updateResetAction();
    });

    connect(&_yDimensionAction, &OptionAction::currentIndexChanged, [this, scatterplotPlugin, updateResetAction](const std::int32_t& currentIndex) {
        scatterplotPlugin->yDimPicked(currentIndex);
        updateResetAction();
    });

    connect(&_resetAction, &QAction::triggered, this, [this]() {
        _xDimensionAction.setCurrentIndex(0);
        _yDimensionAction.setCurrentIndex(1);
    });

    updateResetAction();
    */

    connect(&_colorByDimensionAction, &QAction::triggered, this, [this]() {
        //_xDimensionAction.setCurrentIndex(0);
        //_yDimensionAction.setCurrentIndex(1);
    });
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

ColorAction::Widget::Widget(QWidget* parent, ColorAction* colorAction) :
    WidgetAction::Widget(parent, colorAction),
    _layout()
{
    _layout.addWidget(colorAction->_colorByAction.createWidget(this));

    setLayout(&_layout);
}