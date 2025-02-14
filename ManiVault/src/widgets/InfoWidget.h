// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "util/ThemeWatcher.h"
#include "util/StyledIcon.h"

#include <QWidget>
#include <QLabel>

#include "actions/ColorAction.h"

namespace mv::gui
{

/**
 * Info widget class
 *
 * Standard widget which displays an icon, title and description (used for instance as an overlay for other widgets).
 *  
 * @author Thomas Kroes
 */
class CORE_EXPORT InfoWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * Construct info widget with no text and style with application foreground and background color roles
     * @param parent Pointer to parent widget
     */
    InfoWidget(QWidget* parent);

    /**
     * Construct info widget with text and style with foreground and background colors
     * @param parent Pointer to parent widget
     * @param icon Icon
     * @param title Title of the overlay
     * @param description Overlay description
     */
    InfoWidget(QWidget* parent, const QIcon& icon, const QString& title, const QString& description = "");

    /**
     * Set overlay parameters
     * @param icon Icon
     * @param title Title of the overlay
     * @param description Overlay description
     */
    void set(const QIcon& icon, const QString& title, const QString& description = "");

    /**
     * Get foreground color
     * @return Foreground color
     */
    QColor getForegroundColor() const;

    /**
     * Set foreground color to \p foregroundColor (resets the color roles)
     * @param foregroundColor Foreground color
     */
    void setForegroundColor(const QColor foregroundColor);

    /**
     * Get background color
     * @return Background color
     */
    QColor getBackgroundColor() const;

    /**
     * Set background color to \p backgroundColor (resets the color roles)
     * @param backgroundColor Background color
     */
    void setBackgroundColor(const QColor backgroundColor);

    /**
     * Set colors (resets the color roles)
     * @param foregroundColor Foreground color
     * @param backgroundColor Background color
     */
    void setColors(const QColor& foregroundColor, const QColor& backgroundColor);

    /**
     * Get foreground color role
     * @return Foreground color role (zero if not used)
     */
    std::int32_t getForegroundColorRole() const;

    /**
     * Set foreground color role to \p foregroundColorRole
     * @param foregroundColorRole Foreground color role
     */
    void setForegroundColorRole(const QPalette::ColorRole& foregroundColorRole);

    /**
     * Get background color role
     * @return Background color role (zero if not used)
     */
    std::int32_t getBackgroundColorRole() const;

    /**
     * Set background color role to \p backgroundColorRole
     * @param backgroundColorRole Background color role
     */
    void setBackgroundColorRole(const QPalette::ColorRole& backgroundColorRole);

    /**
     * Set color roles
     * @param foregroundColorRole Foreground color role
     * @param backgroundColorRole Background color role
     */
    void setColorRoles(const QPalette::ColorRole& foregroundColorRole, const QPalette::ColorRole& backgroundColorRole);

private:

    /** Initializes the controls */
    void initialize();

    /** Updates the styling in response to color and/or theme changes */
    void updateStyling();

signals:

    /**
     * Signals that the foreground color changed from \p previousForegroundColor to \p currentForegroundColor
     * @param previousForegroundColor Previous foreground color
     * @param currentForegroundColor Current foreground color
     */
    void foregroundColorChanged(const QColor& previousForegroundColor, const QColor& currentForegroundColor);

    /**
     * Signals that the background color changed from \p previousBackgroundColor to \p currentBackgroundColor
     * @param previousBackgroundColor Previous background color
     * @param currentBackgroundColor Current background color
     */
    void backgroundColorChanged(const QColor& previousBackgroundColor, const QColor& currentBackgroundColor);

    /**
     * Signals that the foreground color role changed from \p previousForegroundColorRole to \p currentForegroundColorRole
     * @param previousForegroundColorRole Previous foreground color role
     * @param currentForegroundColorRole Current foreground color role
     */
    void foregroundColorRoleChanged(const QPalette::ColorRole& previousForegroundColorRole, const QPalette::ColorRole& currentForegroundColorRole);

    /**
     * Signals that the background color role changed from \p previousBackgroundColorRole to \p currentBackgroundColorRole
     * @param previousBackgroundColorRole Previous background color role
     * @param currentBackgroundColorRole Current background color role
     */
    void backgroundColorRoleChanged(const QPalette::ColorRole& previousBackgroundColorRole, const QPalette::ColorRole& currentBackgroundColorRole);

private:
    util::StyledIcon    _icon;                  /** Theme-aware icon */
    QLabel              _iconLabel;             /** Label for displaying the icon */
    QLabel              _titleLabel;            /** Label for displaying the title */
    QLabel              _descriptionLabel;      /** Label for displaying the description */
    QColor              _foregroundColor;       /** Foreground color of the widget */
    QColor              _backgroundColor;       /** Background color of the widget */
    std::int32_t        _foregroundColorRole;   /** Color role for foreground elements (InfoWidget::_foregroundColor will be used if zero) */
    std::int32_t        _backgroundColorRole;   /** Color role for background elements (InfoWidget::_backgroundColor will be used if zero) */
    util::ThemeWatcher  _themeWatcher;          /** Update the styling when the theme changes */

    static QColor defaultForegroundColor;
    static QColor defaultBackgroundColor;
};

}
