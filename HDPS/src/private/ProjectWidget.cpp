#include "ProjectWidget.h"

#include <Application.h>
#include <CoreInterface.h>
#include <AbstractWorkspaceManager.h>

#include <QVBoxLayout>

using namespace hdps;

ProjectWidget::ProjectWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent)
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(workspaces().getWidget());

    setLayout(layout);
}
