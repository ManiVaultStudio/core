#include "MeanShiftSettingsWidget.h"

#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>

#include <QIntValidator>
#include <QDoubleValidator>

MeanShiftSettingsWidget::MeanShiftSettingsWidget()
{
    setFixedWidth(200);

    //connect(&ui_dataOptions,   SIGNAL(currentIndexChanged(QString)), analysis, SLOT(dataSetPicked(QString)));
    connect(&_startButton, SIGNAL(clicked()), this, SIGNAL(startClustering()));

    _startButton.setText("Cluster");
    _startButton.setFixedSize(QSize(150, 50));

    addWidget(&_dataOptions);
    addWidget(&_startButton);
}

void MeanShiftSettingsWidget::addDataOption(QString option)
{
    _dataOptions.addItem(option);
}

QString MeanShiftSettingsWidget::getCurrentDataOption()
{
    return _dataOptions.currentText();
}

const SpadeSettings& MeanShiftSettingsWidget::getSpadeSettings() const
{
    return spadeSettings;
}
