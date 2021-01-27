#pragma once

#include "PopupPushButton.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QEvent>
#include <QDebug>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class QFrame;

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
    class QSpacer : public QWidget {
    public:
        QSpacer(QWidget* left, QWidget* right);

        bool eventFilter(QObject* target, QEvent* event);

    private:
        void updateState();

    protected:
        QWidget*        _left;
        QWidget*        _right;
        QHBoxLayout*    _layout;
        QFrame*         _verticalLine;
    };

    class SectionWidget;

    class StatefulWidget : public QWidget {
    public:
        StatefulWidget(QWidget* parent, const QString& name) :
            QWidget(parent),
            _name(name),
            _sizeHintWidget(new QWidget()),
            _sizeHints({QSize(), QSize(), QSize() })
        {
            setWindowTitle(_name);
            setToolTip(QString("%1 settings").arg(_name));
            //setObjectName("ResponsiveToolBar");
            //setStyleSheet("QWidget#ResponsiveToolBar { background-color:red; }");

            setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

            //_sizeHintWidget->setAttribute(Qt::WA_DontShowOnScreen);
        }

        virtual void setState(const WidgetState& state) {
            auto stateLayout = getLayout(state);

            if (layout())
                delete layout();

            stateLayout->setMargin(0);
            stateLayout->setSpacing(4);

            setLayout(stateLayout);
        }

        void computeSizeHints() {
            computeStateSizeHint(WidgetState::Popup);
            computeStateSizeHint(WidgetState::Compact);
            computeStateSizeHint(WidgetState::Full);

            qDebug() << _sizeHints;
        }

    protected:
        virtual QLayout* getLayout(const WidgetState& state) = 0;

        std::int32_t getWidth(const WidgetState& state) {
            switch (state)
            {
                case WidgetState::Compact:
                case WidgetState::Full:
                    return _sizeHints[static_cast<std::int32_t>(state)].width();

                default:
                    break;
            }

            return 0;
        };

    private:
        void computeStateSizeHint(const WidgetState& state) {
            if (_sizeHintWidget->layout())
                delete _sizeHintWidget->layout();

            auto stateLayout = getLayout(state);

            stateLayout->setMargin(0);
            stateLayout->setSpacing(4);

            _sizeHintWidget->setLayout(stateLayout);
            _sizeHintWidget->adjustSize();

            _sizeHints[static_cast<std::int32_t>(state)] = _sizeHintWidget->sizeHint();
        }

    protected:
        QString         _name;
        QWidget*        _sizeHintWidget;
        QList<QSize>    _sizeHints;

        friend class SectionWidget;
    };

    class SectionWidget : public QWidget {
    public:
        SectionWidget(StatefulWidget* statefulWidget, const QIcon& icon = QIcon(), const std::int32_t& priority = 0) :
            _statefulWidget(statefulWidget),
            _state(WidgetState::Undefined),
            _priority(priority),
            _modified(-1),
            _layout(new QHBoxLayout()),
            _popupPushButton(new PopupPushButton())
        {
            _popupPushButton->setIcon(icon);
            _popupPushButton->setWidget(_statefulWidget);
            //_popupPushButton->setWindowOpacity(0);

            _layout->setMargin(0);
            _layout->setSpacing(0);
            _layout->addWidget(_popupPushButton);
            _layout->addWidget(_statefulWidget);

            _statefulWidget->installEventFilter(this);
            //_statefulWidget->setWindowOpacity(0);

            setLayout(_layout);

            _statefulWidget->computeSizeHints();
        }

        bool eventFilter(QObject* object, QEvent* event)
        {
            auto widget = dynamic_cast<QWidget*>(object);

            if (widget == _statefulWidget && event->type() == QEvent::EnabledChange)
                _popupPushButton->setEnabled(_statefulWidget->isEnabled());

            if (widget == _statefulWidget && event->type() == QEvent::Resize)
                _modified++;

            return QObject::eventFilter(object, event);
        }

        WidgetState getState() const {
            return _state;
        }

        void setState(const WidgetState& state) {
            if (state == _state)
                return;

            auto previousState = _state;

            _state = state;

            _modified++;
            
            switch (_state)
            {
                case WidgetState::Popup:
                {
                    _layout->removeWidget(_statefulWidget);

                    _statefulWidget->hide();
                    _statefulWidget->setState(_state);
                    _popupPushButton->show();

                    break;
                }

                case WidgetState::Compact:
                case WidgetState::Full:
                {
                    _popupPushButton->hide();
                        
                    _layout->addWidget(_statefulWidget);
                    
                    _statefulWidget->setState(_state);
                    _statefulWidget->show();
                    
                    
                    break;
                }

                default:
                    break;
            }
        }

        bool isPopup() const {
            return _state == WidgetState::Popup;
        }

        void setPopup(const bool& popup) {
            if (popup == isPopup())
                return;

            setState(WidgetState::Popup);
        }

        bool isCompact() const {
            return _state == WidgetState::Compact;
        }

        void setCompact(const bool& compact) {
            if (compact == isCompact())
                return;

            setState(WidgetState::Compact);
        }

        std::int32_t getPriority() const {
            return _priority;
        }

        void setPriority(const std::int32_t& priority) {
            if (priority == _priority)
                return;

            _priority = priority;
        }

        std::int32_t getModified() const {
            return _modified;
        }

        std::int32_t getWidth(const WidgetState& state) const {
            switch (state)
            {
                case WidgetState::Popup:
                    return _popupPushButton->sizeHint().width();

                case WidgetState::Compact:
                case WidgetState::Full:
                    return _statefulWidget->getWidth(state);

                default:
                    break;
            }

            return 0;
        }

    private:
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
        StatefulWidget*     _statefulWidget;
        WidgetState         _state;
        std::int32_t        _priority;
        std::int32_t        _modified;
        QHBoxLayout*        _layout;
        PopupPushButton*    _popupPushButton;
    };

public:
    ResponsiveToolBar(QWidget* parent = nullptr);

    void setListenWidget(QWidget* listenWidget);

    bool eventFilter(QObject* object, QEvent* event);

    void addSection(StatefulWidget* statefulWidget, const QIcon& icon = QIcon(), const std::int32_t& priority = 0);
    void addStretch(const std::int32_t& stretch = 0);

private:
    void computeLayout(SectionWidget* sectionWidget = nullptr);
    QList<QWidget*> getVisibleWidgets();

private:
    QWidget*                _listenWidget;
    QHBoxLayout*            _layout;
    QList<SectionWidget*>   _sectionWidgets;
    std::int32_t            _modified;

public:
    static const std::int32_t LAYOUT_MARGIN;
    static const std::int32_t LAYOUT_SPACING;
};

}
}