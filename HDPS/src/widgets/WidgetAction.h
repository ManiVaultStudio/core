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

    explicit WidgetAction(QObject* parent);

    virtual QWidget* getPopupWidget(QWidget* parent) {
        return createWidget(parent);
    };
};

}
}