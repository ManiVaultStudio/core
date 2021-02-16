#pragma once

#include "PluginAction.h"

#include "widgets/ColorAction.h"

#include <QHBoxLayout>
#include <QLabel>

class ConstantColorAction : public PluginAction
{
public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, ConstantColorAction* colorByConstantAction);

    private:
        QHBoxLayout     _layout;
    };

public:
    ConstantColorAction(ScatterplotPlugin* scatterplotPlugin);

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
    hdps::gui::ColorAction  _constantColorAction;
    QAction                 _resetAction;

    static const QColor DEFAULT_COLOR;

    friend class Widget;
};