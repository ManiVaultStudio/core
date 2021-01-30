#pragma once

#include "PopupPushButton.h"

#include <QWidget>
#include <QEvent>
#include <QDebug>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class QFrame;
class QGridLayout;

namespace hdps {

namespace gui {

class ResponsiveToolBar : public QWidget
{
    Q_OBJECT

public:
    enum class WidgetState {
        Undefined = -1,
        Popup,
        Compact,
        Full
    };

public:
    using GetWidgetStateFn = std::function<QWidget*(const WidgetState& state)>;
    using InitializeWidgetFn = std::function<void(QWidget* widget)>;

    class SectionWidget : public QWidget {
    public:
        SectionWidget(GetWidgetStateFn getWidgetStateFn, const QString& name, const QIcon& icon = QIcon(), const std::int32_t& priority = 0) :
            _getWidgetStateFn(getWidgetStateFn),
            _initializeWidgetFn(),
            _state(WidgetState::Undefined),
            _name(name),
            _priority(priority),
            _layout(new QHBoxLayout()),
            _popupPushButton(QSharedPointer<PopupPushButton>::create()),
            _stateWidget(nullptr),
            _stateSizeHints({ QSize(), QSize() , QSize() })
        {
            _popupPushButton->setIcon(icon);
                        
            hideWidget(_popupPushButton.get());

            _layout->setMargin(0);
            _layout->setSpacing(0);
            _layout->addWidget(_popupPushButton.get());

            setLayout(_layout);

            computeSizeHints();
        }

        bool eventFilter(QObject* object, QEvent* event)
        {
            auto widget = dynamic_cast<QWidget*>(object);

            if (widget == _stateWidget) {
                switch (event->type())
                {
                    case QEvent::EnabledChange:
                        _popupPushButton->setEnabled(_stateWidget->isEnabled());
                        break;

                    case QEvent::Resize:
                        computeSizeHints();
                        break;

                    default:
                        break;
                }
            }

            return QObject::eventFilter(object, event);
        }

        void setInitializeWidgetFunction(InitializeWidgetFn initializeWidgetFn) {
            _initializeWidgetFn = initializeWidgetFn;
        }

        QString getName() const {
            return _name;
        }

        WidgetState getState() const {
            return _state;
        }

        void setState(const WidgetState& state) {
            if (state == _state)
                return;

            _state = state;

            switch (_state)
            {
                case WidgetState::Popup:
                {
                    hideWidget(_stateWidget.get(), false);
                    showWidget(_popupPushButton.get(), false);
                    setStateWidget(getWidget(_state));
                    _popupPushButton->setWidget(_stateWidget.get());
                    
                    break;
                }

                case WidgetState::Compact:
                case WidgetState::Full:
                {
                    hideWidget(_popupPushButton.get(), false);
                    setStateWidget(getWidget(_state));
                    _layout->addWidget(_stateWidget.get());
                    showWidget(_stateWidget.get(), false);
                    break;
                }

                default:
                    break;
            }

            qDebug() << QString("%1 state changed to:").arg(_name.toLower()) << static_cast<std::int32_t>(_state);
        }

        void showWidget(QWidget* widget, const bool& animate = false) {
            if (widget == nullptr)
                return;

            if (animate) {
                widget->show();
                animateOpacity(widget, 0, 1, 300);
            }
            else {
                widget->show();
            }
        }

        void hideWidget(QWidget* widget, const bool& animate = false) {
            if (widget == nullptr)
                return;

            if (animate) {
                animateOpacity(widget, 1, 0, 300, [this, widget]() {
                    widget->hide();
                });
            }
            else {
                widget->hide();
            }
        }

        void setStateWidget(QWidget* widget = nullptr) {
            if (widget == nullptr)
                return;

            _stateWidget = QSharedPointer<QWidget>(widget);
                
            _stateWidget->setWindowOpacity(0);
            _stateWidget->installEventFilter(this);
        }

        std::int32_t getPriority() const {
            return _priority;
        }

        void setPriority(const std::int32_t& priority) {
            if (priority == _priority)
                return;

            _priority = priority;
        }

        QSize getStateSizeHint(const WidgetState& state) const {
            return _stateSizeHints[static_cast<std::int32_t>(state)];
        }

    private:
        QWidget* getWidget(const WidgetState& state) {
            auto widget = _getWidgetStateFn(state);

            widget->setWindowTitle(_name);
                    
            if (_initializeWidgetFn)
                _initializeWidgetFn(widget);

            return widget;
        }

        void computeSizeHints() {
            _stateSizeHints[static_cast<std::int32_t>(WidgetState::Popup)]      = computeStateSizeHint(WidgetState::Popup);
            _stateSizeHints[static_cast<std::int32_t>(WidgetState::Compact)]    = computeStateSizeHint(WidgetState::Compact);
            _stateSizeHints[static_cast<std::int32_t>(WidgetState::Full)]       = computeStateSizeHint(WidgetState::Full);

            qDebug() << QString("Computed %1 size hints:").arg(_name.toLower()) << _stateSizeHints;
        }

        QSize computeStateSizeHint(const WidgetState& state) {
            if (state == WidgetState::Popup)
                return _popupPushButton->sizeHint();

            const auto stateWidget      = getWidget(state);
            const auto stateSizeHint    = stateWidget->sizeHint();

            delete stateWidget;

            return stateSizeHint;
        }

        void animateOpacity(QWidget* widget, const float& startOpacity, const float& endOpacity, const std::int32_t& duration, std::function<void()> finishedCallback = std::function<void()>()) {
            QGraphicsOpacityEffect* graphicsOpacityEffect = new QGraphicsOpacityEffect(widget);

            widget->setGraphicsEffect(graphicsOpacityEffect);

            QPropertyAnimation* propertyAnimation = new QPropertyAnimation(graphicsOpacityEffect, "opacity");

            propertyAnimation->setDuration(duration);
            propertyAnimation->setStartValue(startOpacity);
            propertyAnimation->setEndValue(endOpacity);
            //propertyAnimation->setEasingCurve(QEasingCurve::InBack);
            propertyAnimation->start(QPropertyAnimation::DeleteWhenStopped);

            connect(propertyAnimation, &QPropertyAnimation::finished, [finishedCallback]() {
                if (finishedCallback)
                    finishedCallback();
            });
        }

    protected:
        GetWidgetStateFn                    _getWidgetStateFn;
        InitializeWidgetFn                  _initializeWidgetFn;
        WidgetState                         _state;
        QString                             _name;
        std::int32_t                        _priority;
        QHBoxLayout*                        _layout;
        QSharedPointer<PopupPushButton>     _popupPushButton;
        QSharedPointer<QWidget>             _stateWidget;
        QList<QSize>                        _stateSizeHints;
    };

    class Spacer : public QWidget {
    public:
        enum class Type {
            Divider,
            Spacer
        };

    public:
        Spacer(const Type& type = Type::Divider);

        void setType(const Type& type);

        static std::int32_t getWidth(const Type& type);

        static Type getType(const WidgetState& stateBefore, const WidgetState& stateAfter);

        static Type getType(const SectionWidget* sectionBefore, const SectionWidget* sectionAfter);

    protected:
        Type            _type;
        QHBoxLayout*    _layout;
        QFrame*         _verticalLine;
    };

public:
    ResponsiveToolBar(QWidget* parent = nullptr);

    void setListenWidget(QWidget* listenWidget);

    bool eventFilter(QObject* object, QEvent* event);

    template<typename StatefulWidgetType>
    void addWidget(InitializeWidgetFn initializeWidgetFn, const QString& name, const QIcon& icon = QIcon(), const std::int32_t& priority = 0)
    {
        const auto getWidgetState = [](const WidgetState& state) -> QWidget* {
            return new StatefulWidgetType(state);
        };

        auto sectionWidget = new SectionWidget(getWidgetState, name, icon, priority);

        sectionWidget->setInitializeWidgetFunction(initializeWidgetFn);
        sectionWidget->installEventFilter(this);

        _sections << sectionWidget;

        if (_sections.count() >= 2) {
            auto spacer = new Spacer();

            _spacers << spacer;

            _layout->addWidget(spacer);
        }
        
        _layout->addWidget(sectionWidget);
    }

private:
    void computeLayout(QWidget* resizedWidget = nullptr);

private:
    QWidget*                    _listenWidget;
    QHBoxLayout*                _layout;
    QVector<SectionWidget*>     _sections;
    QVector<Spacer*>            _spacers;
    std::int32_t                _modified;

public:
    static const std::int32_t LAYOUT_MARGIN;
    static const std::int32_t LAYOUT_SPACING;
};

}
}