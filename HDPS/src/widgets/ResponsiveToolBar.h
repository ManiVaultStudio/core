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
            class StatefulWidget : public QWidget {
            public:
                StatefulWidget(QWidget* parent, const QString& name) :
                    QWidget(parent),
                    _name(name),
                    _state(WidgetState::Full)
                {
                    setWindowTitle(_name);
                    setToolTip(QString("%1 settings").arg(_name));
                    //setObjectName("ResponsiveToolBar");
                    //setStyleSheet("QWidget#ResponsiveToolBar { background-color:red; }");

                    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

                    //_sizeHintWidget->setAttribute(Qt::WA_DontShowOnScreen);
                }

                QString getName() const {
                    return _name;
                }

                WidgetState getState() const {
                    return _state;
                }

                virtual void setState(const WidgetState& state, const bool& forceUpdate = false) {
                    /*
                    if (!forceUpdate && state == _state)
                        return;

                    _state = state;

                    applyLayout(_state);
                    */
                }

            protected:
                QString         _name;
                WidgetState     _state;
            };

            using GetWidgetStateFn = std::function<QWidget*(const WidgetState& state)>;

            class SectionWidget : public QWidget {
            public:
                SectionWidget(GetWidgetStateFn getWidgetStateFn, const QString& name, const QIcon& icon = QIcon(), const std::int32_t& priority = 0) :
                    _getWidgetStateFn(getWidgetStateFn),
                    _state(WidgetState::Undefined),
                    _name(name),
                    _priority(priority),
                    _layout(new QHBoxLayout()),
                    _popupPushButton(new PopupPushButton()),
                    _stateWidget(nullptr),
                    _stateSizeHints({ QSize(), QSize() , QSize() })
                {
                    _popupPushButton->setIcon(icon);
                    _popupPushButton->setWidget(getWidgetState(WidgetState::Popup));

                    _layout->setMargin(0);
                    _layout->setSpacing(0);
                    _layout->addWidget(_popupPushButton);

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
                                //computeSizeHints();
                                break;

                            default:
                                break;
                        }
                    }

                    return QObject::eventFilter(object, event);
                }

                QString getName() const {
                    return _name;
                }

                void setState(const WidgetState& state) {
                    if (state == _state)
                        return;

                    _state = state;

                    switch (_state)
                    {
                        case WidgetState::Popup:
                        {
                            _popupPushButton->show();

                            _stateWidget = QSharedPointer<QWidget>();

                            break;
                        }

                        case WidgetState::Compact:
                        case WidgetState::Full:
                        {
                            _popupPushButton->hide();

                            _stateWidget = QSharedPointer<QWidget>(_getWidgetStateFn(_state));

                            _stateWidget->installEventFilter(this);

                            _layout->addWidget(_stateWidget.get());
                            break;
                        }

                        default:
                            break;
                    }
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
                QWidget* getWidgetState(const WidgetState& state) {
                    auto widget = _getWidgetStateFn(WidgetState::Popup);
                    widget->setWindowTitle(_name);

                    return widget;
                }

                void computeSizeHints() {
                    _stateSizeHints[static_cast<std::int32_t>(WidgetState::Popup)]      = computeStateSizeHint(WidgetState::Popup);
                    _stateSizeHints[static_cast<std::int32_t>(WidgetState::Compact)]    = computeStateSizeHint(WidgetState::Compact);
                    _stateSizeHints[static_cast<std::int32_t>(WidgetState::Full)]       = computeStateSizeHint(WidgetState::Full);

                    qDebug() << QString("Computed %1 size hints:").arg(_name) << _stateSizeHints;
                }

                QSize computeStateSizeHint(const WidgetState& state) const {
                    if (state == WidgetState::Popup)
                        return _popupPushButton->sizeHint();

                    const auto stateWidget = _getWidgetStateFn(state);
                    const auto stateSizeHint = stateWidget->sizeHint();

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
                GetWidgetStateFn            _getWidgetStateFn;
                WidgetState                 _state;
                QString                     _name;
                std::int32_t                _priority;
                QHBoxLayout*                _layout;
                PopupPushButton*            _popupPushButton;
                QSharedPointer<QWidget>     _stateWidget;
                QList<QSize>                _stateSizeHints;
            };

        public:
            ResponsiveToolBar(QWidget* parent = nullptr);

            void setListenWidget(QWidget* listenWidget);

            bool eventFilter(QObject* object, QEvent* event);

            template<typename StatefulWidgetType>
            void addWidget(const QString& name, const QIcon& icon = QIcon(), const std::int32_t& priority = 0)
            {
                const auto getWidgetState = [](const WidgetState& state) -> QWidget* {
                    return new StatefulWidgetType(state);
                };

                auto sectionWidget = new SectionWidget(getWidgetState, name, icon, priority);

                _sectionWidgets << sectionWidget;

                _layout->addWidget(sectionWidget, 1, _sectionWidgets.count());
            }

            void addWidget(QWidget* widget)
            {
                _layout->addWidget(widget, 1, _sectionWidgets.count());
            }

        private:
            void computeLayout(QWidget* resizedWidget = nullptr);

        private:
            QWidget*                        _listenWidget;
            QGridLayout*                    _layout;
            QVector<SectionWidget*>         _sectionWidgets;
            std::int32_t                    _modified;

        public:
            static const std::int32_t LAYOUT_MARGIN;
            static const std::int32_t LAYOUT_SPACING;
        };

    }
}