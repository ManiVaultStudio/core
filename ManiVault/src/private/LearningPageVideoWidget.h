// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/WidgetOverlayer.h>

#include <QLabel>
#include <QPersistentModelIndex>
#include <QPixmap>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QWidget>

#include "GroupDataDialog.h"

class LearningPageVideosWidget;

class GlowLabel : public QLabel {
    Q_OBJECT
public:
    explicit GlowLabel(QWidget* parent = nullptr) :
        QLabel(parent),
        shadowEffect(new QGraphicsDropShadowEffect(this)),
        animation(new QPropertyAnimation(this, "glowStrength"))
    {
        // Set up the glow effect
        shadowEffect->setBlurRadius(20);
        shadowEffect->setColor(qApp->palette().color(QPalette::ColorGroup::Normal, QPalette::ColorRole::BrightText));
        shadowEffect->setOffset(0, 0);

        setGraphicsEffect(shadowEffect);

        // Animation setup
        animation->setDuration(300);
        animation->setEasingCurve(QEasingCurve::InOutQuad);
        setGlowStrength(0); // Start with no glow

        setAttribute(Qt::WA_Hover); // Enable hover events
    }

    void setGlowStrength(qreal strength) {
        QColor color = shadowEffect->color();
        color.setAlphaF(strength);  // Adjust transparency
        shadowEffect->setColor(color);
    }

    qreal glowStrength() const {
        return shadowEffect->color().alphaF();
    }

protected:
    void enterEvent(QEnterEvent* event) override {
        animation->stop();
        animation->setStartValue(glowStrength());
        animation->setEndValue(1.0); // Full glow
        animation->start();

        QLabel::enterEvent(event);
    }

    void leaveEvent(QEvent* event) override {
        animation->stop();
        animation->setStartValue(glowStrength());
        animation->setEndValue(0.0); // Fade out glow
        animation->start();

        QLabel::leaveEvent(event);
    }

private:
    QGraphicsDropShadowEffect* shadowEffect;
    QPropertyAnimation* animation;

    Q_PROPERTY(qreal glowStrength READ glowStrength WRITE setGlowStrength)
};

/**
 * Learning page video widget class
 *
 * Widget class which show a learning page video
 *
 * @author Thomas Kroes
 */
class LearningPageVideoWidget : public QWidget
{
private:

    /** Overlay widget that shows video-related actions */
    class OverlayWidget : public QWidget {
    public:

        /**
         * Construct with model \p index and pointer to \p parent widget
         * @param index Model index
         * @param parent Pointer to parent widget
         */
        explicit OverlayWidget(const QModelIndex& index, QWidget* parent);

        /**
         * Respond to target object events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override;

    private:

        /** Updates custom styling */
        void updateStyle();

        void enterEvent(QEnterEvent* event) override {
            
        }

        void leaveEvent(QEvent* event) override {
            _opacityAnimation.stop();
            _opacityAnimation.setStartValue(1);
            _opacityAnimation.setEndValue(0);
            _opacityAnimation.start();

            QWidget::leaveEvent(event);
        }

    private:
        QPersistentModelIndex       _index;                     /** Pointer to owning learning page content widget */
        QVBoxLayout                 _mainLayout;                /** Main vertical layout */
        QHBoxLayout                 _centerLayout;              /** Center horizontal layout for the play button */
        QHBoxLayout                 _bottomLayout;              /** Bottom horizontal layout for the meta labels */
        GlowLabel                      _playIconLabel;             /** Play icon label */
        GlowLabel                      _summaryIconLabel;          /** Summary icon label */
        QLabel                      _dateIconLabel;             /** Date icon label */
        QLabel                      _tagsIconLabel;             /** Tags icon label */
        mv::util::WidgetOverlayer   _widgetOverlayer;           /** Synchronizes the size with the source widget */
        QGraphicsOpacityEffect      _opacityEffect;             /** Opacity effect */
        QPropertyAnimation          _opacityAnimation;          /** Opacity animation */
        
    };

public:

    /**
     * Construct with model \p index and pointer to \p parent widget
     * @param index Model index for videos model
     * @param parent Pointer to parent widget
     */
    LearningPageVideoWidget(const QModelIndex& index, QWidget* parent = nullptr);

    /**
     * Respond to target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

private:
    QPersistentModelIndex       _index;                     /** Pointer to owning learning page content widget */
    QVBoxLayout                 _mainLayout;                /** Main vertical layout */
    QLabel                      _thumbnailLabel;            /** Label that shows the thumbnail pixmap */
    QPixmap                     _thumbnailPixmap;           /** Thumbnail pixmap */
    QTextBrowser                _propertiesTextBrowser;     /** Text browser for showing the video title */
    OverlayWidget               _overlayWidget;             /** Overlay widget widget that shows video-related actions */
    
    friend class LearningPageVideoStyledItemDelegate;
};
