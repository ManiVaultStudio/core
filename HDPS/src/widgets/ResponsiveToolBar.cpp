#include "ResponsiveToolBar.h"

#include <QDebug>
#include <QPushButton>

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
    _layout(new QHBoxLayout()),
    _widgetEventProxy(this),
    _widgets()
{
    setAutoFillBackground(true);

    _layout->setMargin(5);
    _layout->setSpacing(10);
    _layout->addStretch(1);

    setLayout(_layout);
}

void ResponsiveToolBar::setListenWidget(QWidget* listenWidget)
{
    _widgetEventProxy.initialize(listenWidget, [this](const QSize& sourceWidgetSize) {
        const auto sourceWidgetWidth    = sourceWidgetSize.width();
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

            if (runningWidth < sourceWidgetWidth)
                widget->setState(ResponsiveToolBar::Widget::State::Compact, false);
            else
                break;
        }

        for (auto widget : _widgets) {
            if (widget->isHidden())
                continue;

            runningWidth += widget->getWidth(ResponsiveToolBar::Widget::State::Full) - widget->getWidth(ResponsiveToolBar::Widget::State::Compact);

            if (runningWidth < sourceWidgetWidth)
                widget->setState(ResponsiveToolBar::Widget::State::Full, false);
            else
                break;
        }

        for (auto widget : _widgets)
            widget->updateState();

        update();
        //qDebug() << minimumWidth << currentWidth << sourceWidgetWidth;
    });
}

void ResponsiveToolBar::addWidget(Widget* widget)
{
    Q_ASSERT(widget != nullptr);

    _widgets << widget;

    _layout->insertWidget(_layout->count() - 1, widget);
}

}
}