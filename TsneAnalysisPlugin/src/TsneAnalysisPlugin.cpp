#include "TsneAnalysisPlugin.h"

#include "PointsPlugin.h"

#include <QtCore>
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

    tsne = std::make_unique<MCV_TsneAnalysis>();
}

void TsneAnalysisPlugin::dataAdded(const QString name)
{
    if (name == "Embedding") {
        return;
    }

    const hdps::Set* set = _core->requestData(name);
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

}

// =============================================================================
// Factory
// =============================================================================

AnalysisPlugin* TsneAnalysisPluginFactory::produce()
{
    return new TsneAnalysisPlugin();
}
