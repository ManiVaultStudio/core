#pragma once

#include <QWidget>

class ScatterplotPlugin;

namespace Ui {
    class DimensionPickerWidget;
}

class DimensionPickerWidget : public QWidget
{
public:
    DimensionPickerWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

    void setDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());
    void setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());
    int getDimensionX();
    int getDimensionY();

private:
    std::unique_ptr<Ui::DimensionPickerWidget>        _ui;        /** Externally loaded UI */
};