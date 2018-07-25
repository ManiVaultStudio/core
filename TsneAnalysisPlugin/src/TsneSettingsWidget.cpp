#include "TsneSettingsWidget.h"

#include "TsneAnalysisPlugin.h"

#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSignalMapper>

using namespace hdps::plugin;

TsneSettingsWidget::TsneSettingsWidget(const TsneAnalysisPlugin* analysis) {
    setFixedWidth(200);

    connect(&dataOptions,   SIGNAL(currentIndexChanged(QString)), analysis, SLOT(dataSetPicked(QString)));
    connect(&startButton,   SIGNAL(clicked()), analysis, SLOT(startComputation()));

    connect(&numIterations, SIGNAL(textChanged(QString)), SLOT(numIterationsChanged(QString)));
    connect(&perplexity,    SIGNAL(textChanged(QString)), SLOT(perplexityChanged(QString)));
    connect(&exaggeration,  SIGNAL(textChanged(QString)), SLOT(exaggerationChanged(QString)));
    connect(&expDecay,      SIGNAL(textChanged(QString)), SLOT(expDecayChanged(QString)));
    connect(&numTrees,      SIGNAL(textChanged(QString)), SLOT(numTreesChanged(QString)));
    connect(&numChecks,     SIGNAL(textChanged(QString)), SLOT(numChecksChanged(QString)));
    connect(&theta,         SIGNAL(textChanged(QString)), SLOT(thetaChanged(QString)));

    QGroupBox* settingsBox = new QGroupBox("Basic settings");
    QGroupBox* advancedSettingsBox = new QGroupBox("Advanced Settings");
    _dimensionSelectionBox = new QGroupBox("Dimension Selection");
    advancedSettingsBox->setCheckable(true);
    advancedSettingsBox->setChecked(false);
    QLabel* iterationLabel = new QLabel("Iteration Count");
    QLabel* perplexityLabel = new QLabel("Perplexity");
    QLabel* exaggerationLabel = new QLabel("Exaggeration");
    QLabel* expDecayLabel = new QLabel("Exponential Decay");
    QLabel* numTreesLabel = new QLabel("Number of Trees");
    QLabel* numChecksLabel = new QLabel("Number of Checks");

    numIterations.setFixedWidth(50);
    numIterations.setValidator(new QIntValidator(1, 10000, this));
    numIterations.setText("1000");
    perplexity.setFixedWidth(50);
    perplexity.setValidator(new QIntValidator(2, 50, this));
    perplexity.setText("30");
    exaggeration.setFixedWidth(50);
    exaggeration.setValidator(new QIntValidator(1, 10000, this));
    exaggeration.setText("250");
    expDecay.setFixedWidth(50);
    expDecay.setValidator(new QIntValidator(1, 10000, this));
    expDecay.setText("70");
    numTrees.setFixedWidth(50);
    numTrees.setValidator(new QIntValidator(1, 10000, this));
    numTrees.setText("4");
    numChecks.setFixedWidth(50);
    numChecks.setValidator(new QIntValidator(1, 10000, this));
    numChecks.setText("1024");

    QVBoxLayout *settingsLayout = new QVBoxLayout;
    settingsLayout->addWidget(iterationLabel);
    settingsLayout->addWidget(&numIterations);
    settingsLayout->addWidget(perplexityLabel);
    settingsLayout->addWidget(&perplexity);
    settingsLayout->addStretch(1);
    settingsBox->setLayout(settingsLayout);

    QGridLayout* advancedSettingsLayout = new QGridLayout;
    advancedSettingsLayout->addWidget(exaggerationLabel, 0, 0);
    advancedSettingsLayout->addWidget(&exaggeration, 1, 0);
    advancedSettingsLayout->addWidget(expDecayLabel, 0, 1);
    advancedSettingsLayout->addWidget(&expDecay, 1, 1);
    advancedSettingsLayout->addWidget(numTreesLabel, 2, 0);
    advancedSettingsLayout->addWidget(&numTrees, 3, 0);
    advancedSettingsLayout->addWidget(numChecksLabel, 2, 1);
    advancedSettingsLayout->addWidget(&numChecks, 3, 1);
    advancedSettingsBox->setLayout(advancedSettingsLayout);

    startButton.setText("Start Computation");
    startButton.setFixedSize(QSize(150, 50));

    addWidget(&dataOptions);
    addWidget(settingsBox);
    addWidget(_dimensionSelectionBox);
    addWidget(advancedSettingsBox);
    addWidget(&startButton);
}

void TsneSettingsWidget::onNumDimensionsChanged(TsneAnalysisPlugin* analysis, unsigned int numDimensions, std::vector<QString> names)
{
    bool hasNames = names.size() == numDimensions;

    QHBoxLayout* dimSelectionLayout = new QHBoxLayout();
    QVBoxLayout* vLayout0 = new QVBoxLayout();
    QVBoxLayout* vLayout1 = new QVBoxLayout();

    for (int i = 0; i < numDimensions; i++)
    {
        QString name = hasNames ? names[i] : QString("Dim ") + QString::number(i);
        QCheckBox* dimBox = new QCheckBox(name);
        dimBox->setChecked(true);
        connect(dimBox, &QCheckBox::stateChanged, [analysis, i](int checkState) {
            analysis->dimensionToggled(checkState, i);
        }
        );

        if (i < numDimensions / 2)
            vLayout0->addWidget(dimBox);
        else
            vLayout1->addWidget(dimBox);
    }

    dimSelectionLayout->addLayout(vLayout0);
    dimSelectionLayout->addLayout(vLayout1);
    _dimensionSelectionBox->setLayout(dimSelectionLayout);
}

bool TsneSettingsWidget::hasValidSettings() {
    if (!numIterations.hasAcceptableInput())
        return false;
    if (!perplexity.hasAcceptableInput())
        return false;
    if (!exaggeration.hasAcceptableInput())
        return false;
    if (!expDecay.hasAcceptableInput())
        return false;
    if (!numTrees.hasAcceptableInput())
        return false;
    if (!numChecks.hasAcceptableInput())
        return false;

    return true;
}

void TsneSettingsWidget::checkInputStyle(QLineEdit& input)
{
    if (input.hasAcceptableInput())
    {
        input.setStyleSheet("");
    }
    else
    {
        input.setStyleSheet("border: 1px solid red");
    }
}

void TsneSettingsWidget::numIterationsChanged(const QString &value)
{
    checkInputStyle(numIterations);
}

void TsneSettingsWidget::perplexityChanged(const QString &value)
{
    checkInputStyle(perplexity);
}

void TsneSettingsWidget::exaggerationChanged(const QString &value)
{
    checkInputStyle(exaggeration);
}

void TsneSettingsWidget::expDecayChanged(const QString &value)
{
    checkInputStyle(expDecay);
}

void TsneSettingsWidget::numTreesChanged(const QString &value)
{
    checkInputStyle(numTrees);
}

void TsneSettingsWidget::numChecksChanged(const QString &value)
{
    checkInputStyle(numChecks);
}

void TsneSettingsWidget::thetaChanged(const QString& value)
{
    checkInputStyle(theta);
}
