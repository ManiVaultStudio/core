#include "VariantAction.h"

namespace hdps::gui {

VariantAction::VariantAction(QObject* parent, const QString& title /*= ""*/, const QVariant& variant /*= QVariant()*/) :
    WidgetAction(parent, title),
    _variant()
{
    setText(title);
    initialize(variant);
}

void VariantAction::initialize(const QVariant& variant /*= QVariant()*/)
{
    setVariant(variant);
}

QVariant VariantAction::getVariant() const
{
    return _variant;
}

void VariantAction::setVariant(const QVariant& variant)
{
    if (variant == _variant)
        return;

    _variant = variant;

    emit variantChanged(_variant);

    saveToSettings();
}

void VariantAction::connectToPublicAction(WidgetAction* publicAction)
{
    auto publicVariantAction = dynamic_cast<VariantAction*>(publicAction);

    Q_ASSERT(publicVariantAction != nullptr);

    connect(this, &VariantAction::variantChanged, publicVariantAction, &VariantAction::setVariant);
    connect(publicVariantAction, &VariantAction::variantChanged, this, &VariantAction::setVariant);

    setVariant(publicVariantAction->getVariant());

    WidgetAction::connectToPublicAction(publicAction);
}

void VariantAction::disconnectFromPublicAction()
{
    auto publicStringAction = dynamic_cast<VariantAction*>(getPublicAction());

    Q_ASSERT(publicStringAction != nullptr);

    disconnect(this, &VariantAction::variantChanged, publicStringAction, &VariantAction::setVariant);
    disconnect(publicStringAction, &VariantAction::variantChanged, this, &VariantAction::setVariant);

    WidgetAction::disconnectFromPublicAction();
}

WidgetAction* VariantAction::getPublicCopy() const
{
    return new VariantAction(parent(), text(), getVariant());
}

void VariantAction::fromVariantMap(const QVariantMap& variantMap)
{
}

QVariantMap VariantAction::toVariantMap() const
{
    return {};
}

}
