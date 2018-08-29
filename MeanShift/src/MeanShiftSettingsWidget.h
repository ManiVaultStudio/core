#pragma once

#include <widgets/SettingsWidget.h>

#include <QObject>
#include <QComboBox>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QSpinBox>

using namespace hdps::gui;

struct SpadeSettings {
    SpadeSettings() :
        _maxRandomSampleSize(2000),
        _alpha(3.0),
        _targetDensityPercentile(3.0),
        _outlierDensityPercentile(1.0),
        _densityLimit(10.0),
        _targetNumClusters(50)
    { }

    // Maximum number of points you want to keep after downsampling
    float _densityLimit;
    // Target number of nodes in the SPADE tree
    int _targetNumClusters;
    // Number of random points to use for median density calculation
    int _maxRandomSampleSize;
    // Scales the neighborhood size for the density computation for downsampling
    float _alpha;
    // Percentile of the upper limit of the target density
    float _targetDensityPercentile;
    // Percentile of the upper limit of the outlier density
    float _outlierDensityPercentile;
};

class MeanShiftSettingsWidget : public SettingsWidget
{
    Q_OBJECT

public:
    MeanShiftSettingsWidget();

    void addDataOption(QString option);
    QString getCurrentDataOption();

    const SpadeSettings& getSpadeSettings() const;

signals:
    void startClustering();
private:
    SpadeSettings spadeSettings;

    QComboBox _dataOptions;
    QPushButton _startButton;
};
