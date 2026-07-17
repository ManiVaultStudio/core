#include "ScalarSourceAction.h"

#include <PointData/PointData.h>

namespace mv::gui {

ScalarSourceAction::ScalarSourceAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _model(this),
    _pickerAction(this, "Source"),
    _dimensionPickerAction(this, "Data dimension"),
    _offsetAction(this, "Offset", 0.0f, 100.0f, 0.0f, 2),
    _rangeAction(this, "Scalar range")
{
    setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    setPopupSizeHint(QSize(250, 0));
    setConnectionPermissionsToForceNone(true);

    addAction(&_pickerAction);
    addAction(&_dimensionPickerAction);
    addAction(&_offsetAction);
    addAction(&_rangeAction);

    _pickerAction.setCustomModel(&_model);
    _pickerAction.setToolTip("Pick scalar option");

    _offsetAction.setSuffix("px");

    const auto scalarSourceChanged = [this]() -> void {
        const auto sourceIndex = _pickerAction.getCurrentIndex();

        _dimensionPickerAction.setEnabled(sourceIndex >= ScalarSourceModel::DefaultRow::DatasetStart);

        setIcon(_model.data(_model.index(sourceIndex, 0), Qt::DecorationRole).value<QIcon>());

        QSignalBlocker dimensionPickerActionBlocker(&_dimensionPickerAction);

        _dimensionPickerAction.setPointsDataset(Dataset<Points>(_model.getDataset(_pickerAction.getCurrentIndex())));

        _offsetAction.setEnabled(sourceIndex > ScalarSourceModel::DefaultRow::Constant);

        updateScalarRange();
    };

    connect(&_pickerAction, &OptionAction::currentIndexChanged, this, scalarSourceChanged);

    connect(&_dimensionPickerAction, &DimensionPickerAction::currentDimensionIndexChanged, this, &ScalarSourceAction::updateScalarRange);

    connect(&_rangeAction.getRangeMinAction(), &DecimalAction::valueChanged, this, [this]() {
        emit scalarRangeChanged(_rangeAction.getRangeMinAction().getValue(), _rangeAction.getRangeMaxAction().getValue());
    });

    connect(&_rangeAction.getRangeMaxAction(), &DecimalAction::valueChanged, this, [this]() {
        emit scalarRangeChanged(_rangeAction.getRangeMinAction().getValue(), _rangeAction.getRangeMaxAction().getValue());
    });

    scalarSourceChanged();
}

ScalarSourceModel& ScalarSourceAction::getModel()
{
    return _model;
}

void ScalarSourceAction::updateScalarRange()
{
    auto points = Dataset<Points>(_model.getDataset(_pickerAction.getCurrentIndex()));

    float minimum = 0.0f;
    float maximum = 1.0f;

    const auto hasScalarRange = points.isValid() && _pickerAction.getCurrentIndex() >= 1;

    if (hasScalarRange) {
        minimum = std::numeric_limits<float>::max();
        maximum = std::numeric_limits<float>::lowest();

        points->visitData([this, &minimum, &maximum, points](auto pointData) {
            const auto currentDimensionIndex = _dimensionPickerAction.getCurrentDimensionIndex();

            for (std::uint32_t pointIndex = 0; pointIndex < points->getNumPoints(); pointIndex++) {
                auto pointValue = static_cast<float>(pointData[pointIndex][currentDimensionIndex]);
                
                minimum = std::min(minimum, pointValue);
                maximum = std::max(maximum, pointValue);
            }
        });
    }

    _rangeAction.getRangeMinAction().initialize(minimum, maximum, minimum, 1);
    _rangeAction.getRangeMaxAction().initialize(minimum, maximum, maximum, 1);

    _rangeAction.getRangeMinAction().setEnabled(hasScalarRange);
    _rangeAction.getRangeMaxAction().setEnabled(hasScalarRange);

    emit scalarRangeChanged(minimum, maximum);
}

void ScalarSourceAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicScalarSourceAction = dynamic_cast<ScalarSourceAction*>(publicAction);

    Q_ASSERT(publicScalarSourceAction != nullptr);

    if (publicScalarSourceAction == nullptr)
        return;

    if (recursive) {
        actions().connectPrivateActionToPublicAction(&_pickerAction, &publicScalarSourceAction->getPickerAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_dimensionPickerAction, &publicScalarSourceAction->getDimensionPickerAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_offsetAction, &publicScalarSourceAction->getOffsetAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_rangeAction, &publicScalarSourceAction->getRangeAction(), recursive);
    }

    GroupAction::connectToPublicAction(publicAction, recursive);
}

void ScalarSourceAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        actions().disconnectPrivateActionFromPublicAction(&_pickerAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_dimensionPickerAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_offsetAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_rangeAction, recursive);
    }

    GroupAction::disconnectFromPublicAction(recursive);
}

void ScalarSourceAction::fromVariantMap(const QVariantMap& variantMap)
{
    GroupAction::fromVariantMap(variantMap);

    _pickerAction.fromParentVariantMap(variantMap);
    _dimensionPickerAction.fromParentVariantMap(variantMap);
    _offsetAction.fromParentVariantMap(variantMap);
    _rangeAction.fromParentVariantMap(variantMap);
}

QVariantMap ScalarSourceAction::toVariantMap() const
{
    auto variantMap = GroupAction::toVariantMap();

    _pickerAction.insertIntoVariantMap(variantMap);
    _dimensionPickerAction.insertIntoVariantMap(variantMap);
    _offsetAction.insertIntoVariantMap(variantMap);
    _rangeAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
