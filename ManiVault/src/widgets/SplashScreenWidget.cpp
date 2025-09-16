// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright &copy; 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SplashScreenWidget.h"
#include "ProjectMetaAction.h"
#include "CoreInterface.h"
#include "Application.h"
#include "ManiVaultVersion.h"

#include "actions/SplashScreenAction.h"

#include "widgets/ElidedLabel.h"

#include <QScreen>
#include <QPainter>
#include <QPainterPath>
#include <QEventLoop>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QGraphicsDropShadowEffect>
#include <QDesktopServices>

#ifdef _DEBUG
    #define SPLASH_SCREEN_WIDGET_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui {

/** Custom QWebEnginePage to handle JavaScript console messages and navigation requests */
class Page : public QWebEnginePage
{
public:

    /** No need for custom constructor */
    using QWebEnginePage::QWebEnginePage;

protected:

    /**
     * Handle JavaScript console messages
     * @param level Level of the message
     * @param message Message content
     * @param line Line number
     * @param source Source file
     */
    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString& message, int line, const QString& source) override {
        qDebug().noquote() << "[JS]" << message << "(line" << line << "src" << source << ")";
    }

    /**
     * Handle navigation requests
     * @param url URL to navigate to
     * @param type Type of navigation
     * @param isMainFrame Whether the navigation is for the main frame
     * @return 
     */
    bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame) override
    {
        if (type == QWebEnginePage::NavigationTypeLinkClicked) {
	        QDesktopServices::openUrl(url);

	        return false;
        }

        return true;
    }
};

SplashScreenWidget::SplashScreenWidget(SplashScreenAction& splashScreenAction, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _splashScreenAction(splashScreenAction),
    _logoImage(":/Icons/AppIcon256"),
    _backgroundImage(":/Images/SplashScreenBackground"),
    _closeToolButton(&_roundedFrame),
    _webChannel(&_webEngineView),
    _splashScreenBridge(&_webEngineView)
{
    setObjectName("SplashScreenWidget");
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window | Qt::WindowStaysOnTopHint);
    setFixedSize(QSize(SplashScreenWidget::fixedWidth + (2 * SplashScreenWidget::shadowMargin), SplashScreenWidget::fixedHeight + (2 * SplashScreenWidget::shadowMargin)));
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(0.0);
    
#ifdef Q_OS_MACX // adjust the shadow to resemble the macOS system shadows a bit more
    _dropShadowEffect.setColor(QColor(0, 0, 0, 100));
    _dropShadowEffect.setBlurRadius(50);
    _dropShadowEffect.setOffset(0);
#else
    _dropShadowEffect.setColor(QColor(0, 0, 0, 50));
    _dropShadowEffect.setBlurRadius(7);
    _dropShadowEffect.setOffset(7);
#endif

    connect(&_processEventsTimer, &QTimer::timeout, this, []() -> void {
        QCoreApplication::processEvents();
    });

    _processEventsTimer.start(10);

    _roundedFrameLayout.setContentsMargins(0, 0, 0, 0);
    _roundedFrameLayout.addWidget(&_webEngineView);

    _roundedFrame.setObjectName("RoundedFrame");
    _roundedFrame.setContentsMargins(0, 0, 0, 0);
    _roundedFrame.setLayout(&_roundedFrameLayout);

    auto mainLayout = new QVBoxLayout();

    mainLayout->setContentsMargins(SplashScreenWidget::shadowMargin, SplashScreenWidget::shadowMargin, SplashScreenWidget::shadowMargin, SplashScreenWidget::shadowMargin);
    mainLayout->addWidget(&_roundedFrame);

    setLayout(mainLayout);
    setGraphicsEffect(&_dropShadowEffect);

    _webChannel.registerObject(QStringLiteral("bridge"), &_splashScreenBridge);

    _webEngineView.setPage(new Page(&_webEngineView));
    _webEngineView.page()->setWebChannel(&_webChannel);

    connect(_splashScreenAction.getTaskAction().getTask(), &Task::progressChanged, this, [this](float progress) -> void {
        if (!Application::current()->getStartupTask().getEnabled())
            return;

        if (!_initialized)
            return;

        QMetaObject::invokeMethod(&_splashScreenBridge, "setProgress", Qt::QueuedConnection, Q_ARG(int, static_cast<int>(100.0f * progress)));

        _webEngineView.update();
    });

    connect(&Application::current()->getStartupTask(), &Task::progressDescriptionChanged, this, [this](const QString& progressDescription) -> void {
        if (!Application::current()->getStartupTask().getEnabled())
            return;

        QMetaObject::invokeMethod(&_splashScreenBridge, "setProgressDescription", Qt::QueuedConnection, Q_ARG(QString, progressDescription));

        _webEngineView.update();
    });
}

SplashScreenWidget::~SplashScreenWidget()
{
    _processEventsTimer.stop();
}

void SplashScreenWidget::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);

    if (_initialized)
        return;

    _initialized = true;

    
    _webEngineView.setHtml(_splashScreenAction.getHtml(), QUrl("qrc:/"));

    QEventLoop loop;

    connect(&_webEngineView, &QWebEngineView::loadFinished, [this, &loop]() -> void {
        QMetaObject::invokeMethod(&_splashScreenBridge, "requestInitial", Qt::QueuedConnection);

    	loop.quit();
    });

    QTimer::singleShot(1000, &loop, &QEventLoop::quit);

    loop.exec();
}

void SplashScreenWidget::showAnimated()
{
#ifdef SPLASH_SCREEN_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QWidget::show();

    QEventLoop eventLoop;

    auto windowOpacityAnimation = new QPropertyAnimation(this, "windowOpacity");

    windowOpacityAnimation->setEasingCurve(QEasingCurve::InQuad);
    windowOpacityAnimation->setDuration(SplashScreenWidget::animationDuration);
    windowOpacityAnimation->setStartValue(0);
    windowOpacityAnimation->setEndValue(1);

    auto positionAnimation = new QPropertyAnimation(this, "pos");

    const auto mainScreen           = QApplication::screens().at(0);
    const auto availableGeometry    = mainScreen->availableGeometry();
    const auto position             = availableGeometry.topLeft() + availableGeometry.center() - rect().center();

    positionAnimation->setEasingCurve(QEasingCurve::InQuad);
    positionAnimation->setDuration(SplashScreenWidget::animationDuration);
    positionAnimation->setStartValue(position - QPoint(0, SplashScreenWidget::panAmount));
    positionAnimation->setEndValue(position);

    auto animationGroup = new QParallelAnimationGroup(this);

    animationGroup->addAnimation(windowOpacityAnimation);
    animationGroup->addAnimation(positionAnimation);

    connect(animationGroup, &QParallelAnimationGroup::finished, animationGroup, &QParallelAnimationGroup::deleteLater);
    connect(animationGroup, &QParallelAnimationGroup::finished, &eventLoop, &QEventLoop::quit);

    animationGroup->start();
    
    while(animationGroup->state() == QAbstractAnimation::Running) {
      QApplication::processEvents();
    }
}

void SplashScreenWidget::closeAnimated()
{
#ifdef SPLASH_SCREEN_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QEventLoop eventLoop;

    auto windowOpacityAnimation = new QPropertyAnimation(this, "windowOpacity");

    windowOpacityAnimation->setEasingCurve(QEasingCurve::InQuad);
    windowOpacityAnimation->setDuration(SplashScreenWidget::animationDuration);
    windowOpacityAnimation->setStartValue(1);
    windowOpacityAnimation->setEndValue(0);
    
    auto positionAnimation = new QPropertyAnimation(this, "pos");

    const auto mainScreen           = QApplication::screens().at(0);
    const auto availableGeometry    = mainScreen->availableGeometry();
    const auto position             = availableGeometry.topLeft() + availableGeometry.center() - rect().center();

    positionAnimation->setEasingCurve(QEasingCurve::InQuad);
    positionAnimation->setDuration(SplashScreenWidget::animationDuration);
    positionAnimation->setStartValue(position);
    positionAnimation->setEndValue(position + QPoint(0, SplashScreenWidget::panAmount));

    auto animationGroup = new QParallelAnimationGroup(this);

    animationGroup->addAnimation(windowOpacityAnimation);
    animationGroup->addAnimation(positionAnimation);

    connect(animationGroup, &QPropertyAnimation::finished, this, &QDialog::close);
    connect(animationGroup, &QParallelAnimationGroup::finished, animationGroup, &QParallelAnimationGroup::deleteLater);
    connect(animationGroup, &QParallelAnimationGroup::finished, &eventLoop, &QEventLoop::quit);

    animationGroup->start();

    eventLoop.exec();

    deleteLater();
}

void SplashScreenWidget::createBody()
{
    //if (shouldDisplayProjectInfo()) {
    //    auto projectMetaAction = _splashScreenAction.getProjectMetaAction();
    //    //auto& splashScreenAction = _splashScreenAction.getProjectMetaAction()->getSplashScreenAction();

    //    const auto projectLogoPixmap = QPixmap::fromImage(_splashScreenAction.getProjectImageAction().getImage());

    //    if (!projectLogoPixmap.isNull())
    //        projectLogoLabel->setPixmap(projectLogoPixmap.scaledToHeight(SplashScreenWidget::logoSize, Qt::SmoothTransformation));

    //    auto& versionAction = projectMetaAction->getProjectVersionAction();
    //    auto title          = projectMetaAction->getTitleAction().getString();
    //    auto version        = QString("%1.%2.%3").arg(QString::number(versionAction.getMajorAction().getValue()), QString::number(versionAction.getMinorAction().getValue()), QString::number(versionAction.getPatchAction().getValue()));
    //    if(!versionAction.getSuffixAction().getString().isEmpty())
    //        version.append("-" + versionAction.getSuffixAction().getString());
    //    auto description    = projectMetaAction->getDescriptionAction().getString();
    //    auto comments       = projectMetaAction->getCommentsAction().getString();

    //    if (title.isEmpty())
    //        title = projects().hasProject() ? QFileInfo(projects().getCurrentProject()->getFilePath()).fileName() : "Untitled";

    //    htmlLabel->setText(QString(" \
    //        <div style='color: %5;'> \
    //            <p style='font-size: 20pt; font-weight: bold; color: rgb(25, 25, 25);'><span>%1</span></p> \
    //            <p style='font-weight: bold;'>Version: %2</p> \
    //            <p>%3</p> \
    //            <p>%4</p> \
    //        </div> \
    //    ").arg(title, version, description, comments, bodyColor));
    //}
    //else {
    //    projectLogoLabel->setPixmap(_logoImage);
    //    projectLogoLabel->setToolTip(SplashScreenWidget::getCopyrightNoticeTooltip());

    //    const auto applicationVersion   = Application::current()->getVersion();
    //    auto versionString              = QString("%1.%2.%3").arg(QString::number(applicationVersion.getMajor()), QString::number(applicationVersion.getMinor()), QString::number(applicationVersion.getPatch()));

    //    if(applicationVersion.getSuffix().size() > 0)
    //        versionString.append("-" + applicationVersion.getSuffix());

    //    htmlLabel->setText(QString(" \
    //        <div> \
    //            <p style='font-size: 20pt; font-weight: bold;'><span style='color: rgb(102, 159, 178)'>ManiVault</span> <span style='color: rgb(162, 141, 208)'>Studio<sup style='font-size: 12pt; font-weight: bold;'>&copy;</sup></span></p> \
    //            <p style='font-weight: bold; color: %2;'>Version: %1</p> \
    //            <p style='color: %2;'><i>An extensible open-source visual analytics framework for analyzing high-dimensional data</i></p> \
    //        </div> \
    //    ").arg(versionString, bodyColor));

    //    htmlLabel->setToolTip(SplashScreenWidget::getCopyrightNoticeTooltip());

    //    rightColumn->addStretch(1);

    //}

    //leftColumn->addStretch(1);
    //rightColumn->addStretch(1);

    //if (!_splashScreenAction.getAlerts().isEmpty()) {
    //    for (const auto& alert : _splashScreenAction.getAlerts()) {
    //        if (alert.getType() != SplashScreenAction::Alert::Type::Debug) {
    //            const auto rowCount = bodyLayout->rowCount();

    //            bodyLayout->addWidget(alert.getIconLabel(this), rowCount, 1);
    //            bodyLayout->addWidget(alert.getMessageLabel(this), rowCount, 2);
    //        }
    //    }
    //}
}

void SplashScreenWidget::createFooter()
{
    //if (shouldDisplayProjectInfo()) {
    //    auto imagesLayout = new QHBoxLayout();

    //    imagesLayout->setContentsMargins(SplashScreenWidget::margin, SplashScreenWidget::margin / 2, SplashScreenWidget::margin, 30);

    //    auto projectMetaAction = _splashScreenAction.getProjectMetaAction();

    //    auto& splashScreenAction = projectMetaAction->getSplashScreenAction();

    //    auto affiliateLogosImageLabel = new QLabel();

    //    const auto affiliatesLogosPixmap = QPixmap::fromImage(splashScreenAction.getAffiliateLogosImageAction().getImage());

    //    if (!affiliatesLogosPixmap.isNull())
    //        affiliateLogosImageLabel->setPixmap(affiliatesLogosPixmap.scaledToHeight(SplashScreenWidget::footerImagesHeight, Qt::SmoothTransformation));

    //    auto builtWithWidget        = new QWidget();
    //    auto builtWithWidgetLayout  = new QHBoxLayout();
    //    
    //    auto logoLabel      = new QLabel();
    //    auto builtWithLabel = new QLabel();

    //    logoLabel->setFixedSize(SplashScreenWidget::footerImagesHeight, SplashScreenWidget::footerImagesHeight);
    //    logoLabel->setScaledContents(true);
    //    logoLabel->setPixmap(_logoImage);

    //    const auto& applicationVersionAction = projectMetaAction->getApplicationVersionAction();

    //    builtWithLabel->setTextFormat(Qt::RichText);
    //    builtWithLabel->setText(QString(" \
    //        <div style='margin-left: 10px; font-family: --bs-font-sans-serif'> \
    //            <span style='font-size: 10pt; font-weight: bold; color: rgb(162, 141, 208);'>Built with</span> \
    //            <br> \
    //            <span style='font-size: 12pt; font-weight: bold; color: rgb(102, 159, 178); padding: 0px;'>ManiVault Studio v%1<sup>&copy;</sup></span> \
    //        </div> \
    //    ").arg(applicationVersionAction.getVersionStringAction().getString()));

    //    builtWithWidgetLayout->setContentsMargins(0, 0, 0, 0);
    //    builtWithWidgetLayout->addWidget(logoLabel);
    //    builtWithWidgetLayout->addWidget(builtWithLabel);

    //    builtWithWidget->setLayout(builtWithWidgetLayout);
    //    builtWithWidget->setToolTip(SplashScreenWidget::getCopyrightNoticeTooltip());

    //    imagesLayout->addWidget(affiliateLogosImageLabel);
    //    imagesLayout->addStretch(1);
    //    imagesLayout->addWidget(builtWithWidget);

    //    _roundedFrameLayout.addLayout(imagesLayout);
    //}

    if (Application::current()->getStartupTask().getEnabled()) {
        auto progressWidget         = new QWidget();
        auto progressWidgetLayout   = new QHBoxLayout();
        auto taskActionLabelWidget  = new ElidedLabel("", this);
        auto taskActionWidget       = _splashScreenAction.getTaskAction().createWidget(this);

        taskActionLabelWidget->setStyleSheet("color: rgb(170, 170, 170); padding-bottom: 2px;");

        connect(_splashScreenAction.getTaskAction().getTask(), &Task::progressDescriptionChanged, taskActionLabelWidget, &ElidedLabel::setText);

        taskActionWidget->setFixedSize(200, 7);

        const auto borderRadius = 2;

        const QString progressBarStyleSheet(QString("\
            QProgressBar { \
                background-color: qlineargradient( x1: 0 y1: 0, x2: 0 y2: 1, stop: 0 rgba(0, 0, 0, 150), stop: 0.6 rgba(20, 20, 20, 25), stop: 1 rgba(20, 20, 20, 25)); \
                font-size: 1pt; \
                color: rgba(20, 20, 20, 50); \
                border: 1px solid rgba(0, 0, 0, 100); \
                border-radius: %1px; \
            } \
            QProgressBar::chunk { \
                background-color: qlineargradient( \
                    x1: 0 y1: 0, \
                    x2: 0 y2: 1, \
                    stop: 0 rgba(255, 255, 255, 60), \
                    stop: 0.5 rgba(255, 255, 255, 200), \
                    stop: 1 rgba(255, 255, 255, 140) \
                ); \
                border-radius: %1px; \
            } \
        ").arg(borderRadius));

        (taskActionWidget->findChild<QProgressBar*>("ProgressBar"))->setStyleSheet(progressBarStyleSheet);

        progressWidgetLayout->setContentsMargins(15, 9, 15, 9);
        progressWidgetLayout->setSpacing(15);
        progressWidgetLayout->addWidget(taskActionLabelWidget, 1);
        progressWidgetLayout->addWidget(taskActionWidget);

        progressWidget->setAutoFillBackground(true);
        progressWidget->setObjectName("ProgressWidget");
        progressWidget->setStyleSheet(QString("QWidget#ProgressWidget { \
            background-color: rgb(50, 50, 50); \
            border-bottom-left-radius: %1px %1px; \
            border-bottom-right-radius: %1px %1px; \
        }").arg(SplashScreenWidget::frameRadius));

        progressWidget->setLayout(progressWidgetLayout);

        _roundedFrameLayout.addWidget(progressWidget);
    }
}

QString SplashScreenWidget::getCopyrightNoticeTooltip()
{
    return "This software is licensed under the GNU Lesser General Public License v3.0.<br>Copyright (c) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)";
}

SplashScreenWidget::ExternalLinkWidget::ExternalLinkWidget(const QString& fontAwesomeIconName, const QString& text, const QUrl& externalLink, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _externalLink(externalLink)
{
    auto layout     = new QHBoxLayout();
    auto iconLabel  = new QLabel(StyledIcon::getIconCharacter(fontAwesomeIconName));
    auto textLabel = new QLabel(QString("<a href='%1' style='color: gray'>%2</a>").arg(externalLink.toString(), text));

    iconLabel->setFont(StyledIcon::getIconFont());
    iconLabel->setAlignment(Qt::AlignCenter);

    textLabel->setTextFormat(Qt::RichText);
    textLabel->setOpenExternalLinks(true);

    layout->setContentsMargins(2, 0, 2, 0);

    layout->addWidget(iconLabel);
    layout->addWidget(textLabel, 1);

    setLayout(layout);

    updateStyle();
}

void SplashScreenWidget::ExternalLinkWidget::enterEvent(QEnterEvent* event)
{
    QWidget::enterEvent(event);

    updateStyle();
}

void SplashScreenWidget::ExternalLinkWidget::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);

    updateStyle();
}

void SplashScreenWidget::ExternalLinkWidget::updateStyle()
{
    const auto shouldHighlight = underMouse();

    setStyleSheet(QString("color: %1").arg(shouldHighlight ? "black" : "gray"));
}

}
