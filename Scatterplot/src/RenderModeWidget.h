#pragma once

#include "widgets/WidgetState.h"
#include "widgets/PopupPushButton.h"

#include <QStackedWidget>

class ScatterplotPlugin;

class QPushButton;

class RenderModeWidget : public QStackedWidget
{
public:
    RenderModeWidget(QWidget* parent = nullptr);

    void initializeUI();
    void setScatterPlotPlugin(const ScatterplotPlugin& plugin);

    QSize sizeHint() const override {
        return currentWidget()->sizeHint();
    }

    QSize minimumSizeHint() const override {
        return currentWidget()->minimumSizeHint();
    }

private:
    hdps::gui::WidgetState          _widgetState;
    hdps::gui::PopupPushButton*     _popupPushButton;
    QWidget*                        _widget;
    QPushButton*                    _scatterPlotPushButton;
    QPushButton*                    _densityPlotPushButton;
    QPushButton*                    _contourPlotPushButton;
};