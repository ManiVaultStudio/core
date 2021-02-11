#include "PositionAction.h"
#include "Application.h"

#include "ScatterplotPlugin.h"

using namespace hdps::gui;

PositionAction::PositionAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin),
    _xDimensionAction(this, "X"),
    _yDimensionAction(this, "Y"),
    _resetAction("Reset")
{
    _xDimensionAction.setToolTip("X dimension");
    _yDimensionAction.setToolTip("Y dimension");
    _resetAction.setToolTip("Reset position settings");

    const auto updateResetAction = [this, scatterplotPlugin]() {
        _resetAction.setEnabled(!scatterplotPlugin->getCurrentDataset().isEmpty() && !(_xDimensionAction.getCurrentIndex() == 0 && _yDimensionAction.getCurrentIndex() == 1));
    };

    connect(&_xDimensionAction, &OptionAction::currentIndexChanged, [this, scatterplotPlugin, updateResetAction](const std::uint32_t& currentIndex) {
        scatterplotPlugin->setXDimension(currentIndex);
        updateResetAction();
    });

    connect(&_yDimensionAction, &OptionAction::currentIndexChanged, [this, scatterplotPlugin, updateResetAction](const std::uint32_t& currentIndex) {
        scatterplotPlugin->setYDimension(currentIndex);
        updateResetAction();
    });

    connect(&_resetAction, &QAction::triggered, this, [this]() {
        _xDimensionAction.setCurrentIndex(0);
        _yDimensionAction.setCurrentIndex(1);
    });

    updateResetAction();
}

QWidget* PositionAction::createWidget(QWidget* parent)
{
    return new Widget(parent, this);
}

QMenu* PositionAction::getContextMenu()
{
    auto menu = new QMenu("Position");

    auto xDimensionMenu = new QMenu("X dimension");
    auto yDimensionMenu = new QMenu("Y dimension");

    xDimensionMenu->addAction(&_xDimensionAction);
    yDimensionMenu->addAction(&_yDimensionAction);

    menu->addMenu(xDimensionMenu);
    menu->addMenu(yDimensionMenu);
    menu->addSeparator();
    menu->addAction(&_resetAction);

    return menu;
}

void PositionAction::setDimensions(const std::uint32_t& numberOfDimensions, const std::vector<QString>& dimensionNames /*= std::vector<QString>()*/)
{
    auto dimensionNamesStringList = common::getDimensionNamesStringList(numberOfDimensions, dimensionNames);

    _xDimensionAction.setOptions(dimensionNamesStringList);
    _yDimensionAction.setOptions(dimensionNamesStringList);

    _xDimensionAction.setCurrentIndex(0);
    _yDimensionAction.setCurrentIndex(numberOfDimensions >= 2 ? 1 : 0);
}

void PositionAction::setDimensions(const std::vector<QString>& dimensionNames)
{
    setDimensions(dimensionNames.size(), dimensionNames);
}

std::int32_t PositionAction::getXDimension() const
{
    return _xDimensionAction.getCurrentIndex();
}

std::int32_t PositionAction::getYDimension() const
{
    return _yDimensionAction.getCurrentIndex();
}

PositionAction::Widget::Widget(QWidget* parent, PositionAction* positionAction) :
    WidgetAction::Widget(parent, positionAction),
    _layout()
{
    _layout.addWidget(positionAction->_xDimensionAction.createWidget(this));
    _layout.addWidget(positionAction->_yDimensionAction.createWidget(this));

    setLayout(&_layout);
}