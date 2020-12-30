#pragma once

#include <QStackedWidget>

class ScatterplotPlugin;

class QComboBox;

namespace Ui {
    class ColorDimensionStackedWidget;
}

class ColorDimensionStackedWidget : public QStackedWidget
{
public:
    ColorDimensionStackedWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

    void setDimensions(unsigned int numDimensions, const std::vector<QString>& names);
    void setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names);

    QComboBox* getColorByDimensionComboBox();

    QSize sizeHint() const override {
        return currentWidget()->sizeHint();
    }

    QSize minimumSizeHint() const override {
        return currentWidget()->minimumSizeHint();
    }

private:
    std::unique_ptr<Ui::ColorDimensionStackedWidget>        _ui;        /** Externally loaded UI */
};