#pragma once

#include <QStackedWidget>

class ScatterplotPlugin;

namespace Ui {
    class PlotSettingsStackedWidget;
}

class PlotSettingsStackedWidget : public QStackedWidget
{
public:
    PlotSettingsStackedWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

    QSize sizeHint() const override {
        return currentWidget()->sizeHint();
    }

    QSize minimumSizeHint() const override {
        return currentWidget()->minimumSizeHint();
    }

private:
    std::unique_ptr<Ui::PlotSettingsStackedWidget>        _ui;        /** Externally loaded UI */
};