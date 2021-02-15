#pragma once

#include "PluginAction.h"

#include "widgets/StandardAction.h"
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

public:
    ColoringAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
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
    hdps::gui::ColorAction      _constantColorAction;
    hdps::gui::OptionAction     _dimensionAction;
    hdps::gui::StringAction     _colorDataAction;
    hdps::gui::StandardAction   _removeColorDataAction;
    hdps::gui::StandardAction   _resetAction;

    friend class Widget;
};