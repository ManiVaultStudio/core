#include "ResponsiveToolBar.h"
#include "WidgetState.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QEvent>

namespace hdps {

namespace gui {

const std::int32_t ResponsiveToolBar::LAYOUT_MARGIN = 0;
const std::int32_t ResponsiveToolBar::LAYOUT_SPACING = 5;
const QSize ResponsiveToolBar::ICON_SIZE = QSize(12, 12);

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

    const auto getPriority = [this](QWidget* widget) -> std::int32_t {
        const auto priorityProperty = widget->property("priority");

        if (priorityProperty.isValid())
            return priorityProperty.toInt();

        return 0;
    };

    std::sort(_widgets.begin(), _widgets.end(), [getPriority](QWidget* left, QWidget* right) {
        return getPriority(left) > getPriority(right);
    });
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

    const auto getWidgetWidth = [this](QWidget* widget, const WidgetState::State& state) -> std::int32_t {
        const auto sizesProperty = widget->property("sizes");

        if (sizesProperty.isValid())
            return sizesProperty.value<QList<QSize>>().at(static_cast<std::int32_t>(state)).width();

        return widget->sizeHint().width();
    };

    auto visibleWidgets = getVisibleWidgets();

    for (auto widget : visibleWidgets) {
        runningWidth += getWidgetWidth(widget, WidgetState::State::Popup);

        if (runningWidth > sourceWidgetWidth)
            break;
        
        widgetsState[widget] = WidgetState::State::Popup;
    }
    
    for (auto widget : visibleWidgets) {
        runningWidth += getWidgetWidth(widget, WidgetState::State::Compact) - getWidgetWidth(widget, WidgetState::State::Popup);

        if (runningWidth > sourceWidgetWidth)
            break;

        widgetsState[widget] = WidgetState::State::Compact;
    }
    
    for (auto widget : visibleWidgets) {
        runningWidth += getWidgetWidth(widget, WidgetState::State::Full) - getWidgetWidth(widget, WidgetState::State::Compact);

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