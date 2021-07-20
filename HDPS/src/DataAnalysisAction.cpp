#include "DataAnalysisAction.h"
#include "Application.h"

#include <QMenu>

using namespace hdps::gui;

DataAnalysisAction::DataAnalysisAction(QObject* parent) :
    WidgetAction(parent),
    _meanShiftAction(this, "Mean-shift"),
    _tsneAction(this, "TSNE")
{
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("microchip"));
}

QMenu* DataAnalysisAction::getContextMenu()
{
    auto menu = new QMenu("Analysis");

    menu->setIcon(icon());

    menu->addAction(&_meanShiftAction);
    menu->addAction(&_tsneAction);

    menu->addSeparator();

    return menu;
}