// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QObject>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

namespace hdps::util {

/**
 * Widget fader utility class
 *
 * Helper class for fading in/out a target widget.
 *
 * @author Thomas Kroes
 */
class WidgetFader : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param targetWidget Pointer to target widget to fade
     * @param opacity Initial opacity [0, 1]
     * @param minimumOpacity Target fade in opacity [0, 1]
     * @param maximumOpacity Target fade out opacity [0, 1]
     * @param animationDuration Duration of the fade in animation
     * @param animationDuration Duration of the fade out animation
     */
    WidgetFader(QObject* parent, QWidget* targetWidget, float opacity = 0.0f, float minimumOpacity = 0.0f, float maximumOpacity = 1.0f, std::int32_t fadeInDuration = 150, std::int32_t fadeOutDuration = 150);

    /**
     * Fade in the target widget
     * @param duration Only used if duration >= 0, otherwise uses _fadeInDuration 
     */
    void fadeIn(std::int32_t duration = -1);

    /**
     * Fade out the target widget
     * @param duration Only used if duration >= 0, otherwise uses _fadeOutDuration
     */
    void fadeOut(std::int32_t duration = -1);

    /**
     * Set opacity
     * @param opacity Opacity
     * @param duration Animation duration
     */
    void setOpacity(float opacity, std::uint32_t duration = 0);

    /**
     * Get whether the target widget is faded in
     * @return Boolean determining whether the target widget is faded in
     */
    bool isFadedIn() const;

    /**
     * Get whether the target widget is faded out
     * @return Boolean determining whether the target widget is faded out
     */
    bool isFadedOut() const;

    /** Get minimum opacity */
    float getMinimumOpacity() const;

    /** Get maximum opacity */
    float getMaximumOpacity() const;

    /**
     * Set minimum opacity
     * @param minimumOpacity Minimum opacity
     */
    void setMinimumOpacity(float minimumOpacity);

    /**
     * Set maximum opacity
     * @param maximumOpacity Maximum opacity
     */
    void setMaximumOpacity(float maximumOpacity);

    /** Get fade-in duration */
    std::int32_t getFadeInDuration() const;

    /** Get fade-out duration */
    std::int32_t getFadeOutDuration() const;

    /**
     * Set fade-in duration
     * @param fadeInDuration Fade-in duration in milliseconds
     */
    void setFadeInDuration(std::int32_t fadeInDuration);

    /**
     * Set fade-out duration
     * @param fadeOutDuration Fade-out duration in milliseconds
     */
    void setFadeOutDuration(std::int32_t fadeOutDuration);

    /**
     * Get opacity effect
     * @return Reference to opacity effect
     */
    QGraphicsOpacityEffect& getOpacityEffect();

signals:

    /** Signals that the animation has faded in */
    void fadedIn();

    /** Signals that the animation has faded out */
    void fadedOut();

private:
    QWidget*                _targetWidget;          /** Pointer to target widget to fade */
    float                   _minimumOpacity;        /** Target fade out opacity */
    float                   _maximumOpacity;        /** Target fade in opacity */
    std::int32_t            _fadeInDuration;        /** Duration of the fade in animation */
    std::int32_t            _fadeOutDuration;       /** Duration of the fade out animation */
    QGraphicsOpacityEffect  _opacityEffect;         /** Effect for modulating opacity */
    QPropertyAnimation      _opacityAnimation;      /** Opacity animation */
};

}
