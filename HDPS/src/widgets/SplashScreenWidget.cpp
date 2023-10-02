// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright &copy; 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SplashScreenWidget.h"
#include "ProjectMetaAction.h"
#include "CoreInterface.h"
#include "Application.h"

#include "actions/SplashScreenAction.h"

#include "widgets/ElidedLabel.h"

#include <QScreen>
#include <QPainter>
#include <QPainterPath>
#include <QEventLoop>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

#ifdef _DEBUG
    #define SPLASH_SCREEN_WIDGET_VERBOSE
#endif

namespace hdps::gui {

SplashScreenWidget::SplashScreenWidget(SplashScreenAction& splashScreenAction, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _splashScreenAction(splashScreenAction),
    _logoImage(":/Icons/AppIcon256"),
    _backgroundImage(":/Images/SplashScreenBackground"),
    _closeToolButton(this)
{
    setObjectName("SplashScreenWidget");
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window | Qt::WindowStaysOnTopHint);
    setFixedSize(QSize(SplashScreenWidget::fixedWidth, SplashScreenWidget::fixedHeight));
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(0.0);

    _backgroundImage = _backgroundImage.scaled(_backgroundImage.size() / 5, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    createHeader();
    createBody();
    createFooter();

    auto mainLayout = new QVBoxLayout();

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(&_roundedFrame);

    _frameLayout.setContentsMargins(0, 0, 0, 0);

    _roundedFrame.setObjectName("RoundedFrame");
    _roundedFrame.setContentsMargins(0, 0, 0, 0);
    _roundedFrame.setLayout(&_frameLayout);

    setLayout(mainLayout);

    _roundedFrame.setStyleSheet(QString(" \
        QFrame#RoundedFrame { \
            background-color: transparent; \
            border-radius: %1px; \
        } \
    ").arg(SplashScreenWidget::frameRadius));

    _closeToolButton.raise();
}

void SplashScreenWidget::paintEvent(QPaintEvent* paintEvent)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    auto backgroundRect = rect();

    if (Application::current()->getTask(Application::TaskType::LoadApplication)->isRunning())
        backgroundRect.setHeight(height() - SplashScreenWidget::frameRadius);

    QPainterPath path;

    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(backgroundRect, SplashScreenWidget::frameRadius, SplashScreenWidget::frameRadius);

    if (Application::current()->getTask(Application::TaskType::LoadApplication)->isRunning()) {
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

    QLinearGradient overlayGradient(0, 0, 0, height());

    overlayGradient.setColorAt(0.0, QColor(255, 255, 255, 0));
    //overlayGradient.setColorAt(0.5, QColor(255, 255, 255, 255));
    overlayGradient.setColorAt(1.0, QColor(255, 255, 255, 255));

    painter.setOpacity(1.0);
    painter.setBrush(overlayGradient);
    painter.drawPath(path);
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

    eventLoop.exec();
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

void SplashScreenWidget::createHeader()
{
    const auto margin       = 10;
    const auto fixedSize    = 25;

    _closeToolButton.setFixedSize(fixedSize, fixedSize);
    _closeToolButton.setVisible(_splashScreenAction.getMayClose());
    _closeToolButton.setIcon(Application::getIconFont("FontAwesome").getIcon("times"));
    _closeToolButton.setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
    _closeToolButton.setAutoRaise(true);
    _closeToolButton.setToolTip("Close the splash screen");
    _closeToolButton.move(width() - fixedSize - margin, margin);

    connect(&_closeToolButton, &QToolButton::clicked, this, &SplashScreenWidget::closeAnimated);
}

void SplashScreenWidget::createBody()
{
    auto columnsLayout  = new QHBoxLayout();
    auto leftColumn     = new QVBoxLayout();
    auto rightColumn    = new QVBoxLayout();

    columnsLayout->setContentsMargins(SplashScreenWidget::margin, SplashScreenWidget::margin, SplashScreenWidget::margin, SplashScreenWidget::margin);
    columnsLayout->setSpacing(50);
    columnsLayout->setAlignment(Qt::AlignTop);
    columnsLayout->addLayout(leftColumn);
    columnsLayout->addLayout(rightColumn);

    leftColumn->setAlignment(Qt::AlignTop);
    rightColumn->setAlignment(Qt::AlignTop);

    auto imageLabel = new QLabel();
    auto htmlLabel  = new QLabel();

    imageLabel->setScaledContents(true);
    imageLabel->setFixedSize(SplashScreenWidget::logoSize, SplashScreenWidget::logoSize);

    htmlLabel->setWordWrap(true);
    htmlLabel->setTextFormat(Qt::RichText);
    htmlLabel->setOpenExternalLinks(true);

    leftColumn->addWidget(imageLabel);
    rightColumn->addWidget(htmlLabel);

    const auto bodyColor = "rgb(50, 50, 50)";

    if (shouldDisplayProjectInfo()) {
        auto projectMetaAction = _splashScreenAction.getProjectMetaAction();
        auto& splashScreenAction = _splashScreenAction.getProjectMetaAction()->getSplashScreenAction();

        imageLabel->setPixmap(QPixmap::fromImage(splashScreenAction.getProjectImageAction().getImage().scaledToHeight(SplashScreenWidget::logoSize, Qt::SmoothTransformation)));

        auto& versionAction = projectMetaAction->getProjectVersionAction();
        auto title          = projectMetaAction->getTitleAction().getString();
        auto version        = QString("%1.%2 <i>%3</i>").arg(QString::number(versionAction.getMajorAction().getValue()), QString::number(versionAction.getMinorAction().getValue()), versionAction.getSuffixAction().getString().toLower());
        auto description    = projectMetaAction->getDescriptionAction().getString();
        auto comments       = projectMetaAction->getCommentsAction().getString();

        if (title.isEmpty())
            title = QFileInfo(projects().getCurrentProject()->getFilePath()).fileName();

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
        imageLabel->setPixmap(_logoImage);
        imageLabel->setToolTip(SplashScreenWidget::getCopyrightTooltip());

        const auto applicationVersion   = Application::current()->getVersion();
        const auto versionString        = QString("%1.%2").arg(QString::number(applicationVersion.getMajor()), QString::number(applicationVersion.getMinor()));

        htmlLabel->setText(QString(" \
            <div> \
                <p style='font-size: 20pt; font-weight: bold;'><span style='color: rgb(102, 159, 178)'>ManiVault</span> <span style='color: rgb(162, 141, 208)'>Studio<sup>&copy;</sup></span></p> \
                <p style='font-weight: bold; color: %5;'>Version: %2</p> \
                <p style='color: %2;'><i>An extensible open-source visual analytics framework for analyzing high-dimensional data</i></p> \
            </div> \
        ").arg(versionString, bodyColor));
        htmlLabel->setToolTip(SplashScreenWidget::getCopyrightTooltip());
    }

    rightColumn->addStretch(1);

    _frameLayout.addLayout(columnsLayout, 1);
}

void SplashScreenWidget::createFooter()
{
    if (shouldDisplayProjectInfo()) {
        auto imagesLayout = new QHBoxLayout();

        imagesLayout->setContentsMargins(SplashScreenWidget::margin, SplashScreenWidget::margin / 2, SplashScreenWidget::margin, SplashScreenWidget::margin / 2);

        auto& splashScreenAction = _splashScreenAction.getProjectMetaAction()->getSplashScreenAction();

        auto affiliateLogosImageLabel = new QLabel();

        affiliateLogosImageLabel->setPixmap(QPixmap::fromImage(splashScreenAction.getAffiliateLogosImageAction().getImage().scaledToHeight(SplashScreenWidget::footerImagesHeight, Qt::SmoothTransformation)));

        auto builtWithWidget        = new QWidget();
        auto builtWithWidgetLayout  = new QHBoxLayout();
        
        auto logoLabel      = new QLabel();
        auto builtWithLabel = new QLabel();

        logoLabel->setFixedSize(SplashScreenWidget::footerImagesHeight, SplashScreenWidget::footerImagesHeight);
        logoLabel->setScaledContents(true);
        logoLabel->setPixmap(_logoImage);

        builtWithLabel->setTextFormat(Qt::RichText);
        builtWithLabel->setText(QString(" \
            <div style='margin-left: 10px; font-family: --bs-font-sans-serif'> \
                <span style='font-size: 10pt; font-weight: bold; color: rgb(162, 141, 208);'>Built with</span> \
                <br> \
                <span style='font-size: 12pt; font-weight: bold; color: rgb(102, 159, 178); padding: 0px;'>ManiVault Studio<sup>&copy;</sup></span> \
            </div> \
        "));

        builtWithWidgetLayout->setContentsMargins(0, 0, 0, 0);
        builtWithWidgetLayout->addWidget(logoLabel);
        builtWithWidgetLayout->addWidget(builtWithLabel);

        builtWithWidget->setLayout(builtWithWidgetLayout);
        builtWithWidget->setToolTip(SplashScreenWidget::getCopyrightTooltip());

        imagesLayout->addWidget(affiliateLogosImageLabel);
        imagesLayout->addStretch(1);
        imagesLayout->addWidget(builtWithWidget);

        _frameLayout.addLayout(imagesLayout);
    }

    if (Application::current()->getTask(Application::TaskType::LoadApplication)->isRunning()) {
        auto progressWidget         = new QWidget();
        auto progressWidgetLayout   = new QHBoxLayout();
        auto taskActionLabelWidget  = new ElidedLabel("", this);
        auto taskActionWidget       = _splashScreenAction.getTaskAction().createWidget(this);

        taskActionLabelWidget->setStyleSheet("color: rgb(170, 170, 170); padding-left: 5px; padding-right: 5px; padding-bottom: 2px;");
        
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

        _frameLayout.addWidget(progressWidget);
    }
}

bool SplashScreenWidget::shouldDisplayProjectInfo() const
{
    return _splashScreenAction.getEnabledAction().isChecked() && _splashScreenAction.getProjectMetaAction();
}

QString SplashScreenWidget::getCopyrightTooltip()
{
    return "<html>Copyright &copy; 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)</html>";
}

}
