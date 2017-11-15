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

private slots:
    void targetEventsChanged(double value);
    void targetNodesChanged(int value);
    void heuristicSamplesChanged(int value);
    void alphaChanged(double value);
    void targetDensityChanged(double value);
    void outlierDensityChanged(double value);

public:
    QComboBox _dataOptions;

    QPushButton _startButton;

    QDoubleSpinBox _targetEvents;
    QSpinBox _targetNodes;

    QSpinBox _heuristicSamples;
    QDoubleSpinBox _alpha;
    QDoubleSpinBox _targetDensity;
    QDoubleSpinBox _outlierDensity;
};

#endif // SPADE_SETTINGS_WIDGET_H
