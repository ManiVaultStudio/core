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
    ScatterplotSettingsWidget(const hdps::gui::ResponsiveSectionWidget::State& state, QWidget* parent = nullptr);

public:
    virtual void setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin) = 0;

protected:
    hdps::gui::ResponsiveSectionWidget::State   _state;
    ScatterplotPlugin*                          _scatterplotPlugin;

    friend class ScatterplotPlugin;
};