#pragma once

#include "widgets/WidgetAction.h"
#include "widgets/OptionAction.h"

#include <QHBoxLayout>
#include <QGroupBox>
#include <QToolBar>
#include <QToolButton>

class ScatterplotPlugin;

class PositionAction : public hdps::gui::WidgetAction
{
    Q_OBJECT

public:
    class Widget : public hdps::gui::WidgetAction::Widget {
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

    friend class Widget;
};