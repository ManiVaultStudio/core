#include "PositionAction.h"
#include "Application.h"

#include "ScatterplotPlugin.h"

using namespace hdps::gui;

PositionAction::PositionAction(ScatterplotPlugin* scatterplotPlugin) :
    WidgetAction(reinterpret_cast<QObject*>(scatterplotPlugin)),
    _xDimensionAction(this, "X"),
    _yDimensionAction(this, "Y")
{
    _xDimensionAction.setToolTip("X dimension");
    _yDimensionAction.setToolTip("Y dimension");

    connect(&_xDimensionAction, &OptionAction::currentIndexChanged, [this, scatterplotPlugin](const std::int32_t& currentIndex) {
        scatterplotPlugin->xDimPicked(currentIndex);
    });

    connect(&_yDimensionAction, &OptionAction::currentIndexChanged, [this, scatterplotPlugin](const std::int32_t& currentIndex) {
        scatterplotPlugin->yDimPicked(currentIndex);
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
    auto dimensionNamesStringList = getDimensionNamesStringList(numberOfDimensions, dimensionNames);

    QSignalBlocker xDimensionActionSignalBlocker(&_xDimensionAction), yDimensionActionSignalBlocker(&_yDimensionAction);

    _xDimensionAction.setOptions(dimensionNamesStringList);
    _yDimensionAction.setOptions(dimensionNamesStringList);

    if (numberOfDimensions >= 2) {
        _xDimensionAction.setCurrentIndex(0);
        _yDimensionAction.setCurrentIndex(1);
    }
}

std::int32_t PositionAction::getXDimension() const
{
    return _xDimensionAction.getCurrentIndex();
}

std::int32_t PositionAction::getYDimension() const
{
    return _yDimensionAction.getCurrentIndex();
}

QStringList PositionAction::getDimensionNamesStringList(const std::uint32_t& numberOfDimensions, const std::vector<QString>& dimensionNames /*= std::vector<QString>()*/) const
{
    QStringList dimensionNamesStringList;

    dimensionNamesStringList.reserve(static_cast<int>(numberOfDimensions));

    if (numberOfDimensions == dimensionNames.size()) {
        for (const auto& name : dimensionNames)
            dimensionNamesStringList.append(name);
    } else {
        for (unsigned int i = 0; i < numberOfDimensions; ++i)
            dimensionNamesStringList.append(QString::fromLatin1("Dim %1").arg(i));
    }

    return dimensionNamesStringList;
}

PositionAction::Widget::Widget(QWidget* parent, PositionAction* positionAction) :
    WidgetAction::Widget(parent, positionAction),
    _layout(),
    _toolBar(),
    _toolButton(),
    _popupWidget(this, "Position"),
    _popupWidgetAction(this)
{
    _layout.addWidget(&_toolBar);

    _toolBar.addAction(&positionAction->_xDimensionAction);
    _toolBar.addAction(&positionAction->_yDimensionAction);

    _popupWidgetAction.setDefaultWidget(&_popupWidget);

    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.addAction(&_popupWidgetAction);

    setLayout(&_layout);
}