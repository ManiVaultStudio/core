#include "SpadeSettingsWidget.h"

#include "SpadeAnalysisPlugin.h"

#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>

using namespace hdps::plugin;

SpadeSettingsWidget::SpadeSettingsWidget(const SpadeAnalysisPlugin* analysis) {
    setFixedWidth(200);

    connect(&startButton,   SIGNAL(clicked()), analysis, SLOT(startComputation()));

    startButton.setText("Cluster");
    startButton.setFixedSize(QSize(150, 50));

    addWidget(&dataOptions);
    addWidget(&startButton);
}
