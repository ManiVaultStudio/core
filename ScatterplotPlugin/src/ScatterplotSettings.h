#pragma once

#include "graphics/Vector3f.h"

#include <QWidget>
#include <QComboBox>
#include <QSlider>
#include <QLabel>
#include <QPushButton>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QStackedWidget>

struct PointSettingsWidget : public QWidget
{
    PointSettingsWidget()
        :
        _pointSizeLabel("Point Size:"),
        _pointSizeSlider(Qt::Horizontal)
    {
        QVBoxLayout* pointSettingsLayout = new QVBoxLayout();
        pointSettingsLayout->addWidget(&_pointSizeLabel);
        pointSettingsLayout->addWidget(&_pointSizeSlider);
        setLayout(pointSettingsLayout);
    }

    QLabel _pointSizeLabel;
    QSlider _pointSizeSlider;
};

struct DensitySettingsWidget : public QWidget
{
    DensitySettingsWidget()
        :
        _sigmaLabel("Sigma:"),
        _sigmaSlider(Qt::Horizontal)
    {
        QVBoxLayout* densitySettingsLayout = new QVBoxLayout();
        densitySettingsLayout->addWidget(&_sigmaLabel);
        densitySettingsLayout->addWidget(&_sigmaSlider);
        setLayout(densitySettingsLayout);
    }

    QLabel _sigmaLabel;
    QSlider _sigmaSlider;
};

class ScatterplotPlugin;

class ScatterplotSettings : public QWidget
{
    Q_OBJECT

public:
    ScatterplotSettings(const ScatterplotPlugin* plugin);
    ~ScatterplotSettings();

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

    void initDimOptions(const unsigned int nDim);
    void initDimOptions(const std::vector<QString> dimNames);
    void addDataOption(const QString option);
    int numDataOptions();

protected slots:
    void renderModePicked(const int index);

private:
    const hdps::Vector3f DEFAULT_BASE_COLOR = hdps::Vector3f(255.f / 255, 99.f / 255, 71.f / 255);
    const hdps::Vector3f DEFAULT_SELECTION_COLOR = hdps::Vector3f(72.f / 255, 61.f / 255, 139.f / 255);

    const int MIN_POINT_SIZE = 5;
    const int MAX_POINT_SIZE = 20;

    const int MIN_SIGMA = 1;
    const int MAX_SIGMA = 50;

    QComboBox _dataOptions;
    QPushButton _subsetButton;
    QComboBox _renderMode;

    QGridLayout* _settingsLayout;
    QStackedWidget* _settingsStack;
    PointSettingsWidget _pointSettingsWidget;
    DensitySettingsWidget _densitySettingsWidget;

    QComboBox _xDimOptions;
    QComboBox _yDimOptions;
    QComboBox _cDimOptions;

    hdps::Vector3f _baseColor;
    hdps::Vector3f _selectionColor;
};
