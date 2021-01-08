#pragma once

#include "WidgetStateMixin.h"
#include "ScatterplotWidget.h"

#include "util/WidgetResizeEventProxy.h"
#include "widgets/PopupPushButton.h"

#include <QWidget>
#include <QIcon>
#include <QGroupBox>

class ScatterplotPlugin;

class QVBoxLayout;
class QPushButton;

class RenderModeWidget : public QWidget, public WidgetStateMixin
{
public:
    RenderModeWidget(const WidgetStateMixin::State& state, QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);
    
    /*
    State getState(const QSize& sourceWidgetSize) const;
    void setState(const WidgetStateMixin::State& state);
    */

    //QIcon getIcon(const ScatterplotWidget::RenderMode& renderMode) const;

private:
    QPushButton*        _scatterPlotPushButton;
    QPushButton*        _densityPlotPushButton;
    QPushButton*        _contourPlotPushButton;

    static const QSize BUTTON_SIZE_COMPACT;
    static const QSize BUTTON_SIZE_FULL;
};