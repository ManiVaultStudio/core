#include "ResponsiveToolBar.h"
#include "WidgetState.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QFrame>
#include <QEvent>

#include <algorithm>

namespace hdps {

namespace gui {

ResponsiveToolBar::QSpacer::QSpacer(QWidget* left, QWidget* right) :
    QWidget(),
    _left(left),
    _right(right),
    _layout(new QHBoxLayout()),
    _verticalLine(new QFrame())
{
    _left->installEventFilter(this);
    _right->installEventFilter(this);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    //setFixedWidth(50);
    setLayout(_layout);

    _verticalLine->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    _verticalLine->setFrameShape(QFrame::VLine);
    _verticalLine->setFrameShadow(QFrame::Sunken);

    _layout->setMargin(0);
    _layout->setSpacing(15);
    _layout->addWidget(_verticalLine);
}

bool ResponsiveToolBar::QSpacer::eventFilter(QObject* target, QEvent* event)
{
    const auto widget = dynamic_cast<QWidget*>(target);

    if (widget) {
        switch (event->type()) {
            case QEvent::Hide:
            case QEvent::Show:
                //setVisible(_left->isVisible() && _right->isVisible());
                break;

            case QEvent::DynamicPropertyChange:
                //updateLayout(dynamic_cast<QWidget*>(target));
                break;

            default:
                break;
        }
    }

    return QObject::eventFilter(target, event);
}

void ResponsiveToolBar::QSpacer::updateState()
{
    /*
    const auto getState = [this](QWidget* widget) -> std::int32_t {
        const auto stateProperty = widget->property("state");

        if (stateProperty.isValid())
            return stateProperty.toInt();

        return 0;
    };

    setVisible(_left->isVisible() && _right->isVisible());
    */
}

const std::int32_t ResponsiveToolBar::LAYOUT_MARGIN = 0;
const std::int32_t ResponsiveToolBar::LAYOUT_SPACING = 5;

ResponsiveToolBar::ResponsiveToolBar(QWidget* parent) :
    QWidget(parent),
    _listenWidget(nullptr),
    _layout(new QHBoxLayout()),
    _sectionWidgets()
{
    setAutoFillBackground(true);

    _layout->setMargin(5);

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
    const auto widget = dynamic_cast<QWidget*>(target);

    if (widget == nullptr)
        return QObject::eventFilter(target, event);

    if (widget == _listenWidget) {
        switch (event->type()) {
            case QEvent::Resize:
                updateLayout(dynamic_cast<QWidget*>(target));
                break;

            default:
                break;
        }
    } else {
        switch (event->type()) {
            case QEvent::Hide:
            case QEvent::Show:
                //updateLayout(dynamic_cast<QWidget*>(target));
                break;

            case QEvent::Resize:
                //updateLayout(dynamic_cast<QWidget*>(target));
                break;

            case QEvent::DynamicPropertyChange:
                //updateLayout(dynamic_cast<QWidget*>(target));
                break;

            default:
                break;
        }
    }

    return QObject::eventFilter(target, event);
}

void ResponsiveToolBar::addStatefulWidget(StatefulWidget* statefulWidget)
{
    Q_ASSERT(statefulWidget != nullptr);

    /*
    if (!_widgets.isEmpty()) {
        auto spacer = new QSpacer(_widgets.last(), widget);
        _layout->addWidget(spacer);
        _widgets << spacer;
    }
    */

    auto sectionWidget = new SectionWidget(statefulWidget);

    _sectionWidgets << sectionWidget;

    statefulWidget->installEventFilter(this);

    _layout->addWidget(sectionWidget);

    /*
    const auto getPriority = [this](QWidget* widget) -> std::int32_t {
        const auto priorityProperty = widget->property("priority");

        if (priorityProperty.isValid())
            return priorityProperty.toInt();

        return 0;
    };

    std::sort(_sectionWidgets.begin(), _sectionWidgets.end(), [getPriority](QWidget* left, QWidget* right) {
        return getPriority(left) > getPriority(right);
    });
    */
}

void ResponsiveToolBar::addStretch(const std::int32_t& stretch /*= 0*/)
{
    _layout->addStretch(stretch);
}

/*
const auto printWidgets = [this](QList<QWidget*> widgets) {
    for (auto widget : widgets)
        qDebug() << widget->windowTitle();
};
*/

void ResponsiveToolBar::updateLayout(QWidget* sourceWidget /*= nullptr*/)
{
    Q_ASSERT(_listenWidget != nullptr);
    /*
    QMap<QWidget*, std::int32_t> widgetStates;

    QList<QWidget*> visibleWidgets = getVisibleWidgets();

    for (auto visibleWidget : visibleWidgets) {
        const auto stateProperty = visibleWidget->property("state");
        widgetStates[visibleWidget] = static_cast<std::int32_t>(WidgetState::State::Popup);
        //widgetsState[widget] = static_cast<WidgetState::State>(stateProperty.toInt());
    }

    const auto changeWidgetState = [this, &widgetStates](QWidget* widget, const bool& expand) {
        const auto newState = widgetStates[widget] + (expand ? 1 : -1);

        widgetStates[widget] = std::max(0, std::min(newState, 2));
    };

    const auto getWidgetWidth = [this](QWidget* widget, const std::int32_t& state) -> std::int32_t {
        const auto sizesProperty = widget->property("sizes");

        if (sizesProperty.isValid())
            return sizesProperty.value<QList<QSize>>().at(state).width();

        return widget->width();
    };

    const auto getSpaceRemaining = [this, visibleWidgets, &widgetStates, getWidgetWidth]() -> std::int32_t {
        auto runningWidth = 2 * layout()->margin();

        for (auto visibleWidget : visibleWidgets)
            runningWidth += getWidgetWidth(visibleWidget, widgetStates[visibleWidget]);

        return _listenWidget->width() - runningWidth;
    };

    auto expand = getSpaceRemaining() > 0;

    for (auto item : QList<std::int32_t>({ 1, 2 }))
    {
        for (auto widget : visibleWidgets) {
            const auto cacheWidgetStates = widgetStates;

            changeWidgetState(widget, expand);

            if (expand && getSpaceRemaining() < 0) {

            }

        }
    }

    for (auto widget : visibleWidgets)
        widget->setProperty("state", widgetStates[widget]);

    update();
    */

    QMap<QWidget*, WidgetState> widgetStates;

    for (auto sectionWidget : _sectionWidgets)
        widgetStates[sectionWidget] = WidgetState::Popup;
    
    auto runningWidth = 0;

    for (auto sectionWidget : _sectionWidgets)
        runningWidth += sectionWidget->getWidth(WidgetState::Popup);

    const auto listenWidgetWidth    = _listenWidget->width();
    const auto preferredState       = listenWidgetWidth < 500 ? WidgetState::Compact : WidgetState::Full;

    for (auto sectionWidget : _sectionWidgets) {
        runningWidth += sectionWidget->getWidth(preferredState);

        if (runningWidth > _listenWidget->width())
            break;

        widgetStates[sectionWidget] = preferredState;
    }

    for (auto sectionWidget : _sectionWidgets)
        sectionWidget->setState(widgetStates[sectionWidget]);
}

QList<QWidget*> ResponsiveToolBar::getVisibleWidgets()
{
    QList<QWidget*> visibleWidgets;

    for (auto widget : _sectionWidgets) {
        if (widget->isHidden())
            continue;

        visibleWidgets << widget;
    }

    return visibleWidgets;
}

}
}