#include "OverlayWidget.h"

#include <QDebug>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QMenu>
#include <QLabel>

#include <stdexcept>

namespace hdps
{

namespace gui
{

//DataHierarchyWidget::NoDataOverlayWidget::NoDataOverlayWidget(QWidget* parent) :
//    QWidget(parent),
//    _opacityEffect(new QGraphicsOpacityEffect(this))
//{
//    setAttribute(Qt::WA_TransparentForMouseEvents);
//    setGraphicsEffect(_opacityEffect);
//
//    _opacityEffect->setOpacity(0.35);
//
//    auto layout             = new QVBoxLayout();
//    auto iconLabel          = new QLabel();
//    auto titleLabel         = new QLabel("No data loaded");
//    auto descriptionLabel   = new QLabel("Right-click to import data");
//
//    const auto& fontAwesome = Application::getIconFont("FontAwesome");
//
//    iconLabel->setAlignment(Qt::AlignCenter);
//    iconLabel->setFont(fontAwesome.getFont(14));
//    iconLabel->setText(fontAwesome.getIconCharacter("database"));
//    iconLabel->setStyleSheet("QLabel { padding: 10px; }");
//
//    titleLabel->setAlignment(Qt::AlignCenter);
//    titleLabel->setStyleSheet("font-weight: bold");
//
//    descriptionLabel->setAlignment(Qt::AlignCenter);
//
//    layout->setContentsMargins(0, 0, 0, 0);
//    layout->setSpacing(0);
//    layout->setAlignment(Qt::AlignCenter);
//
//    layout->addStretch(1);
//    layout->addWidget(iconLabel);
//    layout->addWidget(titleLabel);
//    layout->addWidget(descriptionLabel);
//    layout->addStretch(1);
//
//    setLayout(layout);
//
//    // Install event filter for synchronizing widget size
//    parent->installEventFilter(this);
//
//    setObjectName("NoDataOverlayWidget");
//    setStyleSheet("QWidget#NoDataOverlayWidget > QLabel { color: gray; }");
//}
//
//bool DataHierarchyWidget::NoDataOverlayWidget::eventFilter(QObject* target, QEvent* event)
//{
//    switch (event->type())
//    {
//        // Resize event
//        case QEvent::Resize:
//        {
//            if (dynamic_cast<QWidget*>(target) != parent())
//                break;
//
//            // Set fixed overlay size
//            setFixedSize(static_cast<QResizeEvent*>(event)->size());
//
//            break;
//        }
//
//        default:
//            break;
//    }
//
//    return QWidget::eventFilter(target, event);
//}

}
}
