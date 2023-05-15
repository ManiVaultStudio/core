#include "DecimalRectangleAction.h"

#include <QHBoxLayout>

namespace hdps::gui {

DecimalRectangleAction::DecimalRectangleAction(QObject * parent, const QString& title, const QRectF& rectangle /*= QRectF()*/, const QRectF& defaultRectangle /*= QRectF()*/) :
    RectangleAction<QRectF>(parent, title, rectangle, defaultRectangle)
{
    _rectangleChanged           = [this]() -> void { emit rectangleChanged(_rectangle); };
    _defaultRectangleChanged    = [this]() -> void { emit defaultRectangleChanged(_defaultRectangle); };

    initialize(rectangle, defaultRectangle);
}

void DecimalRectangleAction::initialize(const QRectF& rectangle /*= QRectF()*/, const QRectF& defaultRectangle /*= QRectF()*/)
{
    _rectangle          = rectangle;
    _defaultRectangle   = defaultRectangle;

    emit rectangleChanged(_rectangle);
    emit defaultRectangleChanged(_defaultRectangle);
}

void DecimalRectangleAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicDecimalRectangleAction = dynamic_cast<DecimalRectangleAction*>(publicAction);

    Q_ASSERT(publicDecimalRectangleAction != nullptr);

    if (publicDecimalRectangleAction == nullptr)
        return;

    connect(this, &DecimalRectangleAction::rectangleChanged, publicDecimalRectangleAction, &DecimalRectangleAction::setRectangle);
    connect(publicDecimalRectangleAction, &DecimalRectangleAction::rectangleChanged, this, &DecimalRectangleAction::setRectangle);

    setRectangle(publicDecimalRectangleAction->getRectangle());

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void DecimalRectangleAction::disconnectFromPublicAction(bool recursive)
{
    auto publicDecimalRectangleAction = dynamic_cast<DecimalRectangleAction*>(getPublicAction());

    Q_ASSERT(publicDecimalRectangleAction != nullptr);

    if (publicDecimalRectangleAction == nullptr)
        return;

    disconnect(this, &DecimalRectangleAction::rectangleChanged, publicDecimalRectangleAction, &DecimalRectangleAction::setRectangle);
    disconnect(publicDecimalRectangleAction, &DecimalRectangleAction::rectangleChanged, this, &DecimalRectangleAction::setRectangle);

    WidgetAction::disconnectFromPublicAction(recursive);
}

void DecimalRectangleAction::fromVariantMap(const QVariantMap& variantMap)
{
    if (!variantMap.contains("Value"))
        return;

    setRectangle(variantMap["Value"].toRect());
}

QVariantMap DecimalRectangleAction::toVariantMap() const
{
    return { { "Value", _rectangle } };
}

DecimalRectangleAction::LineEditWidget::LineEditWidget(QWidget* parent, DecimalRectangleAction* integralRectangleAction) :
    QLineEdit(parent)
{
    setAcceptDrops(true);
    setObjectName("LineEdit");
    setEnabled(false);

    const auto updateString = [this, integralRectangleAction]() {
        QSignalBlocker blocker(this);

        const auto topLeft      = integralRectangleAction->getRectangle().topLeft();
        const auto bottomRight  = integralRectangleAction->getRectangle().bottomRight();

        setText(QString("[%1, %2, %3, %4]").arg(QString::number(topLeft.x(), 'f', 2), QString::number(topLeft.y(), 'f', 2), QString::number(bottomRight.x(), 'f', 2), QString::number(bottomRight.y(), 'f', 2)));
    };

    connect(integralRectangleAction, &DecimalRectangleAction::rectangleChanged, this, updateString);

    updateString();
}

QWidget* DecimalRectangleAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    if (widgetFlags & WidgetFlag::LineEdit)
        layout->addWidget(new LineEditWidget(parent, this), 1);

    widget->setLayout(layout);

    return widget;
}

}
