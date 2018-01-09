#ifndef SPADE_SETTINGS_WIDGET_H
#define SPADE_SETTINGS_WIDGET_H

#include <widgets/SettingsWidget.h>

#include <QObject>
#include <QComboBox>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QSpinBox>

using namespace hdps::gui;

class SpadeAnalysisPlugin;

class SpadeSettingsWidget : public SettingsWidget
{
    Q_OBJECT

public:
    SpadeSettingsWidget(const SpadeAnalysisPlugin* analysis);

    void addDataOption(QString option);
    QString getCurrentDataOption();

    float targetEvents() const { return _densityLimit; }
    float targetNumClusters() const { return _targetNumberOfClusters; }
    float maxRandomSampleSize() const { return _maxRandomSampleSize; }
    float alpha() const { return _alpha; }
    float targetDensityPercentile() const { return _targetDensityPercentile; }
    float outlierDensityPercentile() const { return _outlierDensityPercentile; }

private:
    QComboBox _dataOptions;

    QPushButton _startButton;

    QDoubleSpinBox ui_targetEvents;
    QSpinBox ui_targetNodes;

    QSpinBox ui_heuristicSamples;
    QDoubleSpinBox ui_alpha;
    QDoubleSpinBox ui_targetDensity;
    QDoubleSpinBox ui_outlierDensity;

    // Parameters
    float _densityLimit;
    int _targetNumberOfClusters;
    // Maximum number of points you want to keep after downsampling
    int _maxRandomSampleSize;
    float _alpha;
    float _targetDensityPercentile;
    float _outlierDensityPercentile;

private slots:
    void targetEventsChanged(double value);
    void targetNodesChanged(int value);
    void heuristicSamplesChanged(int value);
    void alphaChanged(double value);
    void targetDensityChanged(double value);
    void outlierDensityChanged(double value);
};

#endif // SPADE_SETTINGS_WIDGET_H
