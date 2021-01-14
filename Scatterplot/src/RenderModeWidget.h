#pragma once

#include "WidgetStateMixin.h"

#include <QWidget>

class ScatterplotPlugin;

class QPushButton;

class RenderModeWidget : public QWidget, public WidgetStateMixin
{
public:
    RenderModeWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);
    
    void updateState() override;

private:
    QPushButton*    _scatterPlotPushButton;
    QPushButton*    _densityPlotPushButton;
    QPushButton*    _contourPlotPushButton;

    static const QSize BUTTON_SIZE_COMPACT;
    static const QSize BUTTON_SIZE_FULL;
};