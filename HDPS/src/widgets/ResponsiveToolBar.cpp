#include "ResponsiveToolBar.h"

#include <QDebug>
#include <QPushButton>
#include <QEvent>

namespace hdps {

namespace gui {

const std::int32_t ResponsiveToolBar::LAYOUT_MARGIN = 0;
const std::int32_t ResponsiveToolBar::LAYOUT_SPACING = 6;
const QSize ResponsiveToolBar::ICON_SIZE = QSize(12, 12);

const QMap<QString, WidgetState::State> WidgetState::states = {
    { "Undefined", WidgetState::State::Undefined },
    { "Popup", WidgetState::State::Popup },
    { "Compact", WidgetState::State::Compact },
    { "Full", WidgetState::State::Full }
};

WidgetState::WidgetState(QWidget* widget) :
    QObject(widget),
    _widget(widget),
    _sizes({ QSize(), QSize(), QSize() })
{
    _widget->installEventFilter(this);
}

bool WidgetState::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::DynamicPropertyChange)
    {
        auto propertyChangeEvent = static_cast<QDynamicPropertyChangeEvent*>(event);

        if (propertyChangeEvent) {
            const auto propertyName = QString(propertyChangeEvent->propertyName().data());

            if (propertyName == "state")
                changeState(getState());
        }
    }

    return QObject::eventFilter(object, event);
}

void WidgetState::setSize(const State& state, const QSize& size)
{
    _sizes[static_cast<std::int32_t>(state)] = size;
}

void WidgetState::initialize()
{
    Q_ASSERT(_widget != nullptr);

    changeState(State::Popup);
    changeState(State::Compact);
    changeState(State::Full);

    qDebug() << _sizes;
}

void WidgetState::updateStateSize(const State& state)
{
    Q_ASSERT(_widget != nullptr);

    _widget->adjustSize();

    _sizes[static_cast<std::int32_t>(state)] = _widget->sizeHint();

    qDebug() << "Current state:" << WidgetState::getStateName(state) << _sizes[static_cast<std::int32_t>(state)];

    _widget->setProperty("sizes", QVariant::fromValue(_sizes));
}

void WidgetState::changeState(const State& state)
{
    emit updateState(state);
    updateStateSize(state);
}

ResponsiveToolBar::ResponsiveToolBar(QWidget* parent) :
    QWidget(parent),
    _listenWidget(nullptr),
    _layout(new QHBoxLayout()),
    _widgets()
{
    setAutoFillBackground(true);

    _layout->setMargin(5);
    _layout->setSpacing(4);
    _layout->addStretch(1);

    setLayout(_layout);
}

void ResponsiveToolBar::setListenWidget(QWidget* listenWidget)
{
    Q_ASSERT(listenWidget != nullptr);

    _listenWidget = listenWidget;

    _listenWidget->installEventFilter(this);
}

bool ResponsiveToolBar::eventFilter(QObject* target, QEvent* event)
{
    if (event->type() != QEvent::Resize)
        return QObject::eventFilter(target, event);

    if (target == _listenWidget) {
        updateLayout(dynamic_cast<QWidget*>(target));
    } else {
        switch (event->type()) {
            case QEvent::Hide:
            case QEvent::Show:
            case QEvent::Resize:
                updateLayout(dynamic_cast<QWidget*>(target));
                break;

            default:
                break;
        }
    }

    return QObject::eventFilter(target, event);
}

void ResponsiveToolBar::addWidget(QWidget* widget)
{
    Q_ASSERT(widget != nullptr);

    _widgets << widget;

    widget->installEventFilter(this);

    _layout->insertWidget(_layout->count() - 1, widget);

    /*
    std::sort(_widgets.begin(), _widgets.end(), [](QWidget* left, QWidget* right) {
        return left->getPriority() > right->getPriority();
    });
    */
}

void ResponsiveToolBar::addStretch(const std::int32_t& stretch /*= 0*/)
{
    _layout->addStretch(stretch);
}

void ResponsiveToolBar::updateLayout(QWidget* widget /*= nullptr*/)
{
    Q_ASSERT(_listenWidget != nullptr);

    const auto sourceWidgetWidth    = _listenWidget->width();
    const auto marginsWidth         = 2 * layout()->margin();
    const auto noSpacings           = std::max(_widgets.count() - 1, 0);
    const auto spacingsWidth        = noSpacings * _layout->spacing();

    auto runningWidth = spacingsWidth + marginsWidth;

    QMap<QWidget*, WidgetState::State> widgetsState;

    const auto widgetWidth = [this](QWidget* widget, const WidgetState::State& state) -> std::int32_t {
        const auto sizesProperty = widget->property("sizes");

        if (sizesProperty.isValid())
            return sizesProperty.value<QList<QSize>>().at(static_cast<std::int32_t>(state)).width();

        return widget->sizeHint().width();
    };

    auto visibleWidgets = getVisibleWidgets();

    for (auto widget : visibleWidgets) {
        runningWidth += widgetWidth(widget, WidgetState::State::Popup);

        if (runningWidth > sourceWidgetWidth)
            break;
        
        widgetsState[widget] = WidgetState::State::Popup;
    }
    
    for (auto widget : visibleWidgets) {
        runningWidth += widgetWidth(widget, WidgetState::State::Compact) - widgetWidth(widget, WidgetState::State::Popup);

        if (runningWidth > sourceWidgetWidth)
            break;

        widgetsState[widget] = WidgetState::State::Compact;
    }
    
    for (auto widget : visibleWidgets) {
        runningWidth += widgetWidth(widget, WidgetState::State::Full) - widgetWidth(widget, WidgetState::State::Compact);

        if (runningWidth > sourceWidgetWidth)
            break;

        widgetsState[widget] = WidgetState::State::Full;
    }
    
    for (auto widget : visibleWidgets)
        widget->setProperty("state", static_cast<std::int32_t>(widgetsState[widget]));

    update();
}

QList<QWidget*> ResponsiveToolBar::getVisibleWidgets()
{
    QList<QWidget*> visibleWidgets;

    for (auto widget : _widgets) {
        if (widget->isHidden())
            continue;

        visibleWidgets << widget;
    }

    return visibleWidgets;
}

}
}