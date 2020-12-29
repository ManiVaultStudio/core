#pragma once

#include "graphics/Vector3f.h"

#include "widgets/ComboBox.h"
#include "widgets/PushButton.h"
#include "widgets/HorizontalDivider.h"

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
class DimensionPickerWidget;

struct PlotSettingsStack : public QStackedWidget
{
    PlotSettingsStack(const ScatterplotPlugin& plugin);
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

    DimensionPickerWidget* _dimensionPickerWidget;

    hdps::Vector3f _baseColor;
    hdps::Vector3f _selectionColor;
};
