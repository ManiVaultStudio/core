#pragma once

#include "PluginAction.h"

#include "RenderModeAction.h"
#include "PlotAction.h"
#include "PositionAction.h"
#include "ColoringAction.h"
#include "SubsetAction.h"
#include "SelectionAction.h"

class ScatterplotPlugin;

class SettingsAction : public PluginAction
{
public:
    class SpacerWidget : public QWidget {
    public:
        enum class Type {
            Divider,
            Spacer
        };

    public:
        SpacerWidget(const Type& type = Type::Divider);

        static Type getType(const WidgetAction::WidgetType& widgetTypeLeft, const WidgetAction::WidgetType& widgetTypeRight);
        static Type getType(const WidgetAction::StateWidget* stateWidgetLeft, const WidgetAction::StateWidget* stateWidgetRight);

        void setType(const Type& type);
        static std::int32_t getWidth(const Type& type);

    protected:
        Type            _type;
        QHBoxLayout*    _layout;
        QFrame*         _verticalLine;
    };

    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, SettingsAction* settingsAction);

        bool eventFilter(QObject* object, QEvent* event);

    protected:
        void addStateWidget(WidgetAction* widgetAction, const std::int32_t& priority = 0);

    private:
        void updateLayout();

    private:
        QHBoxLayout             _layout;
        QWidget                 _toolBarWidget;
        QHBoxLayout             _toolBarLayout;
        QVector<StateWidget*>   _stateWidgets;
        QVector<SpacerWidget*>  _spacerWidgets;
    };

public:
    SettingsAction(ScatterplotPlugin* scatterplotPlugin);

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

    RenderModeAction& getRenderModeAction() { return _renderModeAction; }
    PlotAction& getPlotAction() { return _plotAction; }
    PositionAction& getPositionAction() { return _positionAction; }
    ColoringAction& getColoringAction() { return _coloringAction; }
    SubsetAction& getSubsetAction() { return _subsetAction; }
    SelectionAction& getSelectionAction() { return _selectionAction; }

protected:
    RenderModeAction    _renderModeAction;
    PlotAction          _plotAction;
    PositionAction      _positionAction;
    ColoringAction      _coloringAction;
    SubsetAction        _subsetAction;
    SelectionAction     _selectionAction;
};