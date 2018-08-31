#include "TsneSettingsWidget.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDebug>
#include <QScrollArea>

std::vector<bool> DimensionPickerWidget::getEnabledDimensions() const
{
    std::vector<bool> enabledDimensions(_checkBoxes.size());

    for (unsigned int i = 0; i < _checkBoxes.size(); i++)
        enabledDimensions[i] = _checkBoxes[i]->isChecked();

    return enabledDimensions;
}

void DimensionPickerWidget::setDimensions(unsigned int numDimensions, std::vector<QString> names)
{
    bool hasNames = names.size() == numDimensions;

    clearWidget();

    for (int i = 0; i < numDimensions; i++)
    {
        QString name = hasNames ? names[i] : QString("Dim ") + QString::number(i);
        QCheckBox* widget = new QCheckBox(name);
        widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        widget->setMinimumHeight(20);
        widget->setToolTip(name);
        widget->setChecked(true);

        _checkBoxes.push_back(widget);
        int row = i % (numDimensions / 2);
        int column = i / (numDimensions / 2);

        _layout.addWidget(widget, row, column);
    }

    resize(160, 20 * numDimensions / 2);
}

void DimensionPickerWidget::clearWidget()
{
    for (QCheckBox* widget : _checkBoxes)
    {
        _layout.removeWidget(widget);
        delete widget;
    }

    _checkBoxes.clear();
}

TsneSettingsWidget::TsneSettingsWidget() {
    setFixedWidth(200);

    connect(&dataOptions,   SIGNAL(currentIndexChanged(QString)), this, SIGNAL(dataSetPicked(QString)));
    connect(&startButton,   &QPushButton::toggled, this, &TsneSettingsWidget::onStartToggled);

    connect(&numIterations, SIGNAL(textChanged(QString)), SLOT(numIterationsChanged(QString)));
    connect(&perplexity,    SIGNAL(textChanged(QString)), SLOT(perplexityChanged(QString)));
    connect(&exaggeration,  SIGNAL(textChanged(QString)), SLOT(exaggerationChanged(QString)));
    connect(&expDecay,      SIGNAL(textChanged(QString)), SLOT(expDecayChanged(QString)));
    connect(&numTrees,      SIGNAL(textChanged(QString)), SLOT(numTreesChanged(QString)));
    connect(&numChecks,     SIGNAL(textChanged(QString)), SLOT(numChecksChanged(QString)));
    connect(&theta,         SIGNAL(textChanged(QString)), SLOT(thetaChanged(QString)));

    // Create group boxes for grouping together various settings
    QGroupBox* settingsBox = new QGroupBox("Basic settings");
    QGroupBox* advancedSettingsBox = new QGroupBox("Advanced Settings");
    QGroupBox* dimensionSelectionBox = new QGroupBox("Dimension Selection");
    
    advancedSettingsBox->setCheckable(true);
    advancedSettingsBox->setChecked(false);
    
    // Build the labels for all the options
    QLabel* iterationLabel = new QLabel("Iteration Count");
    QLabel* perplexityLabel = new QLabel("Perplexity");
    QLabel* exaggerationLabel = new QLabel("Exaggeration");
    QLabel* expDecayLabel = new QLabel("Exponential Decay");
    QLabel* numTreesLabel = new QLabel("Number of Trees");
    QLabel* numChecksLabel = new QLabel("Number of Checks");

    // Set option default values
    numIterations.setFixedWidth(50);
    perplexity.setFixedWidth(50);
    exaggeration.setFixedWidth(50);
    expDecay.setFixedWidth(50);
    numTrees.setFixedWidth(50);
    numChecks.setFixedWidth(50);

    numIterations.setValidator(new QIntValidator(1, 10000, this));
    perplexity.setValidator(new QIntValidator(2, 50, this));
    exaggeration.setValidator(new QIntValidator(1, 10000, this));
    expDecay.setValidator(new QIntValidator(1, 10000, this));
    numTrees.setValidator(new QIntValidator(1, 10000, this));
    numChecks.setValidator(new QIntValidator(1, 10000, this));

    numIterations.setText("1000");
    perplexity.setText("30");
    exaggeration.setText("250");
    expDecay.setText("70");
    numTrees.setText("4");
    numChecks.setText("1024");

    startButton.setText("Start Computation");
    startButton.setFixedSize(QSize(150, 50));
    startButton.setCheckable(true);

    // Add options to their appropriate group box
    QVBoxLayout* settingsLayout = new QVBoxLayout();
    settingsLayout->addWidget(iterationLabel);
    settingsLayout->addWidget(&numIterations);
    settingsLayout->addWidget(perplexityLabel);
    settingsLayout->addWidget(&perplexity);
    settingsBox->setLayout(settingsLayout);

    QVBoxLayout* dimensionSelectionLayout = new QVBoxLayout();
    QScrollArea* scroller = new QScrollArea();
    scroller->setMinimumHeight(50);
    scroller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    scroller->setWidget(&_dimensionPickerWidget);
    dimensionSelectionLayout->addWidget(scroller);
    dimensionSelectionBox->setLayout(dimensionSelectionLayout);

    QGridLayout* advancedSettingsLayout = new QGridLayout();
    advancedSettingsLayout->addWidget(exaggerationLabel, 0, 0);
    advancedSettingsLayout->addWidget(&exaggeration, 1, 0);
    advancedSettingsLayout->addWidget(expDecayLabel, 0, 1);
    advancedSettingsLayout->addWidget(&expDecay, 1, 1);
    advancedSettingsLayout->addWidget(numTreesLabel, 2, 0);
    advancedSettingsLayout->addWidget(&numTrees, 3, 0);
    advancedSettingsLayout->addWidget(numChecksLabel, 2, 1);
    advancedSettingsLayout->addWidget(&numChecks, 3, 1);
    advancedSettingsBox->setLayout(advancedSettingsLayout);

    // Add all the parts of the settings widget together
    addWidget(&dataOptions);
    addWidget(settingsBox);
    addWidget(dimensionSelectionBox);
    addWidget(advancedSettingsBox);
    addWidget(&startButton);
}



// Communication with the dimension picker widget
void TsneSettingsWidget::onNumDimensionsChanged(TsneAnalysisPlugin* analysis, unsigned int numDimensions, std::vector<QString> names)
{
    _dimensionPickerWidget.setDimensions(numDimensions, names);
}

std::vector<bool> TsneSettingsWidget::getEnabledDimensions()
{
    return _dimensionPickerWidget.getEnabledDimensions();
}


QString TsneSettingsWidget::currentData()
{
    return dataOptions.currentText();
}

// Check if all input values are valid
bool TsneSettingsWidget::hasValidSettings()
{
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


// SLOTS
void TsneSettingsWidget::onStartToggled(bool pressed)
{
    // Do nothing if we have no data set selected
    if (dataOptions.currentText().isEmpty()) {
        return;
    }

    // Check if the tSNE settings are valid before running the computation
    if (!hasValidSettings()) {
        QMessageBox warningBox;
        warningBox.setText(tr("Some settings are invalid or missing. Continue with default values?"));
        QPushButton *continueButton = warningBox.addButton(tr("Continue"), QMessageBox::ActionRole);
        QPushButton *abortButton = warningBox.addButton(QMessageBox::Abort);

        warningBox.exec();

        if (warningBox.clickedButton() == abortButton) {
            return;
        }
    }
    startButton.setText(pressed ? "Stop Computation" : "Start Computation");
    emit pressed ? startComputation() : stopComputation();
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
