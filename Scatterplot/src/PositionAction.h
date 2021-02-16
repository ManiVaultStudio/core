#pragma once

#include "PluginAction.h"

#include "widgets/OptionAction.h"
#include "widgets/DoubleAction.h"

#include <QHBoxLayout>
#include <QLabel>

class PositionAction : public PluginAction
{
public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, PositionAction* positionAction);

    private:
        QHBoxLayout     _layout;
        QLabel          _xDimensionLabel;
        QLabel          _yDimensionLabel;
    };

    class PopupWidget : public PluginAction::PopupWidget {
    public:
        PopupWidget(QWidget* parent, PositionAction* positionAction);
    };

public:
    PositionAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override;

    QWidget* getPopupWidget(QWidget* parent) override {
        return new PopupWidget(parent, this);
    };

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