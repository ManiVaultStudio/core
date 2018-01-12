#ifndef DENSITY_PLOT_SETTINGS_H
#define DENSITY_PLOT_SETTINGS_H

#include "graphics/Vector3f.h"

#include <QWidget>
#include <QComboBox>
#include <QSlider>
#include <QPushButton>

class DensityPlotPlugin;

class DensityPlotSettings : public QWidget
{
    Q_OBJECT

public:
    DensityPlotSettings(const DensityPlotPlugin* plugin);
    ~DensityPlotSettings();

    void xDimPicked(int index);
    void yDimPicked(int index);

    int getXDimension();
    int getYDimension();
    QString currentData();

    void initDimOptions(const unsigned int nDim);
    void addDataOption(const QString option);
    int numDataOptions();

private:
    QComboBox _dataOptions;
    QPushButton _subsetButton;

    QComboBox _xDimOptions;
    QComboBox _yDimOptions;
};

#endif // DENSITY_PLOT_SETTINGS_H
