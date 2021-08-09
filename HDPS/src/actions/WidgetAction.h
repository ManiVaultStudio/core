#pragma once

#include <QWidgetAction>
#include <QVBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QMenu>

class QLabel;

namespace hdps {

namespace gui {

/**
 * Widget action class
 *
 * This widget action class creates widgets for user defined actions
 * 
 * @author Thomas Kroes
 */
class WidgetAction : public QWidgetAction
{
    Q_OBJECT

public:
    class Widget : public QWidget
    {
    public:
        enum class State {
            Standard,
            Collapsed,
            Popup
        };

    public:
        Widget(QWidget* parent, WidgetAction* widgetAction, const State& type);

    protected:
        void setPopupLayout(QLayout* popupLayout);

    protected:
        WidgetAction*   _widgetAction;
        State           _state;
    };

    class Label : public QLabel {
    public:
        explicit Label(WidgetAction* widgetAction, QWidget* parent = nullptr, Qt::WindowFlags windowFlags = Qt::WindowFlags());

        void enterEvent(QEvent *ev) override
        {
            setStyleSheet("QLabel { text-decoration: underline; }");
        }

        void leaveEvent(QEvent *ev) override
        {
            setStyleSheet("QLabel { text-decoration: none; }");
        }
    };

    class ToolButton : public QToolButton {
    public:
        void paintEvent(QPaintEvent* paintEvent);
    };

    class CollapsedWidget : public Widget {
    public:
        CollapsedWidget(QWidget* parent, WidgetAction* widgetAction);

        ToolButton& getToolButton() { return _toolButton; }

    private:
        QHBoxLayout     _layout;
        ToolButton      _toolButton;
    };

    class StateWidget : public QWidget {
    public:
        StateWidget(QWidget* parent, WidgetAction* widgetAction, const std::int32_t& priority = 0, const Widget::State& state = Widget::State::Collapsed);

        Widget::State getState() const;
        void setState(const Widget::State& state);

        std::int32_t getPriority() const;
        void setPriority(const std::int32_t& priority);

        QSize getSizeHint(const Widget::State& state) const;

    private:
        WidgetAction*   _widgetAction;
        Widget::State   _state;
        std::int32_t    _priority;
        QWidget*        _standardWidget;
        QWidget*        _compactWidget;
    };

    explicit WidgetAction(QObject* parent);

    QWidget* createWidget(QWidget* parent) override {
        if (parent != nullptr && dynamic_cast<ToolButton*>(parent->parent()))
            return getWidget(parent, Widget::State::Popup);

        return getWidget(parent, Widget::State::Standard);
    }

    QWidget* createCollapsedWidget(QWidget* parent) {
        return new CollapsedWidget(parent, this);

    }

    Label* createLabelWidget(QWidget* parent);

    WidgetAction& operator= (const WidgetAction& other)
    {
        setText(other.text());
        setToolTip(other.toolTip());
        setCheckable(other.isCheckable());
        setChecked(other.isChecked());

        return *this;
    }

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    virtual QMenu* getContextMenu(QWidget* parent = nullptr) {
        return new QMenu(parent);
    };

protected:
    virtual QWidget* getWidget(QWidget* parent, const Widget::State& state = Widget::State::Standard);
};

/** List of widget actions */
using WidgetActions = QVector<WidgetAction*>;

}
}