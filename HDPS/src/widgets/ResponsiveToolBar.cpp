#include "ResponsiveToolBar.h"

#include <QDebug>
#include <QPushButton>
#include <QEvent>

namespace hdps {

namespace gui {

const std::int32_t ResponsiveToolBar::Widget::LAYOUT_MARGIN     = 0;
const std::int32_t ResponsiveToolBar::Widget::LAYOUT_SPACING    = 6;

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

    _sizes[static_cast<std::int32_t>(state)] = _widget->sizeHint();

    qDebug() << "Current state:" << WidgetState::getStateName(state) << _sizes[static_cast<std::int32_t>(state)];

    _widget->setProperty("sizes", QVariant::fromValue(_sizes));
}

void WidgetState::changeState(const State& state)
{
    emit updateState(state);
    updateStateSize(state);
}

ResponsiveToolBar::Widget::Widget(const QString& title, const std::int32_t& priority /*= 0*/) :
    QStackedWidget(),
    _priority(priority),
    _state(WidgetState::State::Undefined),
    _title(title),
    _sizes(),
    _popupPushButton(new QPushButton()),
    _widget(new QWidget())
{
    _sizes[WidgetState::State::Popup] = QSize();
    _sizes[WidgetState::State::Compact] = QSize();
    _sizes[WidgetState::State::Full] = QSize();

    _popupPushButton->setIconSize(ICON_SIZE);
    _popupPushButton->setToolTip(QString("%1 settings").arg(_title));

    _widget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    addWidget(_popupPushButton);
    addWidget(_widget);
}

void ResponsiveToolBar::Widget::setState(const WidgetState::State& state, const bool& update /*= true*/)
{
    if (state == _state)
        return;

    _state = state;

    if (update)
        updateState();

    /*
    switch (_state)
    {
        case State::Undefined:
            break;

        case State::Popup:
            _sizes[_state] = _popupPushButton->sizeHint();
            break;

        case State::Compact:
        case State::Full:
            _sizes[_state] = _widget->sizeHint();
            break;

        default:
            break;
    }
    */
}

std::int32_t ResponsiveToolBar::Widget::getPriority() const
{
    return _priority;
}

void ResponsiveToolBar::Widget::setPriority(const std::int32_t& priority)
{
    _priority = priority;
}

QString ResponsiveToolBar::Widget::getTitle() const
{
    return _title;
}

void ResponsiveToolBar::Widget::setWidgetLayout(QLayout* layout)
{
    Q_ASSERT(layout != nullptr);

    layout->setMargin(ResponsiveToolBar::Widget::LAYOUT_MARGIN);
    layout->setSpacing(ResponsiveToolBar::Widget::LAYOUT_SPACING);

    if (_widget->layout())
        delete _widget->layout();

    _widget->setLayout(layout);
}

void ResponsiveToolBar::Widget::computeStateSizes()
{
    setAttribute(Qt::WA_DontShowOnScreen, true);
    show();

    setState(WidgetState::State::Popup);
    _sizes[_state] = _popupPushButton->sizeHint();

    setState(WidgetState::State::Compact);
    _sizes[_state] = _widget->sizeHint();

    setState(WidgetState::State::Full);
    _sizes[_state] = _widget->sizeHint();

    setAttribute(Qt::WA_DontShowOnScreen, false);
}

ResponsiveToolBar::ResponsiveToolBar(QWidget* parent) :
    QWidget(parent),
    _listenWidget(nullptr),
    _layout(new QHBoxLayout()),
    _widgets()
{
    setAutoFillBackground(true);

    _layout->setMargin(5);
    _layout->setSpacing(6);
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

    updateLayout(dynamic_cast<Widget*>(target));

    return QObject::eventFilter(target, event);
}

void ResponsiveToolBar::addWidget(QWidget* widget)
{
    Q_ASSERT(widget != nullptr);

    _widgets << widget;

    //widget->installEventFilter(this);

    _layout->insertWidget(_layout->count() - 1, widget);

    /*
    std::sort(_widgets.begin(), _widgets.end(), [](QWidget* left, QWidget* right) {
        return left->getPriority() > right->getPriority();
    });
    */
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

    for (auto widget : _widgets) {
        if (widget->isHidden())
            continue;

        runningWidth += widgetWidth(widget, WidgetState::State::Popup);

        if (runningWidth > sourceWidgetWidth)
            break;
        
        widgetsState[widget] = WidgetState::State::Popup;
    }

    
    for (auto widget : _widgets) {
        if (widget->isHidden())
            continue;
        
        runningWidth += widgetWidth(widget, WidgetState::State::Compact) - widgetWidth(widget, WidgetState::State::Popup);

        if (runningWidth > sourceWidgetWidth)
            break;

        widgetsState[widget] = WidgetState::State::Compact;
    }
    
    for (auto widget : _widgets) {
        if (widget->isHidden())
            continue;

        runningWidth += widgetWidth(widget, WidgetState::State::Full) - widgetWidth(widget, WidgetState::State::Compact);

        if (runningWidth > sourceWidgetWidth)
            break;

        widgetsState[widget] = WidgetState::State::Full;
    }
    
    for (auto widget : _widgets)
        widget->setProperty("state", static_cast<std::int32_t>(widgetsState[widget]));

    qDebug() << static_cast<std::int32_t>(widgetsState.first());

    update();
}

}
}