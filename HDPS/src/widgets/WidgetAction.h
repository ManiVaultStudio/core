#pragma once

#include "PopupPushButton.h"

#include <QWidgetAction>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QToolButton>

namespace hdps {

namespace gui {

class WidgetAction : public QWidgetAction
{
    Q_OBJECT

public:
    enum class WidgetType {
        Standard,
        Compact,
        Popup
    };

public:
    class Widget : public QWidget
    {
    public:
        Widget(QWidget* parent, QAction* action);

        void setLayout(QLayout* layout);

        bool isChildOfMenu() const;

    protected:
        QAction*  _action;
    };

    class PopupWidget : public Widget {
    public:
        PopupWidget(QWidget* parent, QAction* action);
    };

    class CompactWidget : public Widget {
    public:
        CompactWidget(QWidget* parent, WidgetAction* widgetAction);

    private:
        QHBoxLayout         _layout;
        PopupPushButton     _popupPushButton;
    };

    class StateWidget : public Widget {
    public:
        StateWidget(QWidget* parent, WidgetAction* widgetAction, const std::int32_t& priority = 0, const WidgetType& state = WidgetType::Compact);

        WidgetType getState() const;
        void setState(const WidgetType& state);

        std::int32_t getPriority() const;
        void setPriority(const std::int32_t& priority);

        QSize getSizeHint(const WidgetType& state) const;

    private:
        WidgetType      _state;
        std::int32_t    _priority;
        QHBoxLayout     _layout;
        QWidget*        _standardWidget;
        QWidget*        _compactWidget;
    };

    explicit WidgetAction(QObject* parent);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    virtual QWidget* createWidget(QWidget* parent, const WidgetType& widgetType = WidgetType::Standard) {
        switch (widgetType)
        {
            case WidgetType::Standard:
                return new Widget(parent, this);

            case WidgetType::Compact:
                return new CompactWidget(parent, this);

            case WidgetType::Popup:
                return new PopupWidget(parent, this);

            default:
                break;
        }

        return nullptr;
    }
};

}
}