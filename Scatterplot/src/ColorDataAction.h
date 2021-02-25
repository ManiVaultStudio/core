#pragma once

#include "PluginAction.h"

#include <QHBoxLayout>
#include <QLabel>

class ColorDataAction : public PluginAction
{
public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, ColorDataAction* coloringAction);

    private:
        QHBoxLayout                         _layout;
        hdps::gui::StringAction::Widget     _colorDataWidget;
    };

public:
    ColorDataAction(ScatterplotPlugin* scatterplotPlugin);

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
    hdps::gui::StringAction     _colorDataAction;
    hdps::gui::StandardAction   _resetAction;

    friend class Widget;
};