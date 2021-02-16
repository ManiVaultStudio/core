#pragma once

#include "PluginAction.h"

#include "widgets/OptionAction.h"
#include "widgets/ColorAction.h"
#include "widgets/StringAction.h"

#include <QHBoxLayout>
#include <QLabel>

class ColorDimensionAction : public PluginAction
{
public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, ColorDimensionAction* colorDimensionAction);

    private:
        QHBoxLayout     _layout;
    };

public:
    ColorDimensionAction(ScatterplotPlugin* scatterplotPlugin);

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

    void setDimensions(const std::uint32_t& numberOfDimensions, const std::vector<QString>& dimensionNames = std::vector<QString>());
    void setDimensions(const std::vector<QString>& dimensionNames);

protected:
    hdps::gui::OptionAction _colorDimensionAction;

    friend class Widget;
};