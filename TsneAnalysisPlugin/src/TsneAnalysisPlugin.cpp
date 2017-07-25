#include "TsneAnalysisPlugin.h"

#include "PointsPlugin.h"

#include <QtCore>
#include <QMessageBox>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.TsneAnalysisPlugin")

// =============================================================================
// View
// =============================================================================

TsneAnalysisPlugin::~TsneAnalysisPlugin(void)
{
    
}

void TsneAnalysisPlugin::init()
{
    _settings = std::make_unique<TsneSettingsWidget>(this);

    tsne = std::make_unique<TsneAnalysis>();
}

void TsneAnalysisPlugin::dataAdded(const QString name)
{
    const hdps::Set* set = _core->requestData(name);
    const DataTypePlugin* dataPlugin = _core->requestPlugin(set->getDataName());

    TsneSettingsWidget* tsneSettings = dynamic_cast<TsneSettingsWidget*>(_settings.get());
    if (dataPlugin->getKind() == "Points") {
        tsneSettings->dataOptions.addItem(name);
    }
}

void TsneAnalysisPlugin::dataChanged(const QString name)
{
    TsneSettingsWidget* tsneSettings = dynamic_cast<TsneSettingsWidget*>(_settings.get());
    if (name != tsneSettings->dataOptions.currentText()) {
        return;
    }
}

void TsneAnalysisPlugin::dataRemoved(const QString name)
{
    
}

void TsneAnalysisPlugin::selectionChanged(const QString dataName)
{

}


QStringList TsneAnalysisPlugin::supportedDataKinds()
{
    QStringList supportedKinds;
    supportedKinds << "Points";
    return supportedKinds;
}

void TsneAnalysisPlugin::onEmbeddingUpdate() const
{
    std::vector<float>* output = tsne->output();
    const hdps::Set* embedSet = _core->requestData(_embedSetName);
    PointsPlugin* embedPoints = dynamic_cast<PointsPlugin*>(_core->requestPlugin(embedSet->getDataName()));

    embedPoints->data = *output;

    _core->notifyDataChanged(_embedSetName);
}

void TsneAnalysisPlugin::dataSetPicked(const QString& name)
{

}

void TsneAnalysisPlugin::startComputation()
{
    TsneSettingsWidget* tsneSettings = dynamic_cast<TsneSettingsWidget*>(_settings.get());

    // Do nothing if we have no data set selected
    if (tsneSettings->dataOptions.currentText().isEmpty()) {
        return;
    }

    // Check if the tSNE settings are valid before running the computation
    if (!tsneSettings->hasValidSettings()) {
        QMessageBox warningBox;
        warningBox.setText(tr("Some settings are invalid or missing. Continue with default values?"));
        QPushButton *continueButton = warningBox.addButton(tr("Continue"), QMessageBox::ActionRole);
        QPushButton *abortButton = warningBox.addButton(QMessageBox::Abort);

        warningBox.exec();

        if (warningBox.clickedButton() == abortButton) {
            return;
        }
    }

    // Initialize the tSNE computation with the settings from the settings widget
    tsne->setIterations(tsneSettings->numIterations.text().toInt());
    tsne->setPerplexity(tsneSettings->perplexity.text().toInt());
    tsne->setExaggerationIter(tsneSettings->exaggeration.text().toInt());
    tsne->setExpDecay(tsneSettings->expDecay.text().toInt());
    tsne->setNumTrees(tsneSettings->numTrees.text().toInt());
    tsne->setNumChecks(tsneSettings->numChecks.text().toInt());

    // Run the computation
    QString setName = tsneSettings->dataOptions.currentText();
    const hdps::Set* set = _core->requestData(setName);
    const DataTypePlugin* dataPlugin = _core->requestPlugin(set->getDataName());
    const PointsPlugin* points = dynamic_cast<const PointsPlugin*>(dataPlugin);

    _embedSetName = _core->addData("Points", "Embedding");
    const hdps::Set* embedSet = _core->requestData(_embedSetName);
    PointsPlugin* embedPoints = dynamic_cast<PointsPlugin*>(_core->requestPlugin(embedSet->getDataName()));

    embedPoints->numDimensions = 2;
    _core->notifyDataAdded(_embedSetName);

    tsne->initTSNE(&points->data, points->numDimensions);

    tsne->computeGradientDescent(*this);
}

// =============================================================================
// Factory
// =============================================================================

AnalysisPlugin* TsneAnalysisPluginFactory::produce()
{
    return new TsneAnalysisPlugin();
}
