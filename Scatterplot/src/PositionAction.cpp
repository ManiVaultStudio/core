#include "PositionAction.h"
#include "Application.h"

#include "ScatterplotPlugin.h"

using namespace hdps::gui;

PositionAction::PositionAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin, "Position"),
    _xDimensionAction(this, "X"),
    _yDimensionAction(this, "Y"),
    _resetAction(this, "Reset")
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
    setDimensions(static_cast<std::uint32_t>(dimensionNames.size()), dimensionNames);
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
    _layout(),
    _xDimensionLabel(QString("%1:").arg(positionAction->_xDimensionAction.text())),
    _yDimensionLabel(QString("%1:").arg(positionAction->_yDimensionAction.text()))
{
    _xDimensionLabel.setToolTip(positionAction->_xDimensionAction.toolTip());
    _yDimensionLabel.setToolTip(positionAction->_yDimensionAction.toolTip());

    _layout.addWidget(&_xDimensionLabel);
    _layout.addWidget(new OptionAction::Widget(this, &positionAction->_xDimensionAction));

    _layout.addWidget(&_yDimensionLabel);
    _layout.addWidget(new OptionAction::Widget(this, &positionAction->_yDimensionAction));

    setLayout(&_layout);
}

PositionAction::PopupWidget::PopupWidget(QWidget* parent, PositionAction* positionAction):
    WidgetAction::PopupWidget(parent, positionAction)
{
    auto layout = new QGridLayout();

    layout->addWidget(new QLabel("X-dimension:"), 0, 0);
    layout->addWidget(new OptionAction::Widget(this, &positionAction->_xDimensionAction), 0, 1);

    layout->addWidget(new QLabel("Y-dimension:"), 1, 0);
    layout->addWidget(new OptionAction::Widget(this, &positionAction->_yDimensionAction), 1, 1);
    
    layout->addWidget(new StandardAction::PushButton(this, &positionAction->_resetAction), 2, 1);

    setLayout(layout);
}