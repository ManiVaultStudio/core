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
class QStringListModel;

struct PointSettingsWidget : public QWidget
{
    PointSettingsWidget(const ScatterplotPlugin& plugin);

    const int MIN_POINT_SIZE = 5;
    const int MAX_POINT_SIZE = 20;
    const int MIN_POINT_OPACITY = 0;
    const int MAX_POINT_OPACITY = 100;

    QLabel _pointSizeLabel;
    QSlider _pointSizeSlider;
    QLabel _pointOpacityLabel;
    QSlider _pointOpacitySlider;
};

struct DensitySettingsWidget : public QWidget
{
    DensitySettingsWidget(const ScatterplotPlugin& plugin);

    const int MIN_SIGMA = 1;
    const int MAX_SIGMA = 50;

    QLabel _sigmaLabel;
    QSlider _sigmaSlider;
};

struct PlotSettingsStack : public QStackedWidget
{
    PlotSettingsStack(const ScatterplotPlugin& plugin);

private:
    PointSettingsWidget _pointSettingsWidget;
    DensitySettingsWidget _densitySettingsWidget;
};

struct DimensionPicker : public QWidget
{
    DimensionPicker(const ScatterplotPlugin* plugin);

    QGridLayout& getLayout();

    
    void setDimensions(const QStringList &names);
	void setScalarDimensions(const QStringList &names);

    int getDimensionX();
    int getDimensionY();
    int getDimensionColor();

private:
    QLabel _xDimLabel;
    QLabel _yDimLabel;
    QLabel _cDimLabel;

    hdps::gui::ComboBox _xDimOptions;
    hdps::gui::ComboBox _yDimOptions;
    hdps::gui::ComboBox _cDimOptions;
	QStringListModel *_dimNamesModel;
	QStringListModel *_cDimNamesModel;

    QGridLayout _layout;
};

class ScatterplotSettings : public QWidget
{
    Q_OBJECT

public:
    ScatterplotSettings(const ScatterplotPlugin* plugin);
    ~ScatterplotSettings() override;

    void xDimPicked(int index);
    void yDimPicked(int index);

    int getXDimension();
    int getYDimension();
    int getColorDimension();
    hdps::Vector3f getBaseColor();
    hdps::Vector3f getSelectionColor();
    void showPointSettings();
    void showDensitySettings();
    QString currentData();

    
    void initDimOptions(const QStringList &dimNames);
    
    void initScalarDimOptions(const QStringList &dimNames);
    void addDataOption(const QString option);
    int numDataOptions();

    void paintEvent(QPaintEvent* event) override;

protected slots:
    void renderModePicked(const int index);

private:
    const hdps::Vector3f DEFAULT_BASE_COLOR = hdps::Vector3f(255.f / 255, 99.f / 255, 71.f / 255);
    const hdps::Vector3f DEFAULT_SELECTION_COLOR = hdps::Vector3f(72.f / 255, 61.f / 255, 139.f / 255);

    hdps::gui::ComboBox _dataOptions;
    hdps::gui::PushButton _subsetButton;
    hdps::gui::ComboBox _renderMode;

    QVBoxLayout* _settingsLayout;
    PlotSettingsStack* _settingsStack;

    DimensionPicker* _dimensionPicker;

    hdps::Vector3f _baseColor;
    hdps::Vector3f _selectionColor;
};
