#pragma once

#include "widgets/ResponsiveToolBar.h"

#include <QWidget>
#include <QSize>

class ScatterplotPlugin;

class QPushButton;

class RenderModeWidget : public QStackedWidget
{
public:
    RenderModeWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

    QSize sizeHint() const override {
        return currentWidget()->sizeHint();
    }

    QSize minimumSizeHint() const override {
        return currentWidget()->minimumSizeHint();
    }

private:
    hdps::gui::WidgetState  _widgetState;
    QPushButton*            _popupPushButton;
    QWidget*                _widget;
    QPushButton*            _scatterPlotPushButton;
    QPushButton*            _densityPlotPushButton;
    QPushButton*            _contourPlotPushButton;
};