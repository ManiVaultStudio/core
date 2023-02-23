#include "VersionAction.h"

#include <QStringListModel>

namespace hdps::gui {

VersionAction::VersionAction(QObject* parent) :
    HorizontalGroupAction(parent, "Version"),
    _majorAction(this, "Major Version", 0, 100, 1),
    _minorAction(this, "Major Version", 0, 100, 0),
    _suffixAction(this, "Suffix"),
    _versionStringAction(this, "Version String"),
    _suffixCompleter()
{
    setShowLabels(false);

    addAction(&_majorAction);
    addAction(&_minorAction);
    addAction(&_suffixAction);

    _majorAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _minorAction.setDefaultWidgetFlags(IntegralAction::SpinBox);

    _majorAction.setPrefix("major: ");
    _majorAction.setToolTip("Major version number");
    _majorAction.setStretch(1);
    _majorAction.setSerializationName("Major");

    _minorAction.setPrefix("minor: ");
    _minorAction.setToolTip("Minor version number");
    _minorAction.setStretch(1);
    _majorAction.setSerializationName("Minor");

    _suffixAction.setPlaceHolderString("Enter suffix here...");
    _suffixAction.setClearable(true);
    _suffixAction.setStretch(3);
    _suffixAction.setCompleter(&_suffixCompleter);
    _suffixAction.setSerializationName("Suffix");

    _suffixCompleter.setModel(new QStringListModel({ "Alpha", "Beta", "Release Candidate", "Pre-release", "alpha", "beta", "rc", "pre-release" }));

    const auto updateVersionStringAction = [this]() -> void {
        _versionStringAction.setString(QString("%1.%2 %3").arg(QString::number(_majorAction.getValue()), QString::number(_minorAction.getValue()), _suffixAction.getString()));
    };

    updateVersionStringAction();

    connect(&_majorAction, &IntegralAction::valueChanged, this, updateVersionStringAction);
    connect(&_minorAction, &IntegralAction::valueChanged, this, updateVersionStringAction);
    connect(&_suffixAction, &StringAction::stringChanged, this, updateVersionStringAction);
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
    _suffixAction.fromParentVariantMap(variantMap);
}

QVariantMap VersionAction::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    _majorAction.insertIntoVariantMap(variantMap);
    _minorAction.insertIntoVariantMap(variantMap);
    _suffixAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}