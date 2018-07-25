#ifndef TSNE_SETTINGS_WIDGET_H
#define TSNE_SETTINGS_WIDGET_H

#include <widgets/SettingsWidget.h>

#include <QObject>
#include <QGroupBox>
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

    void onNumDimensionsChanged(TsneAnalysisPlugin* analysis, unsigned int numDimensions, std::vector<QString> names);
private:
    void checkInputStyle(QLineEdit& input);

private slots:
    void numIterationsChanged(const QString &value);
    void perplexityChanged(const QString &value);
    void exaggerationChanged(const QString &value);
    void expDecayChanged(const QString &value);
    void numTreesChanged(const QString &value);
    void numChecksChanged(const QString &value);
    void thetaChanged(const QString& value);

public:
    QGroupBox* _dimensionSelectionBox;

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
