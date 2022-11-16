#include "DockWidget.h"

#include <ViewPlugin.h>

#include <util/Serialization.h>
#include <Application.h>

#include <DockWidgetTab.h>

#include <QToolButton>

#ifdef _DEBUG
    #define DOCK_WIDGET_VERBOSE
#endif

using namespace ads;

using namespace hdps;
using namespace hdps::util;
using namespace hdps::plugin;

DockWidget::DockWidget(const QString& title, QWidget* parent /*= nullptr*/) :
    CDockWidget(title, parent),
    Serializable(title),
    _overlayWidget(this, Application::getIconFont("FontAwesome").getIcon("hourglass-half"), "Loading", QString("Waiting for %1 to load...").arg(title))
{
    //tabWidget()->layout()->addWidget(new QToolButton("Do it!"));

    auto& widgetFader = _overlayWidget.getWidgetFader();

    widgetFader.setMaximumOpacity(0.6);
    widgetFader.setFadeInDuration(250);
    widgetFader.setFadeOutDuration(800);
}

hdps::gui::OverlayWidget& DockWidget::getOverlayWidget()
{
    return _overlayWidget;
}

void DockWidget::setWidget(QWidget* widget, eInsertMode insertMode /*= AutoScrollArea*/)
{
    CDockWidget::setWidget(widget, insertMode);

    _overlayWidget.raise();
    _overlayWidget.hide();
}

void DockWidget::fromVariantMap(const QVariantMap& variantMap)
{
    variantMapMustContain(variantMap, "Title");
    variantMapMustContain(variantMap, "Closed");
    variantMapMustContain(variantMap, "Closable");
    variantMapMustContain(variantMap, "Movable");
    variantMapMustContain(variantMap, "Floatable");

    setWindowTitle(variantMap["Title"].toString());
    //toggleView(variantMap["Closed"].toBool());
    setFeature(CDockWidget::DockWidgetClosable, variantMap["Closable"].toBool());
    setFeature(CDockWidget::DockWidgetMovable, variantMap["Movable"].toBool());
    setFeature(CDockWidget::DockWidgetFloatable, variantMap["Floatable"].toBool());
}

QVariantMap DockWidget::toVariantMap() const
{
    return {
        { "Title", windowTitle() },
        { "Closed", isClosed() },
        { "Closable", features().testFlag(CDockWidget::DockWidgetClosable) },
        { "Movable", features().testFlag(CDockWidget::DockWidgetMovable) },
        { "Floatable", features().testFlag(CDockWidget::DockWidgetFloatable) }
    };
}
