#pragma once

#include "graphics/Vector3f.h"

#include "widgets/ComboBox.h"
#include "widgets/PushButton.h"
#include "widgets/HorizontalDivider.h"
#include "widgets/DataSlot.h"

#include <QWidget>
#include <QSlider>
#include <QLabel>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QStackedWidget>

class ScatterplotPlugin;
class QComboBox;
class QCheckBox;
class QPushButton;

struct PlotSettingsStack : public QStackedWidget
{
    PlotSettingsStack(const ScatterplotPlugin& plugin);
};

struct ColorDimensionPicker : public QWidget
{
    Q_OBJECT
public:
    ColorDimensionPicker(const ScatterplotPlugin& plugin);

    void setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names);
private:
    QVBoxLayout* _layout;

    QComboBox* _cDimOptions;
};

struct ColorDropSlot : public QWidget
{
    Q_OBJECT
public:
    ColorDropSlot(const ScatterplotPlugin& plugin);
private:
    QHBoxLayout* _layout;

    hdps::gui::DataSlot& _loadColorData;
};

struct ColorSettingsStack : public QStackedWidget
{
public:
    ColorSettingsStack(const ScatterplotPlugin& plugin)
    {
        _colorDimensionPicker = new ColorDimensionPicker(plugin);
        _colorDropSlot = new ColorDropSlot(plugin);

        addWidget(_colorDimensionPicker);
        addWidget(_colorDropSlot);
    }

    ColorDimensionPicker& getColorDimensionPicker() { return *_colorDimensionPicker; }
    ColorDropSlot& getColorDropSlot() { return *_colorDropSlot; }
private:
    ColorDimensionPicker* _colorDimensionPicker;
    ColorDropSlot* _colorDropSlot;
};

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

class ScatterplotSettings : public QWidget
{
    Q_OBJECT

public:
    ScatterplotSettings(const ScatterplotPlugin* plugin);
    ~ScatterplotSettings() override;

    int getXDimension();
    int getYDimension();
    hdps::Vector3f getBaseColor();
    hdps::Vector3f getSelectionColor();
    void showPointSettings();
    void showDensitySettings();

    void initDimOptions(const unsigned int nDim);
    void initDimOptions(const std::vector<QString>& dimNames);
    void initScalarDimOptions(const unsigned int nDim);
    void initScalarDimOptions(const std::vector<QString>& dimNames);

    void paintEvent(QPaintEvent* event) override;

protected slots:
    void renderModePicked(const int index);

private:
    const hdps::Vector3f DEFAULT_BASE_COLOR = hdps::Vector3f(255.f / 255, 99.f / 255, 71.f / 255);
    const hdps::Vector3f DEFAULT_SELECTION_COLOR = hdps::Vector3f(72.f / 255, 61.f / 255, 139.f / 255);

    QPushButton& _subsetButton = *new QPushButton;
    QComboBox& _renderMode = *new QComboBox;

    QVBoxLayout* _settingsLayout;
    PlotSettingsStack* _settingsStack;

    DimensionPicker* _dimensionPicker;

    hdps::Vector3f _baseColor;
    hdps::Vector3f _selectionColor;
};
