#pragma once

#include "PluginAction.h"

#include "widgets/OptionAction.h"
#include "widgets/ColorAction.h"
#include "widgets/StringAction.h"

#include <QHBoxLayout>
#include <QLabel>

class ColoringAction : public PluginAction
{
public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, ColoringAction* coloringAction);

    private:
        QHBoxLayout     _layout;
        QLabel          _colorByLabel;
    };

    class PopupWidget : public PluginAction::PopupWidget {
    public:
        PopupWidget(QWidget* parent, ColoringAction* coloringAction);
    };

public:
    ColoringAction(ScatterplotPlugin* scatterplotPlugin);

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
    hdps::gui::OptionAction     _colorByAction;
    QAction                     _colorByConstantColorAction;
    QAction                     _colorByDimensionAction;
    QAction                     _colorByColorDataAction;
    QActionGroup                _colorByActionGroup;
    hdps::gui::ColorAction      _constantColorAction;
    hdps::gui::OptionAction     _dimensionAction;
    hdps::gui::StringAction     _colorDataAction;
    QAction                     _removeColorDataAction;
    QAction                     _resetAction;

    friend class Widget;
};