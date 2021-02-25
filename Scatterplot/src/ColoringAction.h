#pragma once

#include "PluginAction.h"

#include "ConstantColorAction.h"
#include "ColorDimensionAction.h"
#include "ColorDataAction.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QStackedWidget>

class ColoringAction : public PluginAction
{
public:
    class Widget : public PluginAction::Widget {
    public:
        class StackedWidget : public QStackedWidget {
        public:
            QSize sizeHint() const override { return currentWidget()->sizeHint(); }
            QSize minimumSizeHint() const override { return currentWidget()->minimumSizeHint(); }
        };

    public:
        Widget(QWidget* parent, ColoringAction* coloringAction);

    private:
        QHBoxLayout                         _layout;
        QLabel                              _colorByLabel;
        hdps::gui::OptionAction::Widget     _colorByWidget;
        StackedWidget                       _stackedWidget;
        ConstantColorAction::Widget         _constantColorWidget;
        ColorDimensionAction::Widget        _colorDimensionWidget;
        ColorDataAction::Widget             _colorDataWidget;
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
    hdps::gui::StandardAction   _colorByConstantColorAction;
    hdps::gui::StandardAction   _colorByDimensionAction;
    hdps::gui::StandardAction   _colorByColorDataAction;
    QActionGroup                _colorByActionGroup;
    ConstantColorAction         _constantColorAction;
    ColorDimensionAction        _colorDimensionAction;
    ColorDataAction             _colorDataAction;

    friend class Widget;
};