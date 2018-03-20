#ifndef SCATTERPLOT_SETTINGS_H
#define SCATTERPLOT_SETTINGS_H

#include "graphics/Vector3f.h"

#include <QWidget>
#include <QComboBox>
#include <QSlider>
#include <QPushButton>

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
    hdps::Vector3f getBaseColor();
    hdps::Vector3f getSelectionColor();
    QString currentData();

    void initDimOptions(const unsigned int nDim);
    void addDataOption(const QString option);
    int numDataOptions();

private:
    const hdps::Vector3f DEFAULT_BASE_COLOR = hdps::Vector3f(255.f / 255, 99.f / 255, 71.f / 255);
    const hdps::Vector3f DEFAULT_SELECTION_COLOR = hdps::Vector3f(72.f / 255, 61.f / 255, 139.f / 255);

    const int MIN_POINT_SIZE = 1;
    const int MAX_POINT_SIZE = 20;

    QComboBox _dataOptions;
    QSlider _pointSizeSlider;
    QPushButton _subsetButton;

    QComboBox _renderMode;

    QComboBox _xDimOptions;
    QComboBox _yDimOptions;

    hdps::Vector3f _baseColor;
    hdps::Vector3f _selectionColor;
};

#endif // SCATTERPLOT_SETTINGS_H
