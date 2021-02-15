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
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, SettingsAction* settingsAction);

    private:
        QHBoxLayout     _layout;
    };

    class Spacer : public QWidget {
    public:
        enum class Type {
            Divider,
            Spacer
        };

    public:
        Spacer(const Type& type = Type::Divider);

        //static Type getType(const ResponsiveSectionWidget::State& stateBefore, const ResponsiveSectionWidget::State& stateAfter);
        //static Type getType(const ResponsiveSectionWidget* sectionBefore, const ResponsiveSectionWidget* sectionAfter);
        void setType(const Type& type);
        static std::int32_t getWidth(const Type& type);

    protected:
        Type            _type;
        QHBoxLayout*    _layout;
        QFrame*         _verticalLine;
    };

public:
    SettingsAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QWidget* getPopupWidget(QWidget* parent) override {
        return new PopupWidget(parent, this);
    };

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