#include "TsneSettingsWidget.h"

#include "TsneAnalysisPlugin.h"

using namespace hdps::plugin;

TsneSettingsWidget::TsneSettingsWidget(const TsneAnalysisPlugin* analysis) {
    setFixedWidth(200);

    startButton.setText("Start Computation");
    connect(&dataOptions, SIGNAL(currentIndexChanged(QString)), analysis, SLOT(dataSetPicked(QString)));
    connect(&startButton, SIGNAL(clicked()), analysis, SLOT(startComputation()));
    connect(&numIterations, SIGNAL(textChanged(QString)), SLOT(numIterationsChanged(QString)));

    QGroupBox* settingsBox = new QGroupBox();
    QLabel* iterationLabel = new QLabel("Iteration count");
    numIterations.setFixedWidth(50);
    numIterations.setValidator(new QIntValidator(1, 10000, this));
    numIterations.setText("1000");
    QVBoxLayout *settingsLayout = new QVBoxLayout;
    settingsLayout->addWidget(iterationLabel);
    settingsLayout->addWidget(&numIterations);
    settingsLayout->addStretch(1);
    settingsBox->setLayout(settingsLayout);

    startButton.setFixedSize(QSize(150, 50));

    addWidget(&dataOptions);
    addWidget(settingsBox);
    addWidget(&startButton);
}

bool TsneSettingsWidget::hasValidSettings() {
    if (!numIterations.hasAcceptableInput())
        return false;

    return true;
}

void TsneSettingsWidget::numIterationsChanged(const QString &value) {
    if (numIterations.hasAcceptableInput()) {
        numIterations.setStyleSheet("");
    }
    else {
        numIterations.setStyleSheet("border: 1px solid red");
    }
}
