#include "IntegralRectangleAction.h"

namespace hdps::gui {

IntegralRectangleAction::IntegralRectangleAction(QObject* parent, const QString& title, const QRect& rectangle /*= QRect()*/) :
    RectangleAction<QRect, IntegralRangeAction>(parent, title, rectangle)
{
    _rectangleChanged = [this]() -> void { emit rectangleChanged(getRectangle()); };
}

void IntegralRectangleAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicIntegralRectangleAction = dynamic_cast<IntegralRectangleAction*>(publicAction);

    Q_ASSERT(publicIntegralRectangleAction != nullptr);

    if (publicIntegralRectangleAction == nullptr)
        return;

    if (recursive) {
        getRangeAction(Axis::X).connectToPublicAction(&publicIntegralRectangleAction->getRangeAction(Axis::X), recursive);
        getRangeAction(Axis::Y).connectToPublicAction(&publicIntegralRectangleAction->getRangeAction(Axis::Y), recursive);
    }

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void IntegralRectangleAction::disconnectFromPublicAction(bool recursive)
{
    if (recursive) {
        getRangeAction(Axis::X).disconnectFromPublicAction(recursive);
        getRangeAction(Axis::Y).disconnectFromPublicAction(recursive);
    }

    WidgetAction::disconnectFromPublicAction(recursive);
}

void IntegralRectangleAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    getRangeAction(Axis::X).fromParentVariantMap(variantMap);
    getRangeAction(Axis::Y).fromParentVariantMap(variantMap);
}

QVariantMap IntegralRectangleAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    getRangeAction(Axis::X).insertIntoVariantMap(variantMap);
    getRangeAction(Axis::Y).insertIntoVariantMap(variantMap);

    return variantMap;
}

}