#include "VersionAction.h"

namespace hdps::gui {

VersionAction::VersionAction(QObject* parent) :
    HorizontalGroupAction(parent, "Version"),
    _majorAction(this, "Major Version", 0, 100, 1),
    _minorAction(this, "Major Version", 0, 100, 0),
    _versionStringAction(this, "Version String")
{
    setShowLabels(false);

    addAction(_majorAction);
    addAction(_minorAction);

    _majorAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _minorAction.setDefaultWidgetFlags(IntegralAction::SpinBox);

    _majorAction.setPrefix("major: ");
    _minorAction.setPrefix("minor: ");

    _majorAction.setToolTip("Major version number");
    _minorAction.setToolTip("Minor version number");

    const auto updateVersionStringAction = [this]() -> void {
        _versionStringAction.setString(QString("%1.%2").arg(QString::number(_majorAction.getValue()), QString::number(_minorAction.getValue())));
    };

    updateVersionStringAction();

    connect(&_majorAction, &IntegralAction::valueChanged, this, updateVersionStringAction);
    connect(&_minorAction, &IntegralAction::valueChanged, this, updateVersionStringAction);
}

QString VersionAction::getTypeString() const
{
    return "Version";
}

void VersionAction::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    _majorAction.fromParentVariantMap(variantMap);
    _minorAction.fromParentVariantMap(variantMap);
}

QVariantMap VersionAction::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    _majorAction.insertIntoVariantMap(variantMap);
    _minorAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}