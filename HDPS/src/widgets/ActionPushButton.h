#pragma once

#include <QPushButton>

class QAction;

namespace hdps {

namespace gui {

class ActionPushButton : public QPushButton {
    Q_OBJECT

private:

    QAction *actionOwner = nullptr;

public:
    explicit ActionPushButton(QWidget *parent = nullptr);
    explicit ActionPushButton(QAction *action);
    void setAction(QAction *action);

public slots:
    void updateButtonStatusFromAction();
};

}
}