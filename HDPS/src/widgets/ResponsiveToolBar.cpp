#include "ResponsiveToolBar.h"

#include <QDebug>
#include <QPushButton>
#include <QEvent>

namespace hdps {

namespace gui {

ResponsiveToolBar::Widget::Widget(const QString& title, const std::int32_t& priority /*= 0*/) :
    QStackedWidget(),
    _priority(priority),
    _state(State::Undefined),
    _title(title),
    _sizes(),
    _popupPushButton(new QPushButton()),
    _widget(new QWidget())
{
    _sizes[State::Popup] = QSize();
    _sizes[State::Compact] = QSize();
    _sizes[State::Full] = QSize();

    _popupPushButton->setText("P");

    //_widget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    addWidget(_popupPushButton);
    addWidget(_widget);
}

void ResponsiveToolBar::Widget::setState(const State& state, const bool& update /*= true*/)
{
    if (state == _state)
        return;

    _state = state;

    if (update)
        updateState();
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

    setState(State::Popup);
    _sizes[State::Popup] = _popupPushButton->sizeHint();

    setState(State::Compact);
    _sizes[State::Compact] = _widget->sizeHint();

    setState(State::Full);
    _sizes[State::Full] = _widget->sizeHint();

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

    updateLayout();

    return QObject::eventFilter(target, event);
}

void ResponsiveToolBar::addWidget(Widget* widget)
{
    Q_ASSERT(widget != nullptr);

    _widgets << widget;

    _layout->insertWidget(_layout->count() - 1, widget);

    std::sort(_widgets.begin(), _widgets.end(), [](Widget* left, Widget* right) {
        return left->getPriority() > right->getPriority();
    });
}

void ResponsiveToolBar::updateLayout()
{
    Q_ASSERT(_listenWidget != nullptr);

    const auto sourceWidgetWidth    = _listenWidget->width();
    const auto marginsWidth         = 2 * layout()->margin();
    const auto noSpacings           = std::max(_widgets.count() - 1, 0);
    const auto spacingsWidth        = noSpacings * _layout->spacing();

    auto runningWidth = spacingsWidth + marginsWidth;

    for (auto widget : _widgets) {
        if (widget->isHidden())
            continue;

        runningWidth += widget->getWidth(ResponsiveToolBar::Widget::State::Popup);

        widget->setState(Widget::State::Popup, false);
    }

    for (auto widget : _widgets) {
        if (widget->isHidden())
            continue;

        runningWidth += widget->getWidth(ResponsiveToolBar::Widget::State::Compact) - widget->getWidth(ResponsiveToolBar::Widget::State::Popup);

        if (runningWidth > sourceWidgetWidth)
            break;

        widget->setState(ResponsiveToolBar::Widget::State::Compact, false);
    }

    for (auto widget : _widgets) {
        if (widget->isHidden())
            continue;

        runningWidth += widget->getWidth(ResponsiveToolBar::Widget::State::Full) - widget->getWidth(ResponsiveToolBar::Widget::State::Compact);

        if (runningWidth > sourceWidgetWidth)
            break;

        widget->setState(ResponsiveToolBar::Widget::State::Full, false);
    }

    for (auto widget : _widgets)
        widget->updateState();

    update();
}

}
}