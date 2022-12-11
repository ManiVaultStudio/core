#include "ActionsWidget.h"

#include <CoreInterface.h>
#include <AbstractActionsManager.h>
#include <Application.h>

#include <QDebug>

using namespace hdps;

ActionsWidget::ActionsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _filterModel(this),
    _hierarchyWidget(this, "Public action", const_cast<QAbstractItemModel&>(Application::core()->getActionsManager().getModel()), &_filterModel)
{
    _hierarchyWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("running"));
    _hierarchyWidget.getTreeView().setRootIsDecorated(false);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(&_hierarchyWidget);

    setLayout(layout);
}
