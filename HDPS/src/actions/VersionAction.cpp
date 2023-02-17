#include "VersionAction.h"

namespace hdps::gui {

VersionAction::VersionAction(QObject* parent) :
    HorizontalGroupAction(parent, "Version"),
    _majorVersionAction(this, "Major Version", 0, 100, 1),
    _minorVersionAction(this, "Major Version", 0, 100, 0)
{
    *this << _majorVersionAction;
    *this << _minorVersionAction;

    _majorVersionAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _minorVersionAction.setDefaultWidgetFlags(IntegralAction::SpinBox);

    _majorVersionAction.setPrefix("major: ");
    _minorVersionAction.setPrefix("minor: ");

    _majorVersionAction.setToolTip("Major version number");
    _minorVersionAction.setToolTip("Minor version number");
}

QString VersionAction::getTypeString() const
{
    return "Version";
}

void VersionAction::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    _majorVersionAction.fromParentVariantMap(variantMap);
    _minorVersionAction.fromParentVariantMap(variantMap);
}

QVariantMap VersionAction::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    _majorVersionAction.insertIntoVariantMap(variantMap);
    _minorVersionAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}