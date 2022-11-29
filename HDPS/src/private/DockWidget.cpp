#include "DockWidget.h"

#include <ViewPlugin.h>

#include <util/Serialization.h>
#include <Application.h>

#include <DockWidgetTab.h>

#include <QBoxLayout>
#include <QToolButton>

#ifdef _DEBUG
    //#define DOCK_WIDGET_VERBOSE
#endif

using namespace ads;

using namespace hdps;
using namespace hdps::util;
using namespace hdps::plugin;

DockWidget::DockWidget(const QString& title, QWidget* parent /*= nullptr*/) :
    CDockWidget(title, parent),
    Serializable(title),
    _overlayWidget(this, Application::getIconFont("FontAwesome").getIcon("hourglass-half"), "Loading", QString("Waiting for %1 to load...").arg(title)),
    _settingsToolButton(nullptr)
{
#ifdef DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__ << title;
#endif

    setObjectName("DockWidget");

    auto& widgetFader = _overlayWidget.getWidgetFader();

    widgetFader.setMaximumOpacity(0.6f);
    widgetFader.setFadeInDuration(150);
    widgetFader.setFadeOutDuration(150);

    _settingsToolButton = new QToolButton();

    _settingsToolButton->setIcon(Application::getIconFont("FontAwesome").getIcon("bars"));
    _settingsToolButton->setToolTip("Adjust view settings");
    _settingsToolButton->setAutoRaise(true);
    _settingsToolButton->setIconSize(QSize(14, 14));
    _settingsToolButton->setFixedSize(QSize(14, 14));
    _settingsToolButton->setPopupMode(QToolButton::InstantPopup);
    _settingsToolButton->setStyleSheet("QToolButton::menu-indicator { image: none; }");
    _settingsToolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    dynamic_cast<QBoxLayout*>(tabWidget()->layout())->insertSpacing(1, 5);
    dynamic_cast<QBoxLayout*>(tabWidget()->layout())->insertWidget(2, _settingsToolButton, Qt::AlignCenter);
}

QString DockWidget::getTypeString() const
{
    return "DockWidget";
}

void DockWidget::showEvent(QShowEvent* showEvent)
{
    if (_settingsToolButton != nullptr) {
        auto settingsMenu = this->getSettingsMenu();

        if (settingsMenu)
            _settingsToolButton->setMenu(settingsMenu);
    }
}

hdps::gui::OverlayWidget& DockWidget::getOverlayWidget()
{
    return _overlayWidget;
}

QMenu* DockWidget::getSettingsMenu()
{
    return nullptr;
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
        { "Type", this->getTypeString() },
        { "Closed", isClosed() },
        { "Closable", features().testFlag(CDockWidget::DockWidgetClosable) },
        { "Movable", features().testFlag(CDockWidget::DockWidgetMovable) },
        { "Floatable", features().testFlag(CDockWidget::DockWidgetFloatable) }
    };
}
