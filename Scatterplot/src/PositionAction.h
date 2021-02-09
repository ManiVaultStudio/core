#pragma once

#include "PluginAction.h"

#include "widgets/OptionAction.h"

#include <QHBoxLayout>
#include <QGroupBox>
#include <QToolBar>
#include <QToolButton>

class PositionAction : public PluginAction
{
    Q_OBJECT

public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, PositionAction* positionAction);

    private:
        QHBoxLayout         _layout;
        QToolBar            _toolBar;
        QToolButton         _toolButton;
        PopupWidget         _popupWidget;
        QWidgetAction       _popupWidgetAction;
    };

public:
    PositionAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

    void setDimensions(const std::uint32_t& numberOfDimensions, const std::vector<QString>& dimensionNames = std::vector<QString>());

    std::int32_t getXDimension() const;
    std::int32_t getYDimension() const;

private:
    QStringList getDimensionNamesStringList(const std::uint32_t& numberOfDimensions, const std::vector<QString>& dimensionNames) const;

protected:
    hdps::gui::OptionAction     _xDimensionAction;
    hdps::gui::OptionAction     _yDimensionAction;
    QAction                     _resetAction;

    friend class Widget;
};