#pragma once

#include <QWidgetAction>
#include <QVBoxLayout>
#include <QGroupBox>

namespace hdps {

namespace gui {

class WidgetAction : public QWidgetAction
{
    Q_OBJECT

public:
    class PopupWidget : public QWidget {
    public:
        PopupWidget(QWidget* parent, const QString& title);

        void setContentLayout(QLayout* layout);

    private:
        QVBoxLayout     _mainLayout;
        QGroupBox       _groupBox;
    };

    class Widget : public QWidget
    {
    public:
        Widget(QWidget* parent, QAction* action);

        bool isChildOfMenu() const;
        bool childOfToolbar() const;
        bool childOfWidget() const;

        bool eventFilter(QObject* object, QEvent* event);

    protected:
        QAction*  _action;
    };

    explicit WidgetAction(QObject* parent);
};

}
}