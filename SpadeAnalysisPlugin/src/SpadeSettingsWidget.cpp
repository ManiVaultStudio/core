#include "SpadeSettingsWidget.h"

#include "SpadeAnalysisPlugin.h"

#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>

#include <QIntValidator>
#include <QDoubleValidator>

using namespace hdps::plugin;

SpadeSettingsWidget::SpadeSettingsWidget(const SpadeAnalysisPlugin* analysis)
{
    setFixedWidth(200);

    connect(&_dataOptions,   SIGNAL(currentIndexChanged(QString)), analysis, SLOT(dataSetPicked(QString)));
    connect(&_startButton,   SIGNAL(clicked()), analysis, SLOT(startComputation()));

    connect(&_targetEvents,     SIGNAL(valueChanged(double)), analysis, SLOT(targetEventsChanged(double)));
    connect(&_targetNodes,      SIGNAL(valueChanged(int)),    analysis, SLOT(targetNodesChanged(int)));
    connect(&_heuristicSamples, SIGNAL(valueChanged(int)),    analysis, SLOT(heuristicSamplesChanged(int)));
    connect(&_alpha,            SIGNAL(valueChanged(double)), analysis, SLOT(alphaChanged(double)));
    connect(&_targetDensity,    SIGNAL(valueChanged(double)), analysis, SLOT(targetDensityChanged(double)));
    connect(&_outlierDensity,   SIGNAL(valueChanged(double)), analysis, SLOT(outlierDensityChanged(double)));

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

    _targetEvents.setToolTip("Target number of points to keep after downsampling in percent.\nThe number might not be reached depending on the outlier and target density parameters.");
    _targetEvents.setRange(0, 100.0);
    _targetEvents.setValue(analysis->targetEvents());
    _targetNodes.setToolTip("Target number of nodes in the SPADE tree.");
    _targetNodes.setRange(0, 99999);
    _targetNodes.setValue(analysis->targetNumClusters());

    _heuristicSamples.setToolTip("The number of random Points used to determine the median density.");
    _heuristicSamples.setRange(0, 999999);
    _heuristicSamples.setValue(analysis->maxRandomSampleSize());
    _alpha.setToolTip("Scales the neighborhood size for the density computation for downsampling.");
    _alpha.setValue(analysis->alpha());
    _targetDensity.setToolTip("Percentile of the upper limit of the target density. Points with a density between the outlier density and the target density are considered rare, but not noise and kept.\n A value of 3.0 (and an outlier density of 1.0) means the cells in between the first and third percentile, ordered by density are considered rare and kept.\nPoints with a density above this value are kept only with a probability according to their density, the larger the density, the lower the probability.");
    _targetDensity.setValue(analysis->targetDensityPercentile());
    _outlierDensity.setToolTip("Percentile of the upper limit of the outlier density. Points with a density below this threshold are considered as noise.\nA value of 1.0 means the first percentile of the cells ordered by density is discarded. Use larger values if you expect more noise.");
    _outlierDensity.setValue(analysis->outlierDensityPercentile());

    _startButton.setText("Cluster");
    _startButton.setFixedSize(QSize(150, 50));

    QGridLayout *settingsLayout = new QGridLayout;
    settingsLayout->setAlignment(Qt::AlignTop);
    settingsLayout->addWidget(targetEventsLabel, 0, 0);
    settingsLayout->addWidget(&_targetEvents, 0, 1);
    settingsLayout->addWidget(targetNodesLabel, 1, 0);
    settingsLayout->addWidget(&_targetNodes);
    settingsLayout->setColumnStretch(0, 0);
    settingsLayout->setColumnStretch(1, 1);
    settingsBox->setLayout(settingsLayout);

    QGridLayout* advancedSettingsLayout = new QGridLayout;
    advancedSettingsLayout->setAlignment(Qt::AlignTop);
    advancedSettingsLayout->addWidget(heuristicSamplesLabel, 0, 0);
    advancedSettingsLayout->addWidget(&_heuristicSamples, 0, 1);
    advancedSettingsLayout->addWidget(alphaLabel, 1, 0);
    advancedSettingsLayout->addWidget(&_alpha, 1, 1);
    advancedSettingsLayout->addWidget(targetDensityLabel, 2, 0);
    advancedSettingsLayout->addWidget(&_targetDensity, 2, 1);
    advancedSettingsLayout->addWidget(outlierDensityLabel, 3, 0);
    advancedSettingsLayout->addWidget(&_outlierDensity, 3, 1);
    advancedSettingsLayout->setColumnStretch(0, 0);
    advancedSettingsLayout->setColumnStretch(1, 1);
    advancedSettingsBox->setLayout(advancedSettingsLayout);

    addWidget(&_dataOptions);
    addWidget(settingsBox);
    addWidget(advancedSettingsBox);
    addWidget(&_startButton);
}
