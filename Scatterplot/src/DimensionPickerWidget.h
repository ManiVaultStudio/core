#pragma once

#include <QWidget>

class ScatterplotPlugin;

namespace Ui {
    class DimensionPickerWidget;
}

class DimensionPickerWidget : public QWidget
{
public:
    DimensionPickerWidget(const ScatterplotPlugin& plugin);

    void setDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());
    void setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());
    int getDimensionX();
    int getDimensionY();

private:
    std::unique_ptr<Ui::DimensionPickerWidget>        _ui;        /** Externally loaded UI */
};

/*
struct DimensionPicker : public QWidget
{
    Q_OBJECT
public:
    DimensionPicker(const ScatterplotPlugin* plugin);

    QGridLayout& getLayout();

    void setDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());
    void setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());
    int getDimensionX();
    int getDimensionY();

protected slots:
    void colorOptionsPicked(const int index);

private:
    QLabel& _xDimLabel;
    QLabel& _yDimLabel;
    QLabel& _cDimLabel;

    QComboBox& _xDimOptions = *new QComboBox;
    QComboBox& _yDimOptions = *new QComboBox;
    QComboBox* _colorOptions;
    ColorSettingsStack* _colorSettingsStack;

    QGridLayout& _layout = *new QGridLayout;
};
*/