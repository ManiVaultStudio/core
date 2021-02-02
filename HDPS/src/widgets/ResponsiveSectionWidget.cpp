#include "ResponsiveSectionWidget.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QEvent>
#include <QResizeEvent>

namespace hdps {

namespace gui {

ResponsiveSectionWidget::ResponsiveSectionWidget(GetWidgetForStateFn getWidgetStateFn, const QString& name, const QIcon& icon /*= QIcon()*/, const std::int32_t& priority /*= 0*/) :
    _getWidgetStateFn(getWidgetStateFn),
    _initializeWidgetFn(),
    _state(State::Undefined),
    _name(name),
    _priority(priority),
    _layout(new QHBoxLayout()),
    _popupWidget(),
    _stateWidget(),
    _popupPushButton(QSharedPointer<PopupPushButton>::create()),
    _stateSizeHints({ QSize(), QSize() , QSize() })
{
    _popupPushButton->setIcon(icon);
}

bool ResponsiveSectionWidget::eventFilter(QObject* object, QEvent* event)
{
    auto widget = dynamic_cast<QWidget*>(object);

    if (widget == _stateWidget) {
        switch (event->type())
        {
            case QEvent::EnabledChange:
                _popupPushButton->setEnabled(_stateWidget->isEnabled());
                break;

            case QEvent::Resize:
            {
                /*
                if (widget == _stateWidget.get()) {
                    auto* resizeEvent = static_cast<QResizeEvent*>(event);

                    qDebug() << QString("Resize %1 (%2)").arg(_name.toLower(), QString::number(static_cast<std::int32_t>(_state))) << resizeEvent->size();

                    _stateSizeHints[static_cast<std::int32_t>(_state)] = resizeEvent->size();
                }
                */
                
                break;
            }

            default:
                break;
        }
    }

    return QObject::eventFilter(object, event);
}

void ResponsiveSectionWidget::initialize(InitializeWidgetFn initializeWidgetFn)
{
    _initializeWidgetFn = initializeWidgetFn;

    _layout->setMargin(0);
    _layout->setSpacing(0);
    _layout->addWidget(_popupPushButton.get());

    setLayout(_layout);

    computeSizeHints();
}

QString ResponsiveSectionWidget::getName() const
{
    return _name;
}

hdps::gui::ResponsiveSectionWidget::State ResponsiveSectionWidget::getState() const
{
    return _state;
}

void ResponsiveSectionWidget::setState(const State& state)
{
    if (state == _state)
        return;

    _state = state;

    const auto isPopup = _state == State::Popup;

    _popupPushButton->setVisible(isPopup);

    if (!_stateWidget.isNull())
        _stateWidget->setVisible(!isPopup);

    switch (_state)
    {
        case State::Popup:
        {
            _popupWidget = QSharedPointer<QWidget>(getWidget(State::Popup));
            _popupPushButton->setWidget(_popupWidget.get());
            break;
        }

        case State::Compact:
        case State::Full:
        {
            if (!_stateWidget.isNull())
                _stateWidget->removeEventFilter(this);

            _popupWidget.reset();
            _stateWidget = QSharedPointer<QWidget>(getWidget(_state));
            _layout->addWidget(_stateWidget.get());
            _stateWidget->installEventFilter(this);
            break;
        }

        default:
            break;
    }

    //qDebug() << QString("%1 state changed to:").arg(_name.toLower()) << static_cast<std::int32_t>(_state);
}

std::int32_t ResponsiveSectionWidget::getPriority() const
{
    return _priority;
}

void ResponsiveSectionWidget::setPriority(const std::int32_t& priority)
{
    if (priority == _priority)
        return;

    _priority = priority;
}

QSize ResponsiveSectionWidget::getStateSizeHint(const State& state) const
{
    return _stateSizeHints[static_cast<std::int32_t>(state)];
}

QWidget* ResponsiveSectionWidget::getWidget(const State& state)
{
    qDebug() << QString("Create %1 state widget: %2").arg(_name, QString::number(static_cast<std::int32_t>(state)));

    auto widget = _getWidgetStateFn(state);

    widget->setWindowTitle(_name);

    if (_initializeWidgetFn)
        _initializeWidgetFn(widget);

    return widget;
}

void ResponsiveSectionWidget::computeSizeHints()
{
    _stateSizeHints[static_cast<std::int32_t>(State::Compact)]    = computeStateSizeHint(State::Compact);
    _stateSizeHints[static_cast<std::int32_t>(State::Full)]       = computeStateSizeHint(State::Full);

    qDebug() << QString("Computed %1 size hints:").arg(_name.toLower()) << _stateSizeHints;
}

QSize ResponsiveSectionWidget::computeStateSizeHint(const State& state)
{
    //qDebug() << QString("Computing state %1 size hints %2").arg(_name.toLower(), QString::number(static_cast<std::int32_t>(state))) << _stateSizeHints;
    
    if (state == State::Popup)
        return _popupPushButton->sizeHint();

    auto stateWidget = QSharedPointer<QWidget>(getWidget(state));
    
    stateWidget->setObjectName(QString("Size hint widget: %1").arg(static_cast<std::int32_t>(state)));
    stateWidget->setAttribute(Qt::WA_DontShowOnScreen);
    stateWidget->show();

    return stateWidget->sizeHint();
}

}
}