#include "StartPageWidget.h"

#include <Application.h>
#include <CoreInterface.h>

#include <widgets/Divider.h>

#include <QDebug>
#include <QFileInfo>
#include <QStyleOption>
#include <QScrollBar>
#include <QResizeEvent>
#include <QPainter>

using namespace hdps;
using namespace hdps::gui;

StartPageWidget::StartPageWidget(QWidget* parent /*= nullptr*/) :
    OverlayWidget(parent),
    _layout(),
    _centerColumnLayout(),
    _startPageContentWidget(this),
    _backgroundImage(":/Images/StartPageBackground")
{
    setLayout(&_layout);
    setMinimumWidth(500);

    _layout.setContentsMargins(0, 0, 0, 0);
    _layout.setSpacing(0);

    _centerColumnLayout.addWidget(new HeaderWidget(this));
    //_barLayout.addWidget(createHorizontalDivider());
    _centerColumnLayout.addWidget(&_startPageContentWidget);

    _layout.addStretch(1);
    _layout.addLayout(&_centerColumnLayout, 2);
    _layout.addStretch(1);

    getWidgetFader().setFadeInDuration(250);
    getWidgetFader().setFadeOutDuration(250);

    connect(this, &OverlayWidget::shown, this, [this]() -> void {
        setAttribute(Qt::WA_TransparentForMouseEvents, false);

        _startPageContentWidget.updateActions();
    });

    connect(this, &OverlayWidget::hidden, this, [this]() -> void {
        setAttribute(Qt::WA_TransparentForMouseEvents, true);
    });
}

void StartPageWidget::paintEvent(QPaintEvent* paintEvent)
{
    QPainter painter(this);

    auto backgroundImage    = _backgroundImage.scaled(width(), height(), Qt::KeepAspectRatioByExpanding);
    auto centerOfWidget     = rect().center();
    auto pixmapRectangle    = backgroundImage.rect();

    pixmapRectangle.moveCenter(centerOfWidget);

    painter.drawPixmap(pixmapRectangle.topLeft(), QPixmap(backgroundImage));
}

void StartPageWidget::setWidgetBackgroundColorRole(QWidget* widget, const QPalette::ColorRole& colorRole)
{
    Q_ASSERT(widget != nullptr);

    QPalette palette;

    QStyleOption styleOption;

    styleOption.initFrom(widget);

    palette.setColor(QPalette::Window, styleOption.palette.color(QPalette::Normal, colorRole));

    widget->setStyleSheet(QString("background-color: %1").arg(styleOption.palette.color(QPalette::Normal, colorRole).name(QColor::HexArgb)));
}

StartPageWidget::HeaderWidget::HeaderWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _layout(),
    _headerLabel()
{
    setLayout(&_layout);

    const int pixelRatio = devicePixelRatio();

    QString iconName = ":/Images/AppBackground256";

    if (pixelRatio > 1)
        iconName = ":/Images/AppBackground512";

    if (pixelRatio > 2)
        iconName = ":/Images/AppBackground1024";

    _headerLabel.setPixmap(QPixmap(iconName).scaled(256, 256));
    _headerLabel.setAlignment(Qt::AlignCenter);

    _layout.setContentsMargins(50, 50, 50, 50);
    _layout.addWidget(&_headerLabel);

    StartPageWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}

/*
StartPageWidget::ImportDataWidget::ImportDataWidget(QWidget* parent) :
    ScrollArea(parent),
    _containerWidget(this),
    _containerLayout()
{
    setFrameShape(QFrame::NoFrame);
    createContainerWidget();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setWidget(&_containerWidget);

    StartPageWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}

void StartPageWidget::ImportDataWidget::createContainerWidget()
{
    StartPageWidget::setWidgetBackgroundColorRole(&_containerWidget, QPalette::Midlight);

    _containerLayout.setContentsMargins(0, 0, 0, 0);

    const auto loaderPluginKinds = Application::core()->getPluginManager().getPluginKindsByPluginTypes({ plugin::Type::LOADER });

    for (const auto& pluginKind : loaderPluginKinds) {

        const auto title        = Application::core()->getPluginManager().getPluginGuiName(pluginKind);
        const auto description  = "Import data";
        const auto tooltip      = "Import data into HDPS with the " + pluginKind;

        _containerLayout.addWidget(new ProjectActionWidget(QIcon(), title, description, tooltip, [pluginKind]() {
            Application::core()->getPluginManager().requestPlugin(pluginKind);
        }));
    }

    _containerWidget.setLayout(&_containerLayout);
    _containerWidget.setVisible(!loaderPluginKinds.isEmpty());
}
*/