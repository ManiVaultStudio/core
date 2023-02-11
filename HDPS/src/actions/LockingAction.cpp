#include "LockingAction.h"

#include "Application.h"

#include <QDebug>

using namespace hdps::util;

namespace hdps::gui {

LockingAction::LockingAction(QObject* parent, const QString& what /*= ""*/, bool locked /*= false*/) :
    GroupAction(parent, "Locking"),
    _lockedAction(this),
    _lockAction(this),
    _unlockAction(this)
{
    setCheckable(true);
    setText("Locking");
    initialize(locked);
    updateActionsText();

    setSerializationName("Locking");

    _lockedAction.setSerializationName("Locked");
}

void LockingAction::initialize(bool locked /*= false*/)
{
    _lockedAction.setIcon(Application::getIconFont("FontAwesome").getIcon("lock"));
    _lockAction.setIcon(Application::getIconFont("FontAwesome").getIcon("lock"));
    _unlockAction.setIcon(Application::getIconFont("FontAwesome").getIcon("unlock"));

    _lockAction.setToolTip("Lock");
    _unlockAction.setToolTip("Unlock");

    _lockedAction.setChecked(locked);

    connect(&_lockedAction, &gui::ToggleAction::toggled, this, [this](bool toggled) -> void {
        _lockAction.setEnabled(!toggled);
        _unlockAction.setEnabled(toggled);

        emit lockedChanged(toggled);
    });

    connect(&_lockAction, &gui::TriggerAction::triggered, this, [this]() -> void {
        _lockedAction.setChecked(true);
    });

    connect(&_unlockAction, &gui::TriggerAction::triggered, this, [this]() -> void {
        _lockedAction.setChecked(false);
    });
}

void LockingAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _lockedAction.fromParentVariantMap(variantMap);
}

QVariantMap LockingAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    _lockedAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

bool LockingAction::isLocked() const
{
    return _lockedAction.isChecked();
}

void LockingAction::setLocked(bool locked)
{
    _lockedAction.setChecked(locked);
}

QString LockingAction::getWhat() const
{
    return _what;
}

void LockingAction::setWhat(QString what)
{
    _what = what;

    updateActionsText();
}

void LockingAction::updateActionsText()
{
    _lockedAction.setText(QString("%1 locked").arg(_what));
    _lockAction.setText(QString("Lock %1").arg(_what));
    _unlockAction.setText(QString("Unlock %1").arg(_what));
}

}
