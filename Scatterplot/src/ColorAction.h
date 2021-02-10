#pragma once

#include "PluginAction.h"

#include "widgets/OptionAction.h"
#include "widgets/StringAction.h"

#include <QHBoxLayout>

class ColorAction : public PluginAction
{
public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, ColorAction* colorAction);

    private:
        QHBoxLayout         _layout;
    };

public:
    ColorAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

    void initScalarDimOptions(const unsigned int nDim) {};
    void initScalarDimOptions(const std::vector<QString>& dimNames) {};

protected:
    hdps::gui::OptionAction     _colorByAction;
    QAction                     _colorByDimensionAction;
    QAction                     _colorByDataAction;
    QActionGroup                _colorByActionGroup;
    hdps::gui::OptionAction     _colorDimensionAction;
    hdps::gui::StringAction     _colorDataAction;
    QAction                     _removeColorDataAction;
    QAction                     _resetAction;

    friend class Widget;
};