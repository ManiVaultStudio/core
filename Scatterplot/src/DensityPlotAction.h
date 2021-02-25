#pragma once

#include "PluginAction.h"

#include <QLabel>

class DensityPlotAction : public PluginAction
{
public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, DensityPlotAction* densityPlotAction);

    private:
        QHBoxLayout     _layout;
        QLabel          _sigmaLabel;
    };

    class PopupWidget : public PluginAction::PopupWidget {
    public:
        PopupWidget(QWidget* parent, DensityPlotAction* densityPlotAction);
    };

public:
    DensityPlotAction(ScatterplotPlugin* scatterplotPlugin);

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

    bool canReset() const;
    void reset();

protected:
    hdps::gui::DoubleAction     _sigmaAction;
    hdps::gui::StandardAction   _resetAction;

    static constexpr double DEFAULT_SIGMA = 25.0;

    friend class Widget;
};