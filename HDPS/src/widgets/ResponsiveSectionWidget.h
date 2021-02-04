#pragma once

#include "PopupPushButton.h"

#include <QWidget>
#include <QMap>

class QHBoxLayout;

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

    /** Maps state name to state enum and vice versa */
    static QMap<QString, State> const states;

    /** Get string representation of state enum */
    static QString getStateName(const State& state) {
        return states.key(state);
    }

    /** Get enum representation from state name */
    static State getStateEnum(const QString& stateName) {
        return states[stateName];
    }

    using GetWidgetForStateFn = std::function<QWidget*(const State& state)>;
    using InitializeWidgetFn = std::function<void(QWidget* widget)>;

protected:
    /**
     * Constructor
     */
    ResponsiveSectionWidget(GetWidgetForStateFn getWidgetStateFn, InitializeWidgetFn initializeWidgetFn, const QString& name, const QIcon& icon = QIcon(), const std::int32_t& priority = 0);

public:
    bool eventFilter(QObject* object, QEvent* event);

    QString getName() const;
    
    /** Get/set state */
    State getState() const;
    void setState(const State& state);

    /** Get/set priority */
    std::int32_t getPriority() const;
    void setPriority(const std::int32_t& priority);

    QMap<State, QSize> getSizeHints() const;
    QSize getSizeHintForState(const State& state) const;
    QSize getSizeHintForState(const std::int32_t& state) const;

private:

    /**
     * Get widget for \p state
     * @param state State to query the widget for
     * @return Pointer to widget for \p state
     */
    QSharedPointer<QWidget> getWidgetForState(const State& state);

    /**
     * Precomputes the size hint for \p state
     * @param state State to precompute the size hint for
     */
    void precomputeSizeHintForState(const State& state);

protected:
    GetWidgetForStateFn                 _getWidgetStateFn;
    InitializeWidgetFn                  _initializeWidgetFn;
    State                               _state;
    QString                             _name;
    std::int32_t                        _priority;
    QHBoxLayout*                        _layout;
    QSharedPointer<PopupPushButton>     _popupPushButton;
    QSharedPointer<QWidget>             _popupWidget;
    QSharedPointer<QWidget>             _stateWidget;
    QMap<State, QSize>                  _sizeHints;

    friend class ResponsiveToolBar;
};

}
}