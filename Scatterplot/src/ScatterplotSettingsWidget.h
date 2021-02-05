#pragma once

#include "widgets/ResponsiveToolBar.h"

#include <QWidget>
#include <QList>
#include <QSize>

class ScatterplotPlugin;
class ScatterplotWidget;

class ScatterplotSettingsWidget : public QWidget
{
public:
    using SizeHints = QList<QSize>;

protected:
    ScatterplotSettingsWidget(const hdps::gui::ResponsiveSectionWidget::WidgetState& widgetState, QWidget* parent = nullptr);

public:
    virtual void setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin) = 0;

    ScatterplotWidget* getScatterplotWidget();

    void applyLayout(QLayout* layout);

    bool isForm() const;
    bool isSequential() const;
    bool isCompact() const;
    bool isFull() const;

protected:
    hdps::gui::ResponsiveSectionWidget::WidgetState     _widgetState;
    ScatterplotPlugin*                                  _scatterplotPlugin;

    friend class ScatterplotPlugin;
};