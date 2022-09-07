#include "EditProxyDatasetsAction.h"

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

hdps::Dataset<Points>& EditProxyDatasetsAction::getPoints()
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
    
    const auto updateListView = [this, editProxyDatasetsAction, listView]() -> void {
        QStringList proxyMemberNames;

        for (const auto& proxyMember : editProxyDatasetsAction->getPoints()->getProxyMembers())
            proxyMemberNames << proxyMember->getGuiName();

        listView->setModel(new QStringListModel(proxyMemberNames));
    };

    connect(&editProxyDatasetsAction->getPoints(), &Dataset<Points>::dataChanged, this, updateListView);

    updateListView();
}
