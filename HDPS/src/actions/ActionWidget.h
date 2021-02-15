#pragma once

#include <QWidget>

class QAction;

class ActionWidget : public QWidget
{
public:
    ActionWidget(QWidget* parent, QAction* action);

    bool childOfMenu() const;

    bool childOfToolbar() const;

    bool childOfWidget() const;

    bool eventFilter(QObject* object, QEvent* event);

protected:
    QAction*  _action;
};