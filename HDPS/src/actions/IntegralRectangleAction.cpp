#include "IntegralRectangleAction.h"

#include <QHBoxLayout>

namespace hdps {

namespace gui {

IntegralRectangleAction::IntegralRectangleAction(QObject * parent, const QString& title, const QRect& rectangle /*= QRect()*/, const QRect& defaultRectangle /*= QRect()*/) :
    RectangleAction<QRect>(parent, title, rectangle, defaultRectangle)
{
    _rectangleChanged           = [this]() -> void { emit rectangleChanged(_rectangle); };
    _defaultRectangleChanged    = [this]() -> void { emit defaultRectangleChanged(_defaultRectangle); };

    initialize(rectangle, defaultRectangle);
}

QString IntegralRectangleAction::getTypeString() const
{
    return "IntegralRectangle";
}

void IntegralRectangleAction::initialize(const QRect& rectangle /*= QRect()*/, const QRect& defaultRectangle /*= QRect()*/)
{
    _rectangle          = rectangle;
    _defaultRectangle   = defaultRectangle;

    emit rectangleChanged(_rectangle);
    emit defaultRectangleChanged(_defaultRectangle);
}

bool IntegralRectangleAction::mayPublish() const
{
    return true;
}

void IntegralRectangleAction::connectToPublicAction(WidgetAction* publicAction)
{
    auto publicIntegralRectangleAction = dynamic_cast<IntegralRectangleAction*>(publicAction);

    Q_ASSERT(publicIntegralRectangleAction != nullptr);

    connect(this, &IntegralRectangleAction::rectangleChanged, publicIntegralRectangleAction, &IntegralRectangleAction::setRectangle);
    connect(publicIntegralRectangleAction, &IntegralRectangleAction::rectangleChanged, this, &IntegralRectangleAction::setRectangle);

    setRectangle(publicIntegralRectangleAction->getRectangle());

    WidgetAction::connectToPublicAction(publicAction);
}

void IntegralRectangleAction::disconnectFromPublicAction()
{
    auto publicIntegralRectangleAction = dynamic_cast<IntegralRectangleAction*>(_publicAction);

    Q_ASSERT(publicIntegralRectangleAction != nullptr);

    disconnect(this, &IntegralRectangleAction::rectangleChanged, publicIntegralRectangleAction, &IntegralRectangleAction::setRectangle);
    disconnect(publicIntegralRectangleAction, &IntegralRectangleAction::rectangleChanged, this, &IntegralRectangleAction::setRectangle);

    WidgetAction::disconnectFromPublicAction();
}

WidgetAction* IntegralRectangleAction::getPublicCopy() const
{
    return new IntegralRectangleAction(parent(), text(), _rectangle, _defaultRectangle);
}

void IntegralRectangleAction::fromVariantMap(const QVariantMap& variantMap)
{
    if (!variantMap.contains("Value"))
        return;

    setRectangle(variantMap["Value"].toRect());
}

QVariantMap IntegralRectangleAction::toVariantMap() const
{
    return { { "Value", _rectangle } };
}

IntegralRectangleAction::LineEditWidget::LineEditWidget(QWidget* parent, IntegralRectangleAction* integralRectangleAction) :
    QLineEdit(parent)
{
    setAcceptDrops(true);
    setObjectName("LineEdit");
    setEnabled(false);

    const auto updateString = [this, integralRectangleAction]() {
        QSignalBlocker blocker(this);

        const auto topLeft      = integralRectangleAction->getRectangle().topLeft();
        const auto bottomRight  = integralRectangleAction->getRectangle().bottomRight();

        setText(QString("[%1, %2, %3, %4]").arg(QString::number(topLeft.x()), QString::number(topLeft.y()), QString::number(bottomRight.x()), QString::number(bottomRight.y())));
    };

    connect(integralRectangleAction, &IntegralRectangleAction::rectangleChanged, this, updateString);

    updateString();
}

QWidget* IntegralRectangleAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
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
}
