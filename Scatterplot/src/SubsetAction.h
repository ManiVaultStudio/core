#pragma once

#include "widgets/WidgetAction.h"

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

class ScatterplotPlugin;

class SubsetAction : public hdps::gui::WidgetAction
{
    Q_OBJECT

public:
    class Widget : public hdps::gui::WidgetAction::Widget {
    public:
        Widget(QWidget* parent, SubsetAction* subsetAction);

    private:
        QHBoxLayout         _layout;
        QToolButton         _toolButton;
        PopupWidget         _popupWidget;
        QWidgetAction       _popupWidgetAction;
    };

public:
    SubsetAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

protected:
    QAction     _createSubsetAction;
    QAction     _fromSourceDataAction;

    friend class Widget;
};