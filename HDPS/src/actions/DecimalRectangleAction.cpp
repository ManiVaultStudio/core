#include "DecimalRectangleAction.h"

namespace hdps::gui {

DecimalRectangleAction::DecimalRectangleAction(QObject * parent, const QString& title, const QRectF& rectangle /*= QRectF()*/) :
    RectangleAction<QRectF, DecimalRangeAction>(parent, title, rectangle)
{
    _rectangleChanged = [this]() -> void { emit rectangleChanged(getRectangle()); };
}

void DecimalRectangleAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicDecimalRectangleAction = dynamic_cast<DecimalRectangleAction*>(publicAction);

    Q_ASSERT(publicDecimalRectangleAction != nullptr);

    if (publicDecimalRectangleAction == nullptr)
        return;

    if (recursive) {
        getRangeAction(Axis::X).connectToPublicAction(&publicDecimalRectangleAction->getRangeAction(Axis::X), recursive);
        getRangeAction(Axis::Y).connectToPublicAction(&publicDecimalRectangleAction->getRangeAction(Axis::Y), recursive);
    }

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void DecimalRectangleAction::disconnectFromPublicAction(bool recursive)
{
    if (recursive) {
        getRangeAction(Axis::X).disconnectFromPublicAction(recursive);
        getRangeAction(Axis::Y).disconnectFromPublicAction(recursive);
    }

    WidgetAction::disconnectFromPublicAction(recursive);
}

void DecimalRectangleAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    getRangeAction(Axis::X).fromParentVariantMap(variantMap);
    getRangeAction(Axis::Y).fromParentVariantMap(variantMap);
}

QVariantMap DecimalRectangleAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    getRangeAction(Axis::X).insertIntoVariantMap(variantMap);
    getRangeAction(Axis::Y).insertIntoVariantMap(variantMap);

    return variantMap;
}

}
