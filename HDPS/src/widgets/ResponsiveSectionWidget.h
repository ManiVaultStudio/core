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
    /** State enumeration flags */
    enum class State {
        Undefined   = 0x0001,   /** Undefined */
        Collapsed   = 0x0002,   /** Single popup push button */
        Compact     = 0x0004,   /** Compact */
        Full        = 0x0008,   /** Show entire user interface */
    };

    /** Maps state name to state enum */
    static QMap<QString, State> const states;

    /** Get string/enum representation of state */
    static QString getStateName(const State& state) { return states.key(state); }
    static State getStateEnum(const QString& stateName) { return states[stateName]; }

    /** Determines whether shrink/grow is possible */
    static bool canShrink(const State& state) { return state == State::Compact || state == State::Full; }
    static bool canGrow(const State& state) { return state == State::Collapsed || state == State::Compact; }

    /** Shrink */
    static State shrink(const State& state) { 
        switch (state)
        {
            case State::Collapsed:  return State::Collapsed;
            case State::Compact:    return State::Collapsed;
            case State::Full:       return State::Compact;

            default:
                break;
        }

        return State::Undefined;
    }

    /** Grow */
    static State grow(const State& state) {
        switch (state)
        {
            case State::Collapsed:  return State::Compact;
            case State::Compact:    return State::Full;
            case State::Full:       return State::Full;

            default:
                break;
        }

        return State::Undefined;
    }

    /** Widget state enumeration flags */
    enum class WidgetStateFlag {
        Sequential  = 0x0010,                   /** Horizontal box layout */
        Form        = 0x0020,                   /** Form layout */

        CompactSequential   = static_cast<int>(State::Compact) | Sequential,
        FullSequential      = static_cast<int>(State::Full) | Sequential,

        CompactForm         = static_cast<int>(State::Compact) | Form,
        FullForm            = static_cast<int>(State::Collapsed) | Form,
    };

    Q_DECLARE_FLAGS(WidgetState, WidgetStateFlag)

    /** Maps widget state name to widget state enum */
    static QMap<QString, WidgetStateFlag> const widgetStates;

    /** Get string/enum representation of widget state */
    static QString getWidgetStateName(const WidgetStateFlag& widgetState) { return widgetStates.key(widgetState); }
    static WidgetStateFlag getWidgetStateEnum(const QString& widgetStateName) { return widgetStates[widgetStateName]; }

    using GetWidgetFn = std::function<QWidget*(const WidgetState& widgetState)>;
    using InitializeWidgetFn = std::function<void(QWidget* widget)>;

protected:
    /**
     * Constructor
     */
    ResponsiveSectionWidget(GetWidgetFn getWidgetFn, InitializeWidgetFn initializeWidgetFn, const QString& name, const QIcon& icon = QIcon(), const std::int32_t& priority = 0);

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

private:

    /**
     * Get widget for \p widgetState
     * @param state State to query the widget for
     * @return Pointer to widget for \p widgetState
     */
    QSharedPointer<QWidget> getWidget(const WidgetState& widgetState);

    /**
     * Computes the size hint for \p widgetState
     * @param widgetState Widget state to compute the size hint for
     */
    void computeSizeHintForState(const State& state);

    /** Compute size hints for all states */
    void computeSizeHints();

protected:
    GetWidgetFn                         _getWidgetFn;
    InitializeWidgetFn                  _initializeWidgetFn;
    State                               _state;
    QString                             _name;
    std::int32_t                        _priority;
    QHBoxLayout*                        _layout;
    QSharedPointer<PopupPushButton>     _popupPushButton;
    QSharedPointer<QWidget>             _formWidget;
    QSharedPointer<QWidget>             _sequentialWidget;
    QMap<State, QSize>                  _sizeHints;

    friend class ResponsiveToolBar;
};

}
}