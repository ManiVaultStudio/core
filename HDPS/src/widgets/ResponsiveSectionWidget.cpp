#include "ResponsiveSectionWidget.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QEvent>

namespace hdps {

namespace gui {

const QMap<QString, ResponsiveSectionWidget::State> ResponsiveSectionWidget::states = {
    { "Undefined", ResponsiveSectionWidget::State::Undefined },
    { "Collapsed", ResponsiveSectionWidget::State::Collapsed },
    { "Compact", ResponsiveSectionWidget::State::Compact },
    { "Full", ResponsiveSectionWidget::State::Full },
};

const QMap<QString, ResponsiveSectionWidget::WidgetStateFlag> ResponsiveSectionWidget::widgetStates = {
    { "Sequential", ResponsiveSectionWidget::WidgetStateFlag::Sequential },
    { "Form", ResponsiveSectionWidget::WidgetStateFlag::Form },
    { "Basic sequential", ResponsiveSectionWidget::WidgetStateFlag::CompactSequential },
    { "Full sequential", ResponsiveSectionWidget::WidgetStateFlag::FullSequential },
    { "Compact form", ResponsiveSectionWidget::WidgetStateFlag::CompactForm },
    { "Full form", ResponsiveSectionWidget::WidgetStateFlag::FullForm }
};

ResponsiveSectionWidget::ResponsiveSectionWidget(GetWidgetFn getWidgetFn, InitializeWidgetFn initializeWidgetFn, const QString& name, const QIcon& icon /*= QIcon()*/, const std::int32_t& priority /*= 0*/) :
    _getWidgetFn(getWidgetFn),
    _initializeWidgetFn(initializeWidgetFn),
    _state(State::Undefined),
    _name(name),
    _priority(priority),
    _layout(new QHBoxLayout()),
    _popupPushButton(QSharedPointer<PopupPushButton>::create()),
    _formWidget(),
    _sequentialWidget(),
    _sizeHints()
{
    _layout->setMargin(0);
    _layout->setSpacing(0);
    _layout->addWidget(_popupPushButton.get());

    setLayout(_layout);

    _popupPushButton->setIcon(icon);

    _formWidget         = QSharedPointer<QWidget>(getWidget(WidgetStateFlag::FullForm));
    _sequentialWidget   = QSharedPointer<QWidget>(getWidget(WidgetStateFlag::FullSequential));

    computeSizeHints();

    qDebug() << _sizeHints.values();
}

bool ResponsiveSectionWidget::eventFilter(QObject* object, QEvent* event)
{
    auto widget = dynamic_cast<QWidget*>(object);

    if (widget == _sequentialWidget) {
        switch (event->type())
        {
            case QEvent::EnabledChange:
                _popupPushButton->setEnabled(_sequentialWidget->isEnabled());
                break;

            case QEvent::Resize:
            {
                //if (_state.testFlag() & WidgetStateFlag::Sequential)
                    //computeSizeHints();

                break;
            }
                
        }
    }

    return QObject::eventFilter(object, event);
}

QString ResponsiveSectionWidget::getName() const
{
    return _name;
}

ResponsiveSectionWidget::State ResponsiveSectionWidget::getState() const
{
    return _state;
}

void ResponsiveSectionWidget::setState(const State& state)
{
    if (state == _state || state == State::Undefined)
        return;

    _state = state;

    _formWidget = QSharedPointer<QWidget>(getWidget(WidgetState(static_cast<int>(state)) | WidgetStateFlag::Form));

    _popupPushButton->setWidget(_formWidget.get());
    _popupPushButton->setVisible(state == State::Collapsed);

    _sequentialWidget = QSharedPointer<QWidget>(getWidget(WidgetState(static_cast<int>(state)) | WidgetStateFlag::Sequential));
    
    _layout->addWidget(_sequentialWidget.get());

    _sequentialWidget->setVisible(state != State::Collapsed);
    _sequentialWidget->installEventFilter(this);

    

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

QMap<ResponsiveSectionWidget::State, QSize> ResponsiveSectionWidget::getSizeHints() const
{
    return _sizeHints;
}

QSize ResponsiveSectionWidget::getSizeHintForState(const State& state) const
{
    return _sizeHints[state];
}

QSharedPointer<QWidget> ResponsiveSectionWidget::getWidget(const WidgetState& widgetState)
{
    auto widget = _getWidgetFn(widgetState);

    widget->setWindowTitle(_name);

    if (_initializeWidgetFn)
        _initializeWidgetFn(widget);

    return QSharedPointer<QWidget>(widget);
}

void ResponsiveSectionWidget::computeSizeHintForState(const State& state)
{
    switch (state)
    {
        case State::Undefined:
            break;

        case State::Collapsed:
            _sizeHints[state] = _popupPushButton->sizeHint();
            break;

        case State::Compact:
        case State::Full:
            _sizeHints[state] = getWidget(WidgetState(static_cast<int>(state)) | WidgetStateFlag::Sequential)->sizeHint();
            break;

        default:
            break;
    }
}

void ResponsiveSectionWidget::computeSizeHints()
{
    computeSizeHintForState(State::Collapsed);
    computeSizeHintForState(State::Compact);
    computeSizeHintForState(State::Full);
}

}
}