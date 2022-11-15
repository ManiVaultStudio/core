#include "DockWidget.h"

#include <ViewPlugin.h>

#include <DockWidgetTab.h>

#include <QToolButton>

#ifdef _DEBUG
    #define DOCK_WIDGET_VERBOSE
#endif

using namespace ads;

using namespace hdps::util;
using namespace hdps::plugin;

DockWidget::DockWidget(const QString& title, QWidget* parent /*= nullptr*/) :
    CDockWidget(title, parent),
    Serializable(title)
{
    //tabWidget()->layout()->addWidget(new QToolButton("Do it!"));
}

void DockWidget::fromVariantMap(const QVariantMap& variantMap)
{

}

QVariantMap DockWidget::toVariantMap() const
{
    return {
        { "Title", windowTitle() }
    };
}
