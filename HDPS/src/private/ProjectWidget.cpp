#include "ProjectWidget.h"

ProjectWidget::ProjectWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //setStyleSheet("background-color: red;");
}
