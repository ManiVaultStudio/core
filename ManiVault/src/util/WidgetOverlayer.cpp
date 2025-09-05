// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetOverlayer.h"

#include <QDebug>
#include <QWidget>
#include <QResizeEvent>

#ifdef _DEBUG
    #define WIDGET_OVERLAYER_VERBOSE
#endif

namespace mv::util {

#include <QWidget>

    static void setMouseTrackingRecursive(QWidget* root, bool enabled)
    {
        if (!root) return;
        root->setMouseTracking(enabled);

        // Apply to all descendant QWidgets
        const auto descendants = root->findChildren<QWidget*>(QString(), Qt::FindChildrenRecursively);
        for (QWidget* w : descendants)
            w->setMouseTracking(enabled);
    }

WidgetOverlayer::WidgetOverlayer(QObject* parent, QWidget* sourceWidget, QWidget* targetWidget, float initialOpacity /*= 1.0f*/) :
    QObject(parent),
    _sourceWidget(sourceWidget)
{
    setObjectName("WidgetOverlayer");

    Q_ASSERT(_sourceWidget);

    if (!sourceWidget)
        return;

    _sourceWidget->installEventFilter(this);
    setMouseTrackingRecursive(_sourceWidget, true);

    //_sourceWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    setTargetWidget(targetWidget);
}

QWidget* WidgetOverlayer::getSourceWidget() const
{
    return _sourceWidget;
}

QWidget* WidgetOverlayer::getTargetWidget() const
{
    return _targetWidget;
}

void WidgetOverlayer::setTargetWidget(QWidget* targetWidget)
{
    Q_ASSERT(targetWidget);

    if (!targetWidget)
        return;

    //if (targetWidget == _targetWidget)
    //    return;

    auto previousTargetWidget = _targetWidget;

    if (_targetWidget)
        _targetWidget->removeEventFilter(this);

    _targetWidget = targetWidget;

    setMouseTrackingRecursive(_targetWidget, true);
    _targetWidget->installEventFilter(this);

    synchronizeGeometry();

    emit targetWidgetChanged(previousTargetWidget, _targetWidget);
}

void WidgetOverlayer::addMouseEventReceiverWidget(const QWidget* mouseEventReceiverWidget)
{
    return;
    Q_ASSERT(mouseEventReceiverWidget);

    if (!mouseEventReceiverWidget)
        return;

    if (!_mouseEventReceiverWidgets.contains(mouseEventReceiverWidget))
		_mouseEventReceiverWidgets << mouseEventReceiverWidget;

    emit mouseEventReceiverWidgetAdded(mouseEventReceiverWidget);

    _sourceWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
}

void WidgetOverlayer::removeMouseEventReceiverWidget(const QWidget* mouseEventReceiverWidget)
{
    return;
    Q_ASSERT(mouseEventReceiverWidget);

    if (!mouseEventReceiverWidget)
        return;

    _mouseEventReceiverWidgets.removeOne(mouseEventReceiverWidget);

    emit mouseEventReceiverWidgetRemoved(mouseEventReceiverWidget);

    _sourceWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

WidgetOverlayer::MouseEventReceiverWidgets WidgetOverlayer::getMouseEventReceiverWidgets()
{
    return _mouseEventReceiverWidgets;
}

bool WidgetOverlayer::shouldReceiveMouseEvents() const
{
    for (const auto mouseEventReceiverWidget : _mouseEventReceiverWidgets)
        if (mouseEventReceiverWidget->underMouse())
            return true;

    return false;
}

class TransparentAttrGuard {
public:
    explicit TransparentAttrGuard(QWidget* root) {
        // include root and all descendants
        auto all = root->findChildren<QWidget*>(QString(), Qt::FindChildrenRecursively);
        all.push_back(root);
        for (QWidget* w : all) {
            if (w->testAttribute(Qt::WA_TransparentForMouseEvents)) {
                touched_.push_back(w);
                w->setAttribute(Qt::WA_TransparentForMouseEvents, false);
            }
        }
    }
    ~TransparentAttrGuard() {
        for (QWidget* w : touched_)
            if (w) w->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    }
private:
    QVector<QPointer<QWidget>> touched_;
};

QWidget* childAtIncludingTransparent(QWidget* parent, const QPoint& posInParent)
{
    TransparentAttrGuard guard(parent);
    return parent->childAt(posInParent);          // now includes “transparent” widgets
}

QWidget* topChildAt(QWidget* parent, const QPoint& pLocal)
{
    QWidget* best = nullptr;
    // Direct children only; recurse if you want the deepest widget
    const auto children = parent->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
    // You'd need correct Z-order here; QWidget doesn't expose it publicly,
    // so this approach is only safe if you control stacking or don't care.
    for (QWidget* c : children) {
        if (!c->isVisible()) continue;
        QPoint q = c->mapFromParent(pLocal);
        if (!c->rect().contains(q)) continue;
        if (!c->mask().isEmpty() && !c->mask().contains(q)) continue; // respect masks
        // Do NOT check WA_TransparentForMouseEvents — that's the point
        best = c; // pick the topmost; without Z-order info, this is heuristic
    }
    return best;
}

#include <QWidget>
#include <QPointer>
#include <QVector>
#include <QApplication>
#include <QAbstractItemView>
#include <QAbstractScrollArea>

class TransparentAttrScope {
public:
    explicit TransparentAttrScope(QWidget* root) {
        if (!root) return;
        auto all = root->findChildren<QWidget*>(QString(), Qt::FindChildrenRecursively);
        all.push_back(root);
        for (QWidget* w : all) {
            if (w->testAttribute(Qt::WA_TransparentForMouseEvents)) {
                changed_.push_back(w);
                w->setAttribute(Qt::WA_TransparentForMouseEvents, false);
            }
        }
    }
    ~TransparentAttrScope() {
        for (QWidget* w : changed_)
            if (w) w->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    }
private:
    QVector<QPointer<QWidget>> changed_;
};

static QWidget* childAtExcludingEmptyItemViews(QWidget* parent, const QPoint& posInParent)
{
    if (!parent) return nullptr;

    TransparentAttrScope includeTransparent(parent); // make transparent widgets hittable

    QVector<QPointer<QWidget>> temporarilySkipped;   // widgets we’ll skip to see what's underneath
    auto skipOnce = [&](QWidget* w) {
        if (!w) return;
        temporarilySkipped.push_back(w);
        w->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        };

    // Try multiple times, each time skipping the current candidate if it fails our criteria.
    for (int guard = 0; guard < 64; ++guard) { // safety cap
        QWidget* cand = parent->childAt(posInParent);
        if (!cand) break;

        // If it's an item view, only accept if we're over an actual item.
        if (auto view = qobject_cast<QAbstractItemView*>(cand)) {
            const QPoint globalPos = parent->mapToGlobal(posInParent);
            const QPoint inViewport = view->viewport()->mapFromGlobal(globalPos);

            // If we're not inside the viewport, it's probably frame/scrollbar/header – skip it.
            if (!view->viewport()->rect().contains(inViewport)) {
                skipOnce(cand);
                continue;
            }

            // Empty area? (no item under the point) => skip and look through it.
            if (!view->indexAt(inViewport).isValid()) {
                skipOnce(cand);
                continue;
            }

            // Otherwise, it's a real hit on an item.
            return cand;
        }

        // For generic scroll areas, insist the point is within the viewport region.
        if (auto sa = qobject_cast<QAbstractScrollArea*>(cand)) {
            const QPoint globalPos = parent->mapToGlobal(posInParent);
            const QPoint inViewport = sa->viewport()->mapFromGlobal(globalPos);
            if (!sa->viewport()->rect().contains(inViewport)) {
                skipOnce(cand);
                continue;
            }
            return cand;
        }

        // Normal widget – accept.
        return cand;
    }

    // Restore any widgets we made transparent
    for (QWidget* w : temporarilySkipped)
        if (w) w->setAttribute(Qt::WA_TransparentForMouseEvents, false);

    return nullptr;
}


bool WidgetOverlayer::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            if (dynamic_cast<QWidget*>(target) == _targetWidget)
                synchronizeGeometry();

            break;
        }

        case QEvent::Show:
        {
            if (dynamic_cast<QWidget*>(target) == _sourceWidget)
                _sourceWidget->raise();

            break;
        }

        case QEvent::ChildAdded:
        {
            if (dynamic_cast<QWidget*>(target) == _targetWidget) {

                auto ce = static_cast<QChildEvent*>(event);

                if (auto obj = ce->child()) {
                    if (auto w = qobject_cast<QWidget*>(obj)) {
                        if (w == _targetWidget || _targetWidget->isAncestorOf(w)) {
                            // Defer a tick to catch grandchildren created during construction
                            QTimer::singleShot(0, w, [this, w] { enableTrackingRecursive(w); });
                        }
                    }
                }
            }
                

            break;
        }

        case QEvent::MouseMove:
		case QEvent::HoverMove:
        {
            if (auto widget = dynamic_cast<QWidget*>(target)) {

                //if (widget == _sourceWidget)
                //    qDebug() << "WidgetOverlayer::eventFilter: source move";

                //if (widget != _sourceWidget && _sourceWidget->isAncestorOf(widget))
                //    qDebug() << "WidgetOverlayer::eventFilter: source move ancestor";

                if (widget == _targetWidget || _targetWidget->isAncestorOf(widget)) {
                    //qDebug() << "Turn off source widget WA_TransparentForMouseEvents";
                    qDebug() << "WidgetOverlayer::eventFilter: target move" << QCursor::pos();
                    const auto localPos = _sourceWidget->mapFromGlobal(QCursor::pos());

                    //const auto sourceWidgetUnderCursor = _sourceWidget->underMouse();

                    if (auto child = topChildAt(_sourceWidget, localPos)) {
                        qDebug() << "Source widget (or one of its descendants) is under the mouse" << QCursor::pos();

                        _sourceWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
	                    {
                            //QMouseEvent event(QEvent::MouseMove,
                            //    localPos,
                            //    QCursor::pos(),  // global pos
                            //    Qt::NoButton,
                            //    Qt::NoButton,
                            //    Qt::NoModifier);
                            //QApplication::sendEvent(child, &event);
	                    }
	                    //_sourceWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

                        return false;
                    }
                        

                    //_sourceWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
                    //{
                    //    
                    //}
                    //_sourceWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

                    //qDebug() << "Turn on source widget WA_TransparentForMouseEvents";

	                //_sourceWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);

                    //QCursor::setPos(dynamic_cast<QMouseEvent*>(event)->globalPos() + QPoint(1, 0));
                    //QCursor::setPos(dynamic_cast<QMouseEvent*>(event)->globalPos() - QPoint(1, 0));

                    //widget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
                    //qDebug() << "WidgetOverlayer::eventFilter: source";
                    //_sourceWidget->setStyleSheet("background-color: red;");

                    //return true;
                }

                if (widget == _sourceWidget) {
                    const auto localPos = _targetWidget->mapFromGlobal(QCursor::pos());
                    const auto child = _sourceWidget->childAt(localPos);


                    if (!topChildAt(_sourceWidget, localPos)) {
                        _sourceWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

                        return true;
                    } else {
                        return false;
                    }
                }
            }

            if (dynamic_cast<QWidget*>(target) == _sourceWidget) {
	            
                /*if (shouldReceiveMouseEvents()) {
                    QCoreApplication::sendEvent(_targetWidget, event);
                    return true;
                }*/
            }

            //_sourceWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);

            break;
        }

        case QEvent::Leave:
		{
            if (dynamic_cast<QWidget*>(target) == _targetWidget)
                qDebug() << "WidgetOverlayer::eventFilter: target leave" << QCursor::pos();

            break;
	    }

        case QEvent::Enter:
        {
            if (dynamic_cast<QWidget*>(target) == _targetWidget)
                qDebug() << "WidgetOverlayer::eventFilter: target enter" << QCursor::pos();

            break;
        }

        default:
            break;
    }

    return false;
}

void WidgetOverlayer::synchronizeGeometry() const
{
    _sourceWidget->resize(_targetWidget->size());
}

void WidgetOverlayer::enableTrackingRecursive(QWidget* w)
{
	if (!w) return;

	w->setMouseTracking(true);
	w->setAttribute(Qt::WA_Hover, true);

	// If it's a scroll-area based widget (e.g., QTreeView/QListView/QTextEdit),
	// moves happen on the viewport.
	if (auto     sa = qobject_cast<QAbstractScrollArea*>(w)) {
		if (auto vp = sa->viewport()) {
			vp->setMouseTracking(true);
			vp->setAttribute(Qt::WA_Hover, true);
		}
	}

	// Direct children only; recursion handles the rest
	const auto kids = w->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
	for (QWidget* c : kids)
		enableTrackingRecursive(c);
}
}
