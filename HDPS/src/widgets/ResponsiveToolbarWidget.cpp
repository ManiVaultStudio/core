#include "ResponsiveToolbarWidget.h"
#include "CollapsedWidget.h"

#include "util/Exception.h"

#include <QCoreApplication>
#include <QDebug>
#include <QEvent>
#include <QMap>
#include <QPushButton>
#include <QResizeEvent>
#include <QSharedPointer>

using namespace hdps::util;

namespace hdps
{

namespace gui
{

ResponsiveToolbarWidget::ResponsiveToolbarWidget(QWidget* parent) :
    QWidget(parent),
    _layout(),
    _toolBarWidget(),
    _toolBarLayout(),
    _widgets(),
    _spacerWidgets()
{
    //setAutoFillBackground(true);

    _toolBarLayout.setMargin(4);
    _toolBarLayout.setSpacing(0);
    _toolBarLayout.setSizeConstraint(QLayout::SetFixedSize);

    _toolBarLayout.addStretch(1);

    _toolBarWidget.setLayout(&_toolBarLayout);

    _layout.addWidget(&_toolBarWidget);
    _layout.addStretch(1);

    setLayout(&_layout);

    _layout.setMargin(4);

    parent->installEventFilter(this);
    this->installEventFilter(this);
    //_toolBarWidget.installEventFilter(this);
}

bool ResponsiveToolbarWidget::eventFilter(QObject* object, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            auto resizeEvent = static_cast<QResizeEvent*>(event);
            qDebug() << resizeEvent;

            auto widget = dynamic_cast<QWidget*>(object);

            if (widget == parentWidget()) {
                setFixedWidth(parentWidget()->width());
                updateLayout();
            }
                
                

            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(object, event);
}

void ResponsiveToolbarWidget::addWidget(QWidget* widget, const std::int32_t& priority /*= 0*/)
{
    auto sharedWidget = QSharedPointer<QWidget>(widget);

    _widgets << new ToolbarItemWidget(this, [this, sharedWidget, widget](const WidgetActionWidget::State& state) -> QSharedPointer<QWidget> {
        switch (state)
        {
            case WidgetActionWidget::State::Standard:
                return sharedWidget;

            case WidgetActionWidget::State::Collapsed:
                return QSharedPointer<QWidget>(new CollapsedWidget(this, widget));

            default:
                break;
        }

        return nullptr;
    });

    if (_widgets.count() >= 2) {
        _spacerWidgets << new SpacerWidget();
        _toolBarLayout.addWidget(_spacerWidgets.back());
    }

    _toolBarLayout.addWidget(_widgets.back());
}

void ResponsiveToolbarWidget::updateLayout()
{
    qDebug() << __FUNCTION__;
    
    try {
        QMap<ToolbarItemWidget*, WidgetActionWidget::State> states;

        for (auto widget : _widgets)
            states[widget] = WidgetActionWidget::State::Collapsed;

        const auto getWidth = [this, &states]() -> std::uint32_t {
            std::uint32_t width = 2 * _layout.margin();

            for (auto widget : _widgets)
                width += widget->getSizeHint(states[widget]).width();

            for (auto spacerWidget : _spacerWidgets) {
                const auto spacerWidgetIndex = _spacerWidgets.indexOf(spacerWidget);
                const auto stateWidgetLeft = _widgets[spacerWidgetIndex];
                const auto stateWidgetRight = _widgets[spacerWidgetIndex + 1];
                const auto spacerWidgetType = SpacerWidget::getType(states[stateWidgetLeft], states[stateWidgetRight]);
                const auto spacerWidgetWidth = SpacerWidget::getWidth(spacerWidgetType);

                width += spacerWidgetWidth;
            }

            return width;
        };

        // Sorted toolbar item widgets based on priority
        QVector<ToolbarItemWidget*> prioritySortedToolbarItemWidgets = _widgets;

        // Do the sorting
        std::sort(prioritySortedToolbarItemWidgets.begin(), prioritySortedToolbarItemWidgets.end(), [](ToolbarItemWidget* toolbarItemWidgetA, ToolbarItemWidget* toolbarItemWidgetB) {

            // Stateful widgets with higher priority are collapsed later than ones with a low priority
            return toolbarItemWidgetA->getPriority() > toolbarItemWidgetB->getPriority();
        });

        for (auto stateWidget : prioritySortedToolbarItemWidgets) {
            auto cachedStates = states;

            states[stateWidget] = WidgetActionWidget::State::Standard;

            if (getWidth() > static_cast<std::uint32_t>(width())) {
                states = cachedStates;
                break;
            }
        }

        for (auto widget : _widgets)
            widget->setState(states[widget]);

        for (auto spacerWidget : _spacerWidgets) {
            const auto spacerWidgetIndex = _spacerWidgets.indexOf(spacerWidget);
            const auto stateWidgetLeft = _widgets[spacerWidgetIndex];
            const auto stateWidgetRight = _widgets[spacerWidgetIndex + 1];
            const auto spacerWidgetType = SpacerWidget::getType(states[stateWidgetLeft], states[stateWidgetRight]);

            spacerWidget->setType(spacerWidgetType);
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to publish selection change", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to publish selection change");
    }
}

ResponsiveToolbarWidget::SpacerWidget::SpacerWidget(const Type& type /*= State::Divider*/) :
    QWidget(),
    _type(Type::Divider),
    _layout(),
    _verticalLine()
{
    _verticalLine.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    _verticalLine.setFrameShape(QFrame::VLine);
    _verticalLine.setFrameShadow(QFrame::Sunken);

    _layout.setMargin(2);
    _layout.setSpacing(0);
    _layout.setAlignment(Qt::AlignCenter);
    _layout.addWidget(&_verticalLine);
    
    setType(type);
}

ResponsiveToolbarWidget::SpacerWidget::Type ResponsiveToolbarWidget::SpacerWidget::getType(const WidgetActionWidget::State& widgetTypeLeft, const WidgetActionWidget::State& widgetTypeRight)
{
    return widgetTypeLeft == WidgetActionWidget::State::Collapsed && widgetTypeRight == WidgetActionWidget::State::Collapsed ? Type::Spacer : Type::Divider;
}

ResponsiveToolbarWidget::SpacerWidget::Type ResponsiveToolbarWidget::SpacerWidget::getType(const ToolbarItemWidget* toolbarItemWidgetLeft, const ToolbarItemWidget* toolbarItemWidgetRight)
{
    return getType(toolbarItemWidgetLeft->getState(), toolbarItemWidgetRight->getState());
}

void ResponsiveToolbarWidget::SpacerWidget::setType(const Type& type)
{
    _type = type;

    setLayout(&_layout);
    setFixedWidth(getWidth(_type));

    _verticalLine.setVisible(_type == Type::Divider ? true : false);
}

std::int32_t ResponsiveToolbarWidget::SpacerWidget::getWidth(const Type& type)
{
    switch (type)
    {
        case Type::Divider:
            return 14;

        case Type::Spacer:
            return 6;

        default:
            break;
    }

    return 0;
}

}
}
