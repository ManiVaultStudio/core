#include "ProxyDatasetsAction.h"

#include <QHBoxLayout>

using namespace hdps;

ProxyDatasetsAction::ProxyDatasetsAction(QObject* parent, const Dataset<Points>& points) :
    WidgetAction(parent),
    _points(points),
    _countAction(this, "Number of proxy datasets"),
    _editProxyDatasetsAction(this, points)
{
    setText("Proxy datasets");
    setToolTip("Proxy datasets");

    _countAction.setEnabled(false);
    _countAction.setToolTip("Number of proxy datasets");

    const auto updateActions = [this]() -> void {
        _countAction.setString(QString::number(_points->getProxyDatasets().count()));
        _countAction.setToolTip(QString("%1 proxy datasets").arg(QString::number(_points->getProxyDatasets().count())));
        _editProxyDatasetsAction.setEnabled(_points->isProxy());
    };

    connect(&_points, &Dataset<Points>::dataChanged, this, updateActions);

    updateActions();
}

ProxyDatasetsAction::Widget::Widget(QWidget* parent, ProxyDatasetsAction* proxyDatasetsAction) :
    WidgetActionWidget(parent, proxyDatasetsAction)
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(proxyDatasetsAction->getCountAction().createWidget(this), 1);
    layout->addWidget(proxyDatasetsAction->getEditProxyDatasetsAction().createCollapsedWidget(this));

    setLayout(layout);
}