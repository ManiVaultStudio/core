#pragma once

#include "ScatterplotWidget.h"
#include "widgets/PopupPushButton.h"

#include <QWidget>
#include <QIcon>

class ScatterplotPlugin;

namespace Ui {
    class RenderModeWidget;
}

class RenderModeWidget : public QWidget
{
public:

    enum class State {
        PopupButton,
        IconsOnly,
        Full
    };

public:
    RenderModeWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);
    
private:

    QIcon getIcon(const ScatterplotWidget::RenderMode& renderMode) const;

public: // Widget getters

    hdps::gui::PopupPushButton& getPopupPushButton();
    QPushButton& getScatterPlotPushButton();
    QPushButton& getDensityPlotPushButton();
    QPushButton& getContourPlotPushButton();

private:
    std::unique_ptr<Ui::RenderModeWidget>       _ui;            /** Externally loaded UI */
    QSize                                       _iconSize;      /** Icon size  */
};