#include "CentralWidget.h"

#include <QVBoxLayout>

CentralWidget::CentralWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _dockManager(new ads::CDockManager(this))
    //_logoWidget(this)
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    _widget.setStyleSheet("background-color: green;");
    _widget.setStyleSheet("background-color: red;");

    //layout->addWidget(&_widget);
    layout->addWidget(_dockManager.get());

    setLayout(layout);
}

QSharedPointer<ads::CDockManager> CentralWidget::getDockManager()
{
    return _dockManager;
}
