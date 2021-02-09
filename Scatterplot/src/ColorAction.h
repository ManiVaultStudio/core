#pragma once

#include "widgets/WidgetAction.h"
#include "widgets/OptionAction.h"
#include "widgets/StringAction.h"

#include <QHBoxLayout>
#include <QGroupBox>
#include <QToolBar>
#include <QToolButton>

class ScatterplotPlugin;

class ColorAction : public hdps::gui::WidgetAction
{
    Q_OBJECT

public:
    class Widget : public hdps::gui::WidgetAction::Widget {
    public:
        Widget(QWidget* parent, ColorAction* colorAction);

    private:
        QHBoxLayout         _layout;
        QToolBar            _toolBar;
        QToolButton         _toolButton;
        PopupWidget         _popupWidget;
        QWidgetAction       _popupWidgetAction;
    };

public:
    ColorAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

protected:
    hdps::gui::OptionAction     _colorByAction;
    hdps::gui::OptionAction     _colorDimensionAction;
    hdps::gui::StringAction     _colorDataAction;
    QAction                     _removeColorDataAction;
    QAction                     _resetAction;

    friend class Widget;
};