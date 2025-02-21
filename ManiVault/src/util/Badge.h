// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QObject>
#include <QColor>
#include <QPixmap>

namespace mv::util {

/**
 * Badge class
 * 
 * Draws a circle with a number with any Qt::Alignment flag and foreground (text)
 * and background color. Currently used by the collapsed widget only.
 * 
 * Note: By default the badge is disabled, use Badge::setEnabled(...) to enable it
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT Badge : public QObject
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object, badge \p number, \p backgroundColor, \p foregroundColor, \p alignment and \p badge scale
     * @param parent Pointer to parent object
     * @param number Number displayed in the badge
     * @param backgroundColor Background color the badge
     * @param foregroundColor Foreground color the badge
     * @param alignment Alignment of the badge
     * @param scale Scale of the badge w.r.t. of its container
     */
    Badge(QObject* parent = nullptr, std::uint32_t number = 0, const QColor& backgroundColor = Qt::red, const QColor& foregroundColor = Qt::white, Qt::Alignment alignment = Qt::AlignTop | Qt::AlignRight, float scale = 0.5f);

    /**
     * Get pixmap
     * @return Pixmap representation of the badge (100 x 100)
     */
    QPixmap getPixmap() const;

    /**
     * Assign \p other badge
     * @param other Other badge
     * @return Copied result
     */
    Badge& operator=(const Badge& other) {
        
        _enabled            = other._enabled;
        _number             = other._number;
        _backgroundColor    = other._backgroundColor;
        _foregroundColor    = other._foregroundColor;
        _alignment          = other._alignment;
        _scale              = other._scale;
        _customPixmap       = other._customPixmap;

        return *this;
    }

public: // Getters and setters

    /**
     * Get badge enabled
     * @return Badge enabled
     */
    bool getEnabled() const;

    /**
     * Set badge enabled to \p enabled
     * @param enabled Badge enabled
     */
    void setEnabled(bool enabled);

    /**
     * Get badge number
     * @return Badge number
     */
    std::uint32_t getNumber() const;

    /**
     * Set badge number to \p number
     * @param number Badge number
     */
    void setNumber(std::uint32_t number);

    /**
     * Get background color
     * @return Background color
     */
    QColor getBackgroundColor() const;

    /**
     * Set background color to \p backgroundColor
     * @param backgroundColor Background color
     */
    void setBackgroundColor(const QColor& backgroundColor);

    /**
     * Get foreground color
     * @return Foreground color
     */
    QColor getForegroundColor() const;

    /**
     * Set foreground color to \p foregroundColor
     * @param foregroundColor Foreground color
     */
    void setForegroundColor(const QColor& foregroundColor);

    /**
     * Get badge alignment
     * @return Badge alignment
     */
    Qt::Alignment getAlignment() const;

    /**
     * Set badge alignment to \p alignment
     * @param alignment Badge alignment
     */
    void setAlignment(Qt::Alignment alignment);

    /**
     * Get badge scale
     * @return Badge scale
     */
    float getScale() const;

    /**
     * Set badge scale to \p scale
     * @param scale Badge scale
     */
    void setScale(float scale);

    /**
     * Get custom pixmap
     * @return Custom pixmap
     */
    QPixmap getCustomPixmap() const;

    /**
     * Set custom pixmap to \p customPixmap
     * @param customPixmap Custom pixmap
     */
    void setCustomPixmap(const QPixmap& customPixmap);

signals:

    /**
     * Signals that enabled changed to \p enabled
     * @param enabled Enabled
     */
    void enabledChanged(bool enabled);

    /**
     * Signals that the badge number changed to \p number
     * @param number Badge number
     */
    void numberChanged(std::uint32_t number);

    /**
     * Signals that the background color changed to \p backgroundColor
     * @param backgroundColor Background color
     */
    void backgroundColorChanged(const QColor& backgroundColor);

    /**
     * Signals that the foreground color changed to \p foregroundColor
     * @param foregroundColor Foreground color
     */
    void foregroundColorChanged(const QColor& foregroundColor);

    /**
     * Signals that the badge alignment changed to \p alignment
     * @param alignment Badge alignment
     */
    void alignmentChanged(Qt::Alignment alignmen);

    /**
     * Signals that the badge scale changed to \p scale
     * @param scale Badge scale
     */
    void scaleChanged(float scale);

    /**
     * Signals that the custom pixmap changed to \p customPixmap
     * @param customPixmap Custom pixmap
     */
    void customPixmapChanged(const QPixmap& customPixmap);

    /** Signals that the badge configuration changed */
    void changed();

private:
    bool            _enabled;               /** Whether the badge is enabled or not */
    std::uint32_t   _number;                /** Badge number */
    QColor          _backgroundColor;       /** Background color */
    QColor          _foregroundColor;       /** Foreground (text) color */
    Qt::Alignment   _alignment;             /** Badge alignment */
    float           _scale;                 /** Badge scale w.r.t. its container */
    QPixmap         _customPixmap;          /** Overrides the default pixmap */
};

}
