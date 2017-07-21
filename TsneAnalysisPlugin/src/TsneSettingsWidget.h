#ifndef TSNE_SETTINGS_WIDGET_H
#define TSNE_SETTINGS_WIDGET_H

#include <widgets/SettingsWidget.h>

#include <QObject>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>

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
    QLineEdit perplexity;
    QLineEdit exaggeration;
    QLineEdit expDecay;
    QLineEdit numTrees;
    QLineEdit numChecks;
    QLineEdit theta;
    QPushButton startButton;
};

#endif // TSNE_SETTINGS_WIDGET_H
