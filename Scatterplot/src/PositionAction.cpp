#include "PositionAction.h"
#include "Application.h"

#include "ScatterplotPlugin.h"

#include <QMenu>

using namespace hdps::gui;

PositionAction::PositionAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin, "Position"),
    _xDimensionAction(this, "X"),
    _yDimensionAction(this, "Y")
{
    _xDimensionAction.setToolTip("X dimension");
    _yDimensionAction.setToolTip("Y dimension");

    connect(&_xDimensionAction, &OptionAction::currentIndexChanged, [this, scatterplotPlugin](const std::uint32_t& currentIndex) {
        scatterplotPlugin->setXDimension(currentIndex);
    });

    connect(&_yDimensionAction, &OptionAction::currentIndexChanged, [this, scatterplotPlugin](const std::uint32_t& currentIndex) {
        scatterplotPlugin->setYDimension(currentIndex);
    });

    connect(&_yDimensionAction, &OptionAction::optionsChanged, [this, scatterplotPlugin](const QStringList& options) {
        _xDimensionAction.setCurrentIndex(0);
        _xDimensionAction.setDefaultIndex(0);

        const auto yIndex = options.count() >= 2 ? 1 : 0;

        _yDimensionAction.setCurrentIndex(yIndex);
        _yDimensionAction.setDefaultIndex(yIndex);
    });
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
    _layout.addWidget(new OptionAction::Widget(this, &positionAction->_xDimensionAction, false));

    _layout.addWidget(&_yDimensionLabel);
    _layout.addWidget(new OptionAction::Widget(this, &positionAction->_yDimensionAction, false));

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

    setLayout(layout);
}