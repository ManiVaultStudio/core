#pragma once

#include "PluginAction.h"

#include <QLabel>

class PointPlotAction : public PluginAction
{
public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, PointPlotAction* pointPlotAction);

    private:
        QHBoxLayout     _layout;
        QLabel          _pointSizelabel;
        QLabel          _pointOpacitylabel;
    };

    class PopupWidget : public PluginAction::PopupWidget {
    public:
        PopupWidget(QWidget* parent, PointPlotAction* pointPlotAction);
    };

public:
    PointPlotAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent, const WidgetType& widgetType = WidgetType::Standard) override {
        switch (widgetType)
        {
            case WidgetType::Standard:
                return new Widget(parent, this);

            case WidgetType::Compact:
                return new CompactWidget(parent, this);

            case WidgetType::Popup:
                return new PopupWidget(parent, this);

            default:
                break;
        }

        return nullptr;
    }

    QMenu* getContextMenu();

protected:
    hdps::gui::DecimalAction    _pointSizeAction;
    hdps::gui::DecimalAction    _pointOpacityAction;

    static constexpr double DEFAULT_POINT_SIZE = 10.0;
    static constexpr double DEFAULT_POINT_OPACITY = 50.0;

    friend class Widget;
};