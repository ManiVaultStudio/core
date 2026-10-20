// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/Serializable.h>

#include <DockWidget.h>

#include <QMenu>

class QToolButton;

/**
 * Dock widget class
 *
 * ADS dock widget class enhanced with:
 * - Serialization
 * - Ability to choose additional settings from tool button in tab bar
 *
 * @author Thomas Kroes
 */
class DockWidget : public ads::CDockWidget, public mv::util::Serializable
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param title Title of the dock widget
     * @param parent Pointer to parent widget
     */
    DockWidget(const QString& title, QWidget* parent = nullptr);

    /** Destructor */
    ~DockWidget() override;

    /**
     * Get string that describes the dock widget type
     * @return Type string
     */
    virtual QString getTypeString() const;

    /** Performs custom dock widget initialization */
    virtual void initialize() {};

    /**
     * Invoked when the menu is shown
     * @param showEvent Pointer to the show event
     */
    void showEvent(QShowEvent* showEvent) override;

    /**
     * Get settings menu (when derived dock widgets return a valid menu, a tool button will in the tab bar that shows this menu)
     * @return Pointer to settings menu
     */
    virtual QMenu* getSettingsMenu();

    /**
     * Override ADS dock widget
     * @param widget Pointer to widget
     * @param insertMode Insert modus
     */
    void setWidget(QWidget* widget, eInsertMode insertMode = AutoScrollArea);

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

private:
    QToolButton*    _settingsToolButton;    /** Pointer to settings tool button (located in the dock widget tab bar) */
    QMenu           _settingsMenu;          /** Settings menu for settings tool button */
};

using DockWidgets = QVector<DockWidget*>;
