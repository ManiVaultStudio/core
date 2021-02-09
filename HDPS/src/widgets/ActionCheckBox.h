#pragma once

#include <QCheckBox>

class QAction;

namespace hdps {

namespace gui {

class ActionCheckBox : public QCheckBox {

    Q_OBJECT

public:
    explicit ActionCheckBox(QWidget* parent = nullptr);
    explicit ActionCheckBox(QAction* action);

    void setAction(QAction* action);

public slots:
    void updateButtonStatusFromAction();

private:
    QAction*    _actionOwner = nullptr;
};

}
}