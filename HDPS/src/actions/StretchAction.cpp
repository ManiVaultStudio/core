#include "StretchAction.h"

namespace hdps::gui {

StretchAction::StretchAction(QObject* parent, const QString& title, std::int32_t stretch /*= 1*/) :
    WidgetAction(parent, title),
    _stretch(stretch)
{
}

void StretchAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicStretchAction = dynamic_cast<StretchAction*>(publicAction);

    Q_ASSERT(publicStretchAction != nullptr);

    if (publicStretchAction == nullptr)
        return;

    connect(this, &StretchAction::stretchChanged, publicStretchAction, &StretchAction::setStretch);
    connect(publicStretchAction, &StretchAction::stretchChanged, this, &StretchAction::setStretch);

    setStretch(publicStretchAction->getStretch());

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void StretchAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicStretchAction = dynamic_cast<StretchAction*>(getPublicAction());

    Q_ASSERT(publicStretchAction != nullptr);

    if (publicStretchAction == nullptr)
        return;

    disconnect(this, &StretchAction::stretchChanged, publicStretchAction, &StretchAction::setStretch);
    disconnect(publicStretchAction, &StretchAction::stretchChanged, this, &StretchAction::setStretch);

    WidgetAction::disconnectFromPublicAction(recursive);
}

void StretchAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);
}

QVariantMap StretchAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    return variantMap;
}

}