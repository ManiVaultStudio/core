#pragma once

#include "widgets/WidgetAction.h"
#include "widgets/OptionAction.h"
#include "widgets/DoubleAction.h"

#include <QAction>
#include <QWidgetAction>
#include <QMenu>
#include <QComboBox>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QToolBar>
#include <QToolButton>
#include <QEvent>

class ScatterplotWidget;

class SelectionAction : public hdps::gui::WidgetAction
{
    Q_OBJECT

public:
    class Widget : public hdps::gui::WidgetAction::Widget {
    public:
        Widget(QWidget* parent, SelectionAction* selectionAction);

    private:
        QHBoxLayout         _layout;
        QToolBar            _toolBar;
        QToolButton         _toolButton;
        PopupWidget         _popupWidget;
        QWidgetAction       _popupWidgetAction;
    };

public:
    SelectionAction(QObject* parent);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

protected:
    hdps::gui::OptionAction     _typeAction;
    QAction                     _brushAction;
    QAction                     _lassoAction;
    QAction                     _polygonAction;
    QAction                     _rectangleAction;
    QAction                     _modifierAddAction;
    QAction                     _modifierRemoveAction;
    hdps::gui::DoubleAction     _brushRadiusAction;
    QAction                     _clearSelectionAction;
    QAction                     _selectAllAction;
    QAction                     _invertSelectionAction;
    QAction                     _notifyDuringSelectionAction;

    friend class Widget;
};