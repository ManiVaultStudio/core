#include "ColorAction.h"
#include "Application.h"

#include "ScatterplotPlugin.h"

using namespace hdps::gui;

ColorAction::ColorAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin),
    _colorByAction(this, "Color by"),
    _colorByDimensionAction("Color by dimension"),
    _colorByDataAction("Color by data"),
    _colorDimensionAction(this, "Color dimension"),
    _colorDataAction(this, "Color data"),
    _removeColorDataAction("Remove"),
    _resetAction("Reset")
{
    _colorByAction.setToolTip("Color by");
    _colorDimensionAction.setToolTip("Color dimension");
    _colorDataAction.setToolTip("Color data");
    _removeColorDataAction.setToolTip("Remove color data");
    _resetAction.setToolTip("Reset color settings");

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

    menu->addAction(&_colorByDimensionAction);
    menu->addAction(&_colorByDataAction);
    
    menu->addSeparator();

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
    _layout(),
    _toolBar(),
    _toolButton(),
    _popupWidget(this, "Color"),
    _popupWidgetAction(this)
{
    _layout.addWidget(&_toolBar);

    /*
    _toolBar.addAction(&colorAction->_xDimensionAction);
    _toolBar.addAction(&colorAction->_yDimensionAction);
    */

    _popupWidgetAction.setDefaultWidget(&_popupWidget);

    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.addAction(&_popupWidgetAction);

    setLayout(&_layout);
}