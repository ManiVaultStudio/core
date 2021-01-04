#pragma once

#include "ScatterplotWidget.h"

#include <QWidget>
#include <QIcon>

class ScatterplotPlugin;

namespace Ui {
    class RenderModeWidget;
}

class RenderModeWidget : public QWidget
{
public:
    RenderModeWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

private:

    QIcon getIcon(const ScatterplotWidget::RenderMode& renderMode) const;

private:
    std::unique_ptr<Ui::RenderModeWidget>        _ui;        /** Externally loaded UI */
    QSize                                           _iconSize;  /** Icon size  */
};