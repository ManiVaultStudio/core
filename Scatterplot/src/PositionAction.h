#pragma once

#include "PluginAction.h"

#include "widgets/OptionAction.h"
#include "widgets/DoubleAction.h"

#include <QHBoxLayout>
#include <QGroupBox>
#include <QToolBar>
#include <QToolButton>

class PositionAction : public PluginAction
{
public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, PositionAction* positionAction);

    private:
        QHBoxLayout     _layout;
    };

public:
    PositionAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override;

    QMenu* getContextMenu();

    void setDimensions(const std::uint32_t& numberOfDimensions, const std::vector<QString>& dimensionNames = std::vector<QString>());
    void setDimensions(const std::vector<QString>& dimensionNames);

    std::int32_t getXDimension() const;
    std::int32_t getYDimension() const;

protected:
    hdps::gui::OptionAction     _xDimensionAction;
    hdps::gui::OptionAction     _yDimensionAction;
    QAction                     _resetAction;

    friend class Widget;
};