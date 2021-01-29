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
    void setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin);

protected:
    virtual void connectToPlugin() = 0;

protected:
    hdps::gui::ResponsiveToolBar::WidgetState   _state;

public:
    static ScatterplotPlugin* scatterplotPlugin;

    friend class ScatterplotPlugin;
};