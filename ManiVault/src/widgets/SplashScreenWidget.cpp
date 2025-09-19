// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright &copy; 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SplashScreenWidget.h"
#include "CoreInterface.h"
#include "Application.h"
#include "ManiVaultVersion.h"

#include "actions/SplashScreenAction.h"

#include <QScreen>
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

        QTimer::singleShot(50, [this]() -> void { _webEngineView.update(); });
    });

    connect(&Application::current()->getStartupTask(), &Task::progressDescriptionChanged, this, [this](const QString& progressDescription) -> void {
        if (!Application::current()->getStartupTask().getEnabled())
            return;

        QMetaObject::invokeMethod(&_splashScreenBridge, "setProgressDescription", Qt::QueuedConnection, Q_ARG(QString, progressDescription));

        QTimer::singleShot(50, [this]() -> void { _webEngineView.update(); });
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

QString SplashScreenWidget::getCopyrightNoticeTooltip()
{
    return "This software is licensed under the GNU Lesser General Public License v3.0.<br>Copyright (c) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)";
}

}
