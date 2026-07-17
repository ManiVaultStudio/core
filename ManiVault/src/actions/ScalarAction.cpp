#include "ScalarAction.h"
#include "ScalarSourceModel.h"
#include "ScatterplotPlugin.h"

#include <QHBoxLayout>

namespace mv::gui {

ScalarAction::ScalarAction(QObject* parent, const QString& title, const float& minimum /*= 0.0f*/, const float& maximum /*= 100.0f*/, const float& value /*= 0.0f*/) :
    GroupAction(parent, title),
    _magnitudeAction(this, title, minimum, maximum, value),
    _sourceAction(this, QString("%1 source").arg(title)),
    _sourceDatasetPickerAction(this, "Source dataset")
{
    setDefaultWidgetFlags(GroupAction::Horizontal);
    setShowLabels(false);

    addAction(&_magnitudeAction);
    addAction(&_sourceAction);

	_sourceDatasetPickerAction.setDefaultWidgetFlag(OptionAction::Clearable);

    connect(&_sourceAction.getPickerAction(), &OptionAction::currentIndexChanged, this, [this](const std::uint32_t& currentIndex) {
        bool emitSourceSelectionChanged = true;

        if (currentIndex >= ScalarSourceModel::DefaultRow::DatasetStart) {
            _sourceDatasetPickerAction.setCurrentDataset(_sourceAction.getModel().getDataset(currentIndex));

            if (auto scatterplotPlugin = dynamic_cast<ScatterplotPlugin*>(findPluginAncestor())) {
                auto positionDataset            = scatterplotPlugin->getPositionDataset();
                auto scalarSourcePointsDataset  = Dataset<Points>(getCurrentDataset());
                const auto numScalars           = scalarSourcePointsDataset->getNumPoints();
                const auto numPositions         = positionDataset->getNumPoints();

                if (numScalars != numPositions) {
                    emitSourceSelectionChanged = false;

                    scatterplotPlugin->addNotification(QString("The number of points in the scalar source dataset does not match the number of points in the position dataset. (numPositions=%1, numScalars:%2)").arg(QString::number(numPositions), QString::number(numScalars)));
                }
            }
        } else {
            _sourceDatasetPickerAction.setCurrentIndex(-1);
        }

        if (emitSourceSelectionChanged)
            emit sourceSelectionChanged(currentIndex);
    });

    connect(&_sourceDatasetPickerAction, &DatasetPickerAction::datasetPicked, this, [this](const Dataset<>& dataset) {
        if (_currentDataset.isValid()) {
            disconnect(&_currentDataset, &Dataset<>::dataChanged, this, nullptr);
            disconnect(&_currentDataset, &Dataset<>::guiNameChanged, this, nullptr);
        }
            
        _currentDataset = dataset;

        const auto datasetRowIndex = _sourceAction.getModel().getRowIndex(_currentDataset);

        _sourceAction.getPickerAction().setCurrentIndex(_currentDataset.isValid() ? datasetRowIndex : ScalarSourceModel::DefaultRow::Constant);

        if (!_currentDataset.isValid())
            return;

        connect(&_currentDataset, &Dataset<>::dataChanged, this, [this]() -> void {
            emit sourceDataChanged(getCurrentDataset());
        });

        connect(&_currentDataset, &Dataset<>::guiNameChanged, this, [this]() -> void {
            if (auto scatterplotPlugin = dynamic_cast<ScatterplotPlugin*>(findPluginAncestor()))
                scatterplotPlugin->updateHeadsUpDisplay();
        });
    });

    connect(&_magnitudeAction, &DecimalAction::valueChanged, this, [this](const float& value) {
        emit magnitudeChanged(value);
    });

    connect(&_sourceAction, &ScalarSourceAction::scalarRangeChanged, this, [this](const float& minimum, const float& maximum) {
        emit scalarRangeChanged(minimum, maximum);
    });

    connect(&_sourceAction.getOffsetAction(), &DecimalAction::valueChanged, this, [this](const float& value) {
        emit offsetChanged(value);
    });
}

void ScalarAction::addDataset(const Dataset<DatasetImpl>& dataset)
{
    auto& sourceModel = _sourceAction.getModel();

    if (sourceModel.hasDataset(dataset))
        return;

    sourceModel.addDataset(dataset);

    /* TODO: this connection is not removed when the dataset is removed from the model, but that should not cause any issues since the dataset will be invalid and the connection will not do anything in that case
    connect(&sourceModel.getDatasets().last(), &Dataset<DatasetImpl>::dataChanged, this, [this, dataset]() {
        const auto currentDataset = getCurrentDataset();

        if (!currentDataset.isValid())
            return;

        _sourceAction.updateScalarRange();

        if (currentDataset == dataset)
            emit sourceDataChanged(dataset);
    });

*/

    connect(&_magnitudeAction, &DecimalAction::valueChanged, this, [this, dataset](const float& value) {
        emit magnitudeChanged(value);
    });
}

void ScalarAction::removeAllDatasets()
{
    _sourceAction.getModel().removeAllDatasets();
}

Dataset<DatasetImpl> ScalarAction::getCurrentDataset()
{
    return _sourceDatasetPickerAction.getCurrentDataset();
}

void ScalarAction::setCurrentDataset(const Dataset<DatasetImpl>& dataset)
{
    _sourceDatasetPickerAction.setCurrentDataset(dataset);
}

void ScalarAction::setCurrentSourceIndex(std::int32_t sourceIndex)
{
    _sourceAction.getPickerAction().setCurrentIndex(sourceIndex);
}

bool ScalarAction::isSourceConstant() const
{
    return _sourceAction.getPickerAction().getCurrentIndex() == ScalarSourceModel::DefaultRow::Constant;
}

bool ScalarAction::isSourceSelection() const
{
    return _sourceAction.getPickerAction().getCurrentIndex() == ScalarSourceModel::DefaultRow::Selection;
}

bool ScalarAction::isSourceDataset() const
{
    return _sourceAction.getPickerAction().getCurrentIndex() >= ScalarSourceModel::DefaultRow::DatasetStart;
}

void ScalarAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicScalarAction = dynamic_cast<ScalarAction*>(publicAction);

    Q_ASSERT(publicScalarAction != nullptr);

    if (publicScalarAction == nullptr)
        return;

    if (recursive) {
        actions().connectPrivateActionToPublicAction(&_magnitudeAction, &publicScalarAction->getMagnitudeAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_sourceAction, &publicScalarAction->getSourceAction(), recursive);
    }

    GroupAction::connectToPublicAction(publicAction, recursive);
}

void ScalarAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        actions().disconnectPrivateActionFromPublicAction(&_magnitudeAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_sourceAction, recursive);
    }

    GroupAction::disconnectFromPublicAction(recursive);
}

void ScalarAction::fromVariantMap(const QVariantMap& variantMap)
{
    GroupAction::fromVariantMap(variantMap);

    _magnitudeAction.fromParentVariantMap(variantMap);
    _sourceAction.fromParentVariantMap(variantMap);
    //_sourceDatasetPickerAction.fromParentVariantMap(variantMap);
}

QVariantMap ScalarAction::toVariantMap() const
{
    auto variantMap = GroupAction::toVariantMap();

    _magnitudeAction.insertIntoVariantMap(variantMap);
    _sourceAction.insertIntoVariantMap(variantMap);
    //_sourceDatasetPickerAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
