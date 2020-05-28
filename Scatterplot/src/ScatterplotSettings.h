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
class QCheckBox;

struct PointSettingsWidget : public QWidget
{
    PointSettingsWidget(const ScatterplotPlugin& plugin);

    static constexpr int MIN_POINT_SIZE = 5;
    static constexpr int MAX_POINT_SIZE = 20;
    static constexpr int MIN_POINT_OPACITY = 0;
    static constexpr int MAX_POINT_OPACITY = 100;

    QLabel& _pointSizeLabel;
    QSlider& _pointSizeSlider;
    QLabel& _pointOpacityLabel;
    QSlider& _pointOpacitySlider;
};

struct DensitySettingsWidget : public QWidget
{
    DensitySettingsWidget(const ScatterplotPlugin& plugin);

    static constexpr int MIN_SIGMA = 1;
    static constexpr int MAX_SIGMA = 50;

    QLabel& _sigmaLabel;
    QSlider& _sigmaSlider;
    QPushButton& _computeDensityButton;
};

struct PlotSettingsStack : public QStackedWidget
{
    PlotSettingsStack(const ScatterplotPlugin& plugin);

private:
    PointSettingsWidget& _pointSettingsWidget;
    DensitySettingsWidget& _densitySettingsWidget;
};

struct ColorDimensionPicker : public QWidget
{
    Q_OBJECT
public:
    ColorDimensionPicker(const ScatterplotPlugin& plugin);

    void setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names);
private:
    QVBoxLayout* _layout;

    hdps::gui::ComboBox* _cDimOptions;
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

    hdps::gui::ComboBox& _xDimOptions = *new hdps::gui::ComboBox;
    hdps::gui::ComboBox& _yDimOptions = *new hdps::gui::ComboBox;
    hdps::gui::ComboBox* _colorOptions;
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

    bool IsNotifyingOnSelecting() const;

protected slots:
    void renderModePicked(const int index);

private:
    const hdps::Vector3f DEFAULT_BASE_COLOR = hdps::Vector3f(255.f / 255, 99.f / 255, 71.f / 255);
    const hdps::Vector3f DEFAULT_SELECTION_COLOR = hdps::Vector3f(72.f / 255, 61.f / 255, 139.f / 255);

    hdps::gui::PushButton& _subsetButton = *new hdps::gui::PushButton;
    hdps::gui::ComboBox& _renderMode = *new hdps::gui::ComboBox;

    QVBoxLayout* _settingsLayout;
    PlotSettingsStack* _settingsStack;

    DimensionPicker* _dimensionPicker;

    hdps::Vector3f _baseColor;
    hdps::Vector3f _selectionColor;

    const QCheckBox* _notifyOnSelectingCheckBox{ nullptr };
};
