#include "HorizontalToolbarAction.h"

#include <QEvent>
#include <QResizeEvent>

namespace hdps::gui {

HorizontalToolbarAction::HorizontalToolbarAction(QObject* parent, const QString& title /*= "Horizontal Toolbar"*/) :
    ToolbarAction(parent, title)
{
    setText(title);
    
    getGroupAction().setDefaultWidgetFlags(GroupAction::Horizontal);
    //getGroupAction().setShowLabels(false);
}

HorizontalToolbarAction::Widget::Widget(QWidget* parent, HorizontalToolbarAction* horizontalToolbarAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, horizontalToolbarAction, widgetFlags),
    _horizontalToolbarAction(horizontalToolbarAction),
    _layout(),
    _toolbarLayout(),
    _toolbarWidget(),
    _timer()
{
    parent->installEventFilter(this);

    _timer.setInterval(250);
    _timer.setSingleShot(true);

    _toolbarLayout.setContentsMargins(0, 0, 0, 0);
    //_toolbarLayout.setSpacing(0);
    //_toolbarLayout.setSizeConstraint(QLayout::SetFixedSize);

    _layout.setSizeConstraint(QLayout::SetFixedSize);

    _toolbarWidget.setLayout(&_toolbarLayout);
    //_toolbarWidget.setStyleSheet("background-color:red;");

    _layout.setContentsMargins(0, 0, 0, 0);
    _layout.addWidget(&_toolbarWidget);
    _layout.addStretch(1);
    
    setLayout(&_layout);

    connect(_horizontalToolbarAction, &ToolbarAction::actionWidgetsChanged, this, &HorizontalToolbarAction::Widget::setActionWidgets);
    connect(&_timer, &QTimer::timeout, this, &Widget::updateLayout);

    setActionWidgets();
}

bool HorizontalToolbarAction::Widget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            const auto resizeEvent = static_cast<QResizeEvent*>(event);

            auto layoutDirty = resizeEvent->size().width() != resizeEvent->oldSize().width();

            if (!layoutDirty)
                break;

            if (_timer.isActive())
                _timer.stop();

            _timer.start();

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

void HorizontalToolbarAction::Widget::setActionWidgets()
{
    QLayoutItem* layoutItem;

    while ((layoutItem = _toolbarLayout.takeAt(0)) != nullptr) {
        delete layoutItem->widget();
        delete layoutItem;
    }

    for (auto actionWidget : _horizontalToolbarAction->getActionItems())
        _toolbarLayout.addWidget(actionWidget->createWidget(&_toolbarWidget));

    updateLayout();
}

void HorizontalToolbarAction::Widget::updateLayout()
{
    qDebug() << _toolbarWidget.width() << parentWidget()->width();
}

QString HorizontalToolbarAction::getTypeString() const
{
    return "HorizontalToolbar";
}

}
