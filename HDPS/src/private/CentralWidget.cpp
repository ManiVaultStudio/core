#include "CentralWidget.h"

#include <QVBoxLayout>

CentralWidget::CentralWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _logoWidget(this)
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(&_logoWidget);

    setLayout(layout);
}
