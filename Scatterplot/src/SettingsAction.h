#pragma once

#include "PluginAction.h"

#include "RenderModeAction.h"
#include "PlotAction.h"
#include "PositionAction.h"
#include "ColorAction.h"
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

public:
    SettingsAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

    RenderModeAction& getRenderModeAction() { return _renderModeAction; }
    PlotAction& getPlotAction() { return _plotAction; }
    PositionAction& getPositionAction() { return _positionAction; }
    ColorAction& getColorAction() { return _colorAction; }
    SubsetAction& getSubsetAction() { return _subsetAction; }
    SelectionAction& getSelectionAction() { return _selectionAction; }

protected:
    RenderModeAction    _renderModeAction;
    PlotAction          _plotAction;
    PositionAction      _positionAction;
    ColorAction         _colorAction;
    SubsetAction        _subsetAction;
    SelectionAction     _selectionAction;
};