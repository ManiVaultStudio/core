#include "EditProxyDatasetsAction.h"

#include <event/Event.h>

#include <QHBoxLayout>
#include <QListView>
#include <QStringListModel>

EditProxyDatasetsAction::EditProxyDatasetsAction(QObject* parent, const Dataset<Points>& points) :
    WidgetAction(parent),
    _points(points)
{
    setText("View proxy datasets");
    setToolTip("View proxy datasets");
    setIcon(Application::getIconFont("FontAwesome").getIcon("list"));
}

Dataset<Points> EditProxyDatasetsAction::getPoints()
{
    return _points;
}

EditProxyDatasetsAction::Widget::Widget(QWidget* parent, EditProxyDatasetsAction* editProxyDatasetsAction) :
    WidgetActionWidget(parent, editProxyDatasetsAction)
{
    auto layout     = new QHBoxLayout();
    auto listView   = new QListView();

    listView->setFixedHeight(100);

    layout->addWidget(listView);

    setPopupLayout(layout);
    
    auto points = editProxyDatasetsAction->getPoints();

    const auto updateListView = [this, points, listView]() -> void {
        QStringList proxyDatasetNames;

        for (const auto& proxyDataset : points->getProxyDatasets())
            proxyDatasetNames << proxyDataset->getGuiName();

        listView->setModel(new QStringListModel(proxyDatasetNames));
    };

    connect(&points, &Dataset<Points>::dataChanged, this, updateListView);

    updateListView();
}
