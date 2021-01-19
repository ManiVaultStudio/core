#pragma once

#include <QWidget>

class ScatterplotPlugin;

class QLabel;
class QComboBox;

class ColorSettingsWidget : public QWidget
{
public:
    ColorSettingsWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

public:

    /*
    int getDimensionX();
    int getDimensionY();

    void setDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());
    void setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());
    */

private:
    QLabel*         _colorByLabel;
    QComboBox*      _colorByComboBox;
};