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

class ResponsiveSectionWidget : public QWidget {
    Q_OBJECT

public:
    enum class State
    {
        Undefined = -1,
        Popup,
        Compact,
        Full
    };

    using GetWidgetForStateFn = std::function<QWidget*(const State& state)>;
    using InitializeWidgetFn = std::function<void(QWidget* widget)>;

public:
    ResponsiveSectionWidget(GetWidgetForStateFn getWidgetStateFn, const QString& name, const QIcon& icon = QIcon(), const std::int32_t& priority = 0);

    bool eventFilter(QObject* object, QEvent* event);

    void setInitializeWidgetFunction(InitializeWidgetFn initializeWidgetFn);

    QString getName() const;

    State getState() const;

    void setState(const State& state);

    void showWidget(QWidget* widget, const bool& animate = false);

    void hideWidget(QWidget* widget, const bool& animate = false);

    void setStateWidget(QWidget* widget = nullptr);

    std::int32_t getPriority() const;

    void setPriority(const std::int32_t& priority);

    QSize getStateSizeHint(const State& state) const;

    QWidget* getWidget();

private:
    QWidget* getWidget(const State& state);

    void computeSizeHints();

    QSize computeStateSizeHint(const State& state);

    void animateOpacity(QWidget* widget, const float& startOpacity, const float& endOpacity, const std::int32_t& duration, std::function<void()> finishedCallback = std::function<void()>());

protected:
    GetWidgetForStateFn                    _getWidgetStateFn;
    InitializeWidgetFn                  _initializeWidgetFn;
    State                         _state;
    QString                             _name;
    std::int32_t                        _priority;
    QHBoxLayout*                        _layout;
    QSharedPointer<PopupPushButton>     _popupPushButton;
    QSharedPointer<QWidget>             _stateWidget;
    QList<QSize>                        _stateSizeHints;
};

}
}