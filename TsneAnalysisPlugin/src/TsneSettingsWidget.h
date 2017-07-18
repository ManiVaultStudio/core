#ifndef TSNE_SETTINGS_WIDGET_H
#define TSNE_SETTINGS_WIDGET_H

#include <widgets/SettingsWidget.h>

#include <QObject>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>

using namespace hdps::gui;

class TsneAnalysisPlugin;

class TsneSettingsWidget : public SettingsWidget
{
    Q_OBJECT

public:
    TsneSettingsWidget(const TsneAnalysisPlugin* analysis);

    bool hasValidSettings();

private slots:
    void numIterationsChanged(const QString &value);
public:
    QComboBox dataOptions;
    QLineEdit numIterations;
    QPushButton startButton;
};

#endif // TSNE_SETTINGS_WIDGET_H
