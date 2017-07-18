#ifndef TSNE_ANALYSIS_PLUGIN_H
#define TSNE_ANALYSIS_PLUGIN_H

#include <AnalysisPlugin.h>

#include "TsneAnalysis.h"
#include <widgets/SettingsWidget.h>

#include <QObject>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>

using namespace hdps::plugin;
using namespace hdps::gui;

// =============================================================================
// View
// =============================================================================

class TsneAnalysisPlugin : public QObject, public AnalysisPlugin
{
    Q_OBJECT   
public:
    TsneAnalysisPlugin() : AnalysisPlugin("tSNE Analysis") { }
    ~TsneAnalysisPlugin(void);
    
    void init();

    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
    void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
    QStringList supportedDataKinds() Q_DECL_OVERRIDE;
    void onEmbeddingUpdate() const;
public slots:
    void dataSetPicked(const QString& name);
    void startComputation();
private:
    std::unique_ptr<TsneAnalysis> tsne;
    QString _embedSetName;
};

class TsneSettingsWidget : public SettingsWidget
{
    Q_OBJECT

public:
    TsneSettingsWidget(const TsneAnalysisPlugin* const analysis) {
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

    bool hasValidSettings() {
        if (!numIterations.hasAcceptableInput())
            return false;

        return true;
    }

private slots:
    void numIterationsChanged(const QString &value) {
        if (numIterations.hasAcceptableInput()) {
            numIterations.setStyleSheet("");
        }
        else {
            numIterations.setStyleSheet("border: 1px solid red");
        }
    }
public:
    QComboBox dataOptions;
    QLineEdit numIterations;
    QPushButton startButton;
};

// =============================================================================
// Factory
// =============================================================================

class TsneAnalysisPluginFactory : public AnalysisPluginFactory
{
    Q_INTERFACES(hdps::plugin::AnalysisPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.TsneAnalysisPlugin"
                      FILE  "TsneAnalysisPlugin.json")
    
public:
    TsneAnalysisPluginFactory(void) {}
    ~TsneAnalysisPluginFactory(void) {}
    
    AnalysisPlugin* produce();
};

#endif // TSNE_ANALYSIS_PLUGIN_H
