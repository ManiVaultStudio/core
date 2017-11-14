#include "SpadeSettingsWidget.h"

#include "SpadeAnalysisPlugin.h"

#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>

#include <QIntValidator>
#include <QDoubleValidator>

using namespace hdps::plugin;

SpadeSettingsWidget::SpadeSettingsWidget(const SpadeAnalysisPlugin* analysis) {
    setFixedWidth(200);

    connect(&_dataOptions,   SIGNAL(currentIndexChanged(QString)), analysis, SLOT(dataSetPicked(QString)));
    connect(&_startButton,   SIGNAL(clicked()), analysis, SLOT(startComputation()));

    connect(&_targetEvents,     SIGNAL(textChanged(QString)), SLOT(targetEventsChanged(QString)));
    connect(&_targetNodes,      SIGNAL(textChanged(QString)), SLOT(targetNodesChanged(QString)));
    connect(&_heuristicSamples, SIGNAL(textChanged(QString)), SLOT(heuristicSamplesChanged(QString)));
    connect(&_alpha,            SIGNAL(textChanged(QString)), SLOT(alphaChanged(QString)));
    connect(&_targetDensity,    SIGNAL(textChanged(QString)), SLOT(targetDensityChanged(QString)));
    connect(&_outlierDensity,   SIGNAL(textChanged(QString)), SLOT(outlierDensityChanged(QString)));


    QGroupBox* settingsBox = new QGroupBox("Basic settings");
    QGroupBox* advancedSettingsBox = new QGroupBox("Advanced Settings");
    advancedSettingsBox->setCheckable(true);
    advancedSettingsBox->setChecked(false);

    QLabel* targetEventsLabel = new QLabel("Target Events %");
    QLabel* targetNodesLabel = new QLabel("Target Nodes");

    QLabel* heuristicSamplesLabel = new QLabel("Heuristic Samples");
    QLabel* alphaLabel = new QLabel("Alpha");
    QLabel* targetDensityLabel = new QLabel("Target Density %");
    QLabel* outlierDensityLabel = new QLabel("Outlier Density %");

    _targetEvents.setValidator(new QDoubleValidator(0, 100, 2, this));
    _targetEvents.setText("10.0");
    _targetEvents.setFixedWidth(50);
    _targetNodes.setValidator(new QIntValidator(1, 100000, this));
    _targetNodes.setText("50");
    _targetNodes.setFixedWidth(50);

    _heuristicSamples.setValidator(new QIntValidator(1, 10000000, this));
    _heuristicSamples.setText("2000");
    _heuristicSamples.setFixedWidth(50);
    _alpha.setValidator(new QDoubleValidator(0, 100, 2, this));
    _alpha.setText("3.00");
    _alpha.setFixedWidth(50);
    _targetDensity.setValidator(new QDoubleValidator(0, 100, 2, this));
    _targetDensity.setText("3.00");
    _targetDensity.setFixedWidth(50);
    _outlierDensity.setValidator(new QDoubleValidator(0, 100, 2, this));
    _outlierDensity.setText("1.00");
    _outlierDensity.setFixedWidth(50);

    _startButton.setText("Cluster");
    _startButton.setFixedSize(QSize(150, 50));

    QVBoxLayout *settingsLayout = new QVBoxLayout;
    settingsLayout->addWidget(targetEventsLabel);
    settingsLayout->addWidget(&_targetEvents);
    settingsLayout->addWidget(targetNodesLabel);
    settingsLayout->addWidget(&_targetNodes);
    settingsLayout->addStretch(1);
    settingsBox->setLayout(settingsLayout);

    QGridLayout* advancedSettingsLayout = new QGridLayout;
    advancedSettingsLayout->addWidget(heuristicSamplesLabel, 0, 0);
    advancedSettingsLayout->addWidget(&_heuristicSamples, 1, 0);
    advancedSettingsLayout->addWidget(alphaLabel, 0, 1);
    advancedSettingsLayout->addWidget(&_alpha, 1, 1);
    advancedSettingsLayout->addWidget(targetDensityLabel, 2, 0);
    advancedSettingsLayout->addWidget(&_targetDensity, 3, 0);
    advancedSettingsLayout->addWidget(outlierDensityLabel, 2, 1);
    advancedSettingsLayout->addWidget(&_outlierDensity, 3, 1);
    advancedSettingsBox->setLayout(advancedSettingsLayout);

    addWidget(&_dataOptions);
    addWidget(settingsBox);
    addWidget(advancedSettingsBox);
    addWidget(&_startButton);
}
