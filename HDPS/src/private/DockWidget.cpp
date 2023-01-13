#include "DockWidget.h"

#include <ViewPlugin.h>

#include <util/Serialization.h>
#include <Application.h>

#include <DockWidgetTab.h>

#include <QBoxLayout>
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
    _infoOverlayWidget(this, Application::getIconFont("FontAwesome").getIcon("hourglass-half"), "Loading", QString("Waiting for %1 to load...").arg(title)),
    _settingsToolButton(nullptr)
{
#ifdef DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__ << title;
#endif

    setObjectName(getId());
    setMinimumSizeHintMode(CDockWidget::MinimumSizeHintFromContent);

    auto& widgetFader = _infoOverlayWidget.getWidgetFader();

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

DockWidget::~DockWidget()
{
#ifdef DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__ << getSerializationName();
#endif

    takeWidget();
}

QString DockWidget::getTypeString() const
{
    return "DockWidget";
}

void DockWidget::showEvent(QShowEvent* showEvent)
{
    CDockWidget::showEvent(showEvent);

    if (_settingsToolButton != nullptr) {
        auto settingsMenu = this->getSettingsMenu();

        if (settingsMenu)
            _settingsToolButton->setMenu(settingsMenu);
    }
}

hdps::gui::InfoOverlayWidget& DockWidget::getInfoOverlayWidget()
{
    return _infoOverlayWidget;
}

QMenu* DockWidget::getSettingsMenu()
{
    return nullptr;
}

void DockWidget::setWidget(QWidget* widget, eInsertMode insertMode /*= AutoScrollArea*/)
{
    CDockWidget::setWidget(widget, insertMode);

    _infoOverlayWidget.raise();
    _infoOverlayWidget.hide();
}

void DockWidget::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Title");

    setObjectName(getId());
    setWindowTitle(variantMap["Title"].toString());
}

QVariantMap DockWidget::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    variantMap.insert({
        { "Title", QVariant::fromValue(windowTitle()) },
        { "Type", QVariant::fromValue(this->getTypeString()) }
    });

    return variantMap;
}
