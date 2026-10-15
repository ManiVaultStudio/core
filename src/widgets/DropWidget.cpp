// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DropWidget.h"
#include "Application.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QLabel>
#include <QMimeData>
#include <QIcon>
#include <QGraphicsOpacityEffect>

using namespace mv::util;

namespace mv::gui
{

DropWidget::DropWidget(QWidget* parent) :
    QWidget(parent),
    _getDropRegionsFunction(),
    _showDropIndicator(true),
    _dropIndicatorWidget(nullptr)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setLayout(layout);

    // Install event filter for synchronizing widget size
    parent->installEventFilter(this);
}

bool DropWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            if (dynamic_cast<QWidget*>(target) != parent())
                break;

            const auto parentWidgetSize = static_cast<QResizeEvent*>(event)->size();

            setFixedSize(parentWidgetSize);

            if (_dropIndicatorWidget)
                _dropIndicatorWidget->setFixedSize(parentWidgetSize);

            break;
        }

        case QEvent::DragEnter:
        {
            if (dynamic_cast<QWidget*>(target) != parent())
                break;

            const auto dragEnterEvent = static_cast<QDragEnterEvent*>(event);

            resetLayout();

            const auto dropRegions = _getDropRegionsFunction(dragEnterEvent->mimeData());

            for (auto dropRegion : dropRegions)
                layout()->addWidget(new DropRegionContainerWidget(dropRegion, this));

            if (!dropRegions.isEmpty())
                dragEnterEvent->acceptProposedAction();
            
            if (_dropIndicatorWidget)
                _dropIndicatorWidget->hide();

            break;
        }

        case QEvent::DragMove:
        {
            if (dynamic_cast<QWidget*>(target) != parent())
                break;

            const auto dragMoveEvent = static_cast<QDragMoveEvent*>(event);

            if (layout()->count() > 1) {
                for (int i = 0; i < layout()->count(); ++i)
                {
                    auto dropRegionContainerWidget = dynamic_cast<DropRegionContainerWidget*>(layout()->itemAt(i)->widget());

                    if (dropRegionContainerWidget)
                        dropRegionContainerWidget->setHighLight(dropRegionContainerWidget->geometry().contains(dragMoveEvent->position().toPoint()));
                }
            }

            break;
        }
        
        case QEvent::DragLeave:
        {
            if (dynamic_cast<QWidget*>(target) != parent())
                break;

            resetLayout();

            if (_dropIndicatorWidget)
                _dropIndicatorWidget->setVisible(_showDropIndicator);

            break;
        }

        case QEvent::Drop:
        {
            if (dynamic_cast<QWidget*>(target) != parent())
                break;

            const auto dropEvent = static_cast<QDropEvent*>(event);

            for (int i = 0; i < layout()->count(); ++i)
            {
                auto dropRegionContainerWidget = dynamic_cast<DropRegionContainerWidget*>(layout()->itemAt(i)->widget());

                if (dropRegionContainerWidget) {
                    if (dropRegionContainerWidget->geometry().contains(dropEvent->position().toPoint()))
                        dropRegionContainerWidget->getDropRegion()->drop();
                }
            }

            resetLayout();

            if (_dropIndicatorWidget)
                _dropIndicatorWidget->setVisible(_showDropIndicator);

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

void DropWidget::initialize(const GetDropRegionsFunction& getDropRegions)
{
    _getDropRegionsFunction = getDropRegions;
}

bool DropWidget::getShowDropIndicator() const
{
    return _showDropIndicator;
}

void DropWidget::setShowDropIndicator(const bool& showDropIndicator)
{
    if (showDropIndicator == _showDropIndicator)
        return;

    _showDropIndicator = showDropIndicator;

    if (_dropIndicatorWidget)
        _dropIndicatorWidget->setVisible(showDropIndicator);
}

void DropWidget::setDropIndicatorWidget(QWidget* dropIndicatorWidget)
{
    Q_ASSERT(dropIndicatorWidget != nullptr);

    _dropIndicatorWidget = dropIndicatorWidget;

    _dropIndicatorWidget->setParent(parentWidget());
}

void DropWidget::resetLayout()
{
    QLayoutItem* child;

    while ((child = layout()->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
}

DropWidget::DropRegionContainerWidget::DropRegionContainerWidget(DropRegion* dropRegion, QWidget* parent) :
    QWidget(parent),
    _dropRegion(dropRegion),
    _widgetFader(this, this)
{
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    dropRegion->setParent(this);

    auto mainLayout = new QVBoxLayout();

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(_dropRegion->getWidget());

    setLayout(mainLayout);

    setHighLight(false);
}

DropWidget::DropRegion* DropWidget::DropRegionContainerWidget::getDropRegion()
{
    return _dropRegion;
}

void DropWidget::DropRegionContainerWidget::setHighLight(const bool& highlight /*= false*/)
{
    _widgetFader.setOpacity(highlight ? 0.8f : 0.4f, 0);
}

DropWidget::DropRegion::DropRegion(QObject* parent, QWidget* widget, const Dropped& dropped /*= Dropped()*/) :
    QObject(parent),
    _widget(widget),
    _dropped(dropped)
{
}

DropWidget::DropRegion::DropRegion(QObject* parent, const QString& title, const QString& description, const QString& iconName /*= "file-import"*/, const bool& dropAllowed /*= true*/, const Dropped& dropped /*= Dropped()*/) :
    QObject(parent),
    _widget(new StandardWidget(nullptr, title, description, iconName, dropAllowed)),
    _dropped(dropped)
{
}

QWidget* DropWidget::DropRegion::getWidget() const
{
    Q_ASSERT(_widget != nullptr);

    return _widget;
}

void DropWidget::DropRegion::drop()
{
    if (_dropped)
        _dropped();
}

DropWidget::DropRegion::StandardWidget::StandardWidget(QWidget* parent, const QString& title, const QString& description, const QString& iconName /*= "file-import"*/, const bool& dropAllowed /*= true*/) :
    QWidget(parent)
{
    auto layout = new QVBoxLayout();

    auto iconLabel          = new QLabel();
    auto titleLabel         = new QLabel(title);
    auto descriptionLabel   = new QLabel(description);

    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setFont(StyledIcon::getIconFont(14));
    iconLabel->setText(StyledIcon::getIconCharacter(dropAllowed ? iconName : "exclamation-circle"));
    iconLabel->setStyleSheet("QLabel { padding: 10px; }");

    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold");

    descriptionLabel->setAlignment(Qt::AlignCenter);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignCenter);

    layout->addStretch(1);
    layout->addWidget(iconLabel);
    layout->addWidget(titleLabel);
    layout->addWidget(descriptionLabel);
    layout->addStretch(1);

    setLayout(layout);

    const auto saturation       = dropAllowed ? 0 : 100;
    const auto backgroundColor  = QString("hsl(0, %1%, 97%)").arg(QString::number(saturation));
    const auto foregroundColor  = QString("hsl(0, %1%, 30%)").arg(QString::number(saturation));
    const auto borderColor      = QString("hsl(0, %1%, 50%)").arg(QString::number(saturation));
    const auto border           = QString("1px solid %1").arg(borderColor);

    setObjectName("StandardWidget");
    setStyleSheet(QString("QWidget#StandardWidget{ background-color: %1; border: %2; } QLabel { color: %3; }").arg(backgroundColor, border, foregroundColor));
}

DropWidget::DropIndicatorWidget::DropIndicatorWidget(QWidget* parent, const QString& title, const QString& description) :
    QWidget(parent),
    _opacityEffect(new QGraphicsOpacityEffect(this))
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setGraphicsEffect(_opacityEffect);

    _opacityEffect->setOpacity(0.35);

    auto layout = new QVBoxLayout();

    auto iconLabel          = new QLabel();
    auto titleLabel         = new QLabel(title);
    auto descriptionLabel   = new QLabel(description);

    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setFont(StyledIcon::getIconFont(14));
    iconLabel->setText(StyledIcon::getIconCharacter("file-import"));
    iconLabel->setStyleSheet("QLabel { padding: 10px; }");

    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold");

    descriptionLabel->setAlignment(Qt::AlignCenter);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignCenter);

    layout->addStretch(1);
    layout->addWidget(iconLabel);
    layout->addWidget(titleLabel);
    layout->addWidget(descriptionLabel);
    layout->addStretch(1);

    setLayout(layout);

    setObjectName("DropIndicatorWidget");
    setStyleSheet("QWidget#DropIndicatorWidget > QLabel { color: gray; } QWidget#DropIndicatorWidget { background-color: hsl(0, 0%, 97%); }");
}

}
