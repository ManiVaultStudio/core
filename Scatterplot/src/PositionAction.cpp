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