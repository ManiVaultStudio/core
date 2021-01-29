#pragma once

#include "widgets/ResponsiveToolBar.h"

#include <QWidget>
#include <QList>
#include <QSize>

class ScatterplotPlugin;

class ScatterplotSettingsWidget : public QWidget
{
public:
    using SizeHints = QList<QSize>;

protected:
    ScatterplotSettingsWidget(const hdps::gui::ResponsiveToolBar::WidgetState& state, QWidget* parent = nullptr);

public:
    virtual void setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin) = 0;

protected:
    hdps::gui::ResponsiveToolBar::WidgetState   _state;
    ScatterplotPlugin*                          _scatterplotPlugin;

    friend class ScatterplotPlugin;
};