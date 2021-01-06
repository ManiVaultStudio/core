#pragma once

#include <QWidget>

class ScatterplotPlugin;

namespace Ui {
    class DimensionSettingsWidget;
}

class DimensionSettingsWidget : public QWidget
{
public:
    DimensionSettingsWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

    bool eventFilter(QObject* target, QEvent* event) override;

    int getDimensionX();
    int getDimensionY();

    void setDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());
    void setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());

private:
    std::unique_ptr<Ui::DimensionSettingsWidget>        _ui;        /** Externally loaded UI */
};