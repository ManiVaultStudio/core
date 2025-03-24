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

SplashScreenWidget::SplashScreenWidget(SplashScreenAction& splashScreenAction, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _splashScreenAction(splashScreenAction),
    _logoImage(":/Icons/AppIcon256"),
    _backgroundImage(":/Images/SplashScreenBackground"),
    _closeToolButton(&_roundedFrame)
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
    
    setGraphicsEffect(&_dropShadowEffect);

    if (!_backgroundImage.isNull())
        _backgroundImage = _backgroundImage.scaled(_backgroundImage.size() / 5, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    createToolbar();
    createBody();
    createFooter();

    auto mainLayout = new QVBoxLayout();

    mainLayout->setContentsMargins(SplashScreenWidget::shadowMargin, SplashScreenWidget::shadowMargin, SplashScreenWidget::shadowMargin, SplashScreenWidget::shadowMargin);
    mainLayout->addWidget(&_roundedFrame);

    _roundedFrameLayout.setContentsMargins(0, 0, 0, 0);

    _roundedFrame.setObjectName("RoundedFrame");
    _roundedFrame.setContentsMargins(0, 0, 0, 0);
    _roundedFrame.setLayout(&_roundedFrameLayout);

    setLayout(mainLayout);

    _roundedFrame.setStyleSheet(QString(" \
        QFrame#RoundedFrame { \
            background-color: transparent; \
            border-radius: %1px; \
        } \
    ").arg(SplashScreenWidget::frameRadius));

    _closeToolButton.raise();

    connect(&_processEventsTimer, &QTimer::timeout, this, []() -> void {
        QCoreApplication::processEvents();
    });

    _processEventsTimer.start(10);
}

SplashScreenWidget::~SplashScreenWidget()
{
    _processEventsTimer.stop();
}

void SplashScreenWidget::paintEvent(QPaintEvent* paintEvent)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    auto backgroundRect = rect().marginsRemoved(QMargins(SplashScreenWidget::shadowMargin, SplashScreenWidget::shadowMargin, SplashScreenWidget::shadowMargin, SplashScreenWidget::shadowMargin));

    if (Application::current()->getStartupTask().isRunning())
        backgroundRect.setHeight(height() - SplashScreenWidget::frameRadius - (2 * SplashScreenWidget::shadowMargin));

    QPainterPath path;

    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(backgroundRect, SplashScreenWidget::frameRadius, SplashScreenWidget::frameRadius);

    if (Application::current()->getStartupTask().isRunning()) {
        qreal squareSize = backgroundRect.height() / 2;

        path.addRect(QRect(backgroundRect.left(), backgroundRect.top() + backgroundRect.height() - squareSize, squareSize, squareSize));
        path.addRect(QRect((backgroundRect.left() + backgroundRect.width()) - squareSize, backgroundRect.top() + backgroundRect.height() - squareSize, squareSize, squareSize));

        path = path.simplified();
    }

    auto centerOfWidget     = rect().center();
    auto pixmapRectangle    = _backgroundImage.rect();

    pixmapRectangle.moveCenter(centerOfWidget);

    const QBrush backgroundImageBrush(_backgroundImage);

    painter.setPen(QPen(Qt::transparent, 1));
    painter.setBrush(backgroundImageBrush);
    painter.drawPath(path);

    QLinearGradient overlayGradient(0, 0, 0, height() - (2 * SplashScreenWidget::shadowMargin));

    overlayGradient.setColorAt(0.0, QColor(255, 255, 255, 0));
    overlayGradient.setColorAt(1.0, QColor(255, 255, 255, 255));

    painter.setOpacity(1.0);
    painter.setBrush(overlayGradient);
    painter.drawPath(path);

    _closeToolButton.raise();
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

void SplashScreenWidget::createToolbar()
{
    const auto margin       = 10;
    const auto fixedSize    = 25;

    _closeToolButton.setFixedSize(fixedSize, fixedSize);
    _closeToolButton.setVisible(_splashScreenAction.getMayCloseSplashScreenWidget());
    _closeToolButton.setIcon(StyledIcon("xmark"));
    _closeToolButton.setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
    _closeToolButton.setAutoRaise(true);
    _closeToolButton.setToolTip("Close the splash screen");
    _closeToolButton.setIconSize(QSize(20, 20));
    _closeToolButton.move(width() - (2 * SplashScreenWidget::shadowMargin) - fixedSize - margin, margin);

    connect(&_closeToolButton, &QToolButton::clicked, this, &SplashScreenWidget::closeAnimated);
}

void SplashScreenWidget::createBody()
{
    auto bodyLayout     = new QGridLayout();
    auto leftColumn     = new QVBoxLayout();
    auto middleColumn   = new QVBoxLayout();
    auto rightColumn    = new QVBoxLayout();

    bodyLayout->setContentsMargins(SplashScreenWidget::margin, SplashScreenWidget::margin, SplashScreenWidget::margin, shouldDisplayProjectInfo() ? SplashScreenWidget::margin : SplashScreenWidget::margin / 2);
    bodyLayout->setSpacing(10);
    bodyLayout->setAlignment(Qt::AlignTop);
    bodyLayout->setColumnStretch(2, 1);
    bodyLayout->setColumnMinimumWidth(1, 25);

    bodyLayout->addLayout(leftColumn, 0, 0);
    bodyLayout->addLayout(middleColumn, 0, 1);
    bodyLayout->addLayout(rightColumn, 0, 2);

    bodyLayout->setRowStretch(0, 1);

    leftColumn->setAlignment(Qt::AlignTop);
    rightColumn->setAlignment(Qt::AlignTop);

    auto projectLogoLabel   = new QLabel();
    auto htmlLabel          = new QLabel();

    projectLogoLabel->setScaledContents(true);
    projectLogoLabel->setFixedSize(SplashScreenWidget::logoSize, SplashScreenWidget::logoSize);

    htmlLabel->setWordWrap(true);
    htmlLabel->setTextFormat(Qt::RichText);
    htmlLabel->setOpenExternalLinks(true);

    leftColumn->addWidget(projectLogoLabel);
    rightColumn->addWidget(htmlLabel);

    rightColumn->setSpacing(8);

    const auto bodyColor = "rgb(50, 50, 50)";

    if (shouldDisplayProjectInfo()) {
        auto projectMetaAction = _splashScreenAction.getProjectMetaAction();
        auto& splashScreenAction = _splashScreenAction.getProjectMetaAction()->getSplashScreenAction();

        const auto projectLogoPixmap = QPixmap::fromImage(splashScreenAction.getProjectImageAction().getImage());

        if (!projectLogoPixmap.isNull())
            projectLogoLabel->setPixmap(projectLogoPixmap.scaledToHeight(SplashScreenWidget::logoSize, Qt::SmoothTransformation));

        auto& versionAction = projectMetaAction->getProjectVersionAction();
        auto title          = projectMetaAction->getTitleAction().getString();
        auto version        = QString("%1.%2.%3-%4").arg(QString::number(versionAction.getMajorAction().getValue()), QString::number(versionAction.getMinorAction().getValue()), QString::number(versionAction.getPatchAction().getValue()), versionAction.getSuffixAction().getString().toLower());
        auto description    = projectMetaAction->getDescriptionAction().getString();
        auto comments       = projectMetaAction->getCommentsAction().getString();

        if (title.isEmpty())
            title = projects().hasProject() ? QFileInfo(projects().getCurrentProject()->getFilePath()).fileName() : "Untitled";

        htmlLabel->setText(QString(" \
            <div style='color: %5;'> \
                <p style='font-size: 20pt; font-weight: bold; color: rgb(25, 25, 25);'><span>%1</span></p> \
                <p style='font-weight: bold;'>Version: %2</p> \
                <p>%3</p> \
                <p>%4</p> \
            </div> \
        ").arg(title, version, description, comments, bodyColor));
    }
    else {
        projectLogoLabel->setPixmap(_logoImage);
        projectLogoLabel->setToolTip(SplashScreenWidget::getCopyrightNoticeTooltip());

        const auto applicationVersion   = Application::current()->getVersion();
        const auto versionString        = QString("%1.%2%3").arg(QString::number(MV_VERSION_MAJOR), QString::number(MV_VERSION_MINOR), QString(MV_VERSION_SUFFIX.data()));

        htmlLabel->setText(QString(" \
            <div> \
                <p style='font-size: 20pt; font-weight: bold;'><span style='color: rgb(102, 159, 178)'>ManiVault</span> <span style='color: rgb(162, 141, 208)'>Studio<sup style='font-size: 12pt; font-weight: bold;'>&copy;</sup></span></p> \
                <p style='font-weight: bold; color: %5;'>Version: %2</p> \
                <p style='color: %2;'><i>An extensible open-source visual analytics framework for analyzing high-dimensional data</i></p> \
            </div> \
        ").arg(versionString, bodyColor));

        htmlLabel->setToolTip(SplashScreenWidget::getCopyrightNoticeTooltip());

        rightColumn->addStretch(1);

        rightColumn->addWidget(new ExternalLinkWidget("globe", "Visit our website", QUrl("https://www.manivault.studio/")));
        rightColumn->addWidget(new ExternalLinkWidget("globe", "Contribute to ManiVault on Github", QUrl("https://github.com/ManiVaultStudio")));
        rightColumn->addWidget(new ExternalLinkWidget("globe", "Get in touch on our Discord", QUrl("https://discord.gg/pVxmC2cSzA")));
    }

    leftColumn->addStretch(1);
    rightColumn->addStretch(1);

    if (!_splashScreenAction.getAlerts().isEmpty()) {
        for (const auto& alert : _splashScreenAction.getAlerts()) {
            if (alert.getType() != SplashScreenAction::Alert::Type::Debug) {
                const auto rowCount = bodyLayout->rowCount();

                bodyLayout->addWidget(alert.getIconLabel(this), rowCount, 1);
                bodyLayout->addWidget(alert.getMessageLabel(this), rowCount, 2);
            }
        }
    }

    _roundedFrameLayout.addLayout(bodyLayout, 1);

    if (!shouldDisplayProjectInfo()) {
        auto copyrightNoticeLabel = new QLabel();

        copyrightNoticeLabel->setAlignment(Qt::AlignBottom);
        copyrightNoticeLabel->setWordWrap(true);
        copyrightNoticeLabel->setText("<p style='color: rgba(0, 0, 0, 80); font-size: 7pt;'> \
            This software is licensed under the GNU Lesser General Public License v3.0.<br> \
            Copyright &copy; 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)</p>"
        );

        bodyLayout->addWidget(copyrightNoticeLabel, bodyLayout->rowCount(), 0, 1, 3);
    }
}

void SplashScreenWidget::createFooter()
{
    if (shouldDisplayProjectInfo()) {
        auto imagesLayout = new QHBoxLayout();

        imagesLayout->setContentsMargins(SplashScreenWidget::margin, SplashScreenWidget::margin / 2, SplashScreenWidget::margin, 30);

        auto projectMetaAction = _splashScreenAction.getProjectMetaAction();

        auto& splashScreenAction = projectMetaAction->getSplashScreenAction();

        auto affiliateLogosImageLabel = new QLabel();

        const auto affiliatesLogosPixmap = QPixmap::fromImage(splashScreenAction.getAffiliateLogosImageAction().getImage());

        if (!affiliatesLogosPixmap.isNull())
            affiliateLogosImageLabel->setPixmap(affiliatesLogosPixmap.scaledToHeight(SplashScreenWidget::footerImagesHeight, Qt::SmoothTransformation));

        auto builtWithWidget        = new QWidget();
        auto builtWithWidgetLayout  = new QHBoxLayout();
        
        auto logoLabel      = new QLabel();
        auto builtWithLabel = new QLabel();

        logoLabel->setFixedSize(SplashScreenWidget::footerImagesHeight, SplashScreenWidget::footerImagesHeight);
        logoLabel->setScaledContents(true);
        logoLabel->setPixmap(_logoImage);

        const auto& applicationVersionAction = projectMetaAction->getApplicationVersionAction();

        builtWithLabel->setTextFormat(Qt::RichText);
        builtWithLabel->setText(QString(" \
            <div style='margin-left: 10px; font-family: --bs-font-sans-serif'> \
                <span style='font-size: 10pt; font-weight: bold; color: rgb(162, 141, 208);'>Built with</span> \
                <br> \
                <span style='font-size: 12pt; font-weight: bold; color: rgb(102, 159, 178); padding: 0px;'>ManiVault Studio v%1<sup>&copy;</sup></span> \
            </div> \
        ").arg(applicationVersionAction.getVersionStringAction().getString()));

        builtWithWidgetLayout->setContentsMargins(0, 0, 0, 0);
        builtWithWidgetLayout->addWidget(logoLabel);
        builtWithWidgetLayout->addWidget(builtWithLabel);

        builtWithWidget->setLayout(builtWithWidgetLayout);
        builtWithWidget->setToolTip(SplashScreenWidget::getCopyrightNoticeTooltip());

        imagesLayout->addWidget(affiliateLogosImageLabel);
        imagesLayout->addStretch(1);
        imagesLayout->addWidget(builtWithWidget);

        _roundedFrameLayout.addLayout(imagesLayout);
    }

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

bool SplashScreenWidget::shouldDisplayProjectInfo() const
{
    return _splashScreenAction.getEnabledAction().isChecked() && _splashScreenAction.getProjectMetaAction();
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
