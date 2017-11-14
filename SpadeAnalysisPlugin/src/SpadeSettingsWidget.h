#ifndef SPADE_SETTINGS_WIDGET_H
#define SPADE_SETTINGS_WIDGET_H

#include <widgets/SettingsWidget.h>

#include <QObject>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>

using namespace hdps::gui;

class SpadeAnalysisPlugin;

class SpadeSettingsWidget : public SettingsWidget
{
    Q_OBJECT

public:
    SpadeSettingsWidget(const SpadeAnalysisPlugin* analysis);

public:
    QComboBox _dataOptions;
    QPushButton _startButton;

    QLineEdit _targetEvents;
    QLineEdit _targetNodes;

    QLineEdit _heuristicSamples;
    QLineEdit _alpha;
    QLineEdit _targetDensity;
    QLineEdit _outlierDensity;
};

#endif // SPADE_SETTINGS_WIDGET_H
