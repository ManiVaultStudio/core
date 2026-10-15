// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <ManiVaultGlobals.h>

#include <QKeySequence>
#include <QObject>
#include <QString>

#include <vector>

namespace mv::util {

/**
 * Shortcut map class
 *
 * Helper class for storing a map of categorized shortcuts and
 * creating a shortcut cheatsheet overlay widget
 *
 * Note: This class is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ShortcutMap final : public QObject
{
    Q_OBJECT

public:

    struct Shortcut
    {
        QKeySequence    _keySequence;
        QString         _category;
        QString         _title;

        bool operator==(const Shortcut& other) const {
            if (other._keySequence != _keySequence)
                return false;

            if (other._category != _category)
                return false;

            if (other._title != _title)
                return false;

            return true;
        }
    };

    using Shortcuts = std::vector<Shortcut>;

    /**
     * Construct with \p title and pointer to \p parent object
     * @param title Shortcut map title
     * @param parent Pointer to parent object
     */
    explicit ShortcutMap(const QString& title = "", QObject* parent = nullptr);

    /** No need for custom destructor */
    virtual ~ShortcutMap() override = default;

    /**
     * Get shortcut map title
     * @return Shortcut map title
     */
    QString getTitle() const;

    /**
     * Set shortcut map title to \p title
     * @param title Shortcut map title
     */
    void setTitle(const QString& title);

    /**
     * Add \p shortcut to the map
     * @param shortcut Shortcut to add
     */
    void addShortcut(const Shortcut& shortcut);

    /**
     * Remove \p shortcut from the map
     * @param shortcut Shortcut to remove
     */
    void removeShortcut(const Shortcut& shortcut);

    /**
     * Get shortcuts for \p categories
     * @param categories Categories to retrieve shortcuts for (all shortcuts if empty)
     * @return Vector of shortcuts
     */
    Shortcuts getShortcuts(const QStringList& categories = QStringList()) const;

    /**
     * Establish whether any shortcut exists for \p categories
     * @param categories Categories to check (all categories if empty)
     * @return Boolean determining whether any shortcut exists
     */
    bool hasShortcuts(const QStringList& categories = QStringList()) const;

    /**
     * Get shortcut categories
     * @return Shortcut categories
     */
    QStringList getCategories() const;

signals:

    /**
     * Signals that the shortcut map \p title has changed
     * @param title Updated shortcut map title
     */
    void titleChanged(const QString& title);

    /**
     * Signals that \p shortcut is added
     * @param shortcut Shortcut that is added
     */
    void shortcutAdded(const Shortcut& shortcut);

    /**
     * Signals that \p shortcut is about to be removed
     * @param shortcut Shortcut that is about to be removed
     */
    void shortcutAboutToBeRemoved(const Shortcut& shortcut);

    /**
     * Signals that \p shortcut is removed
     * @param shortcut Shortcut that is removed
     */
    void shortcutRemoved(const Shortcut& shortcut);

private:
    QString         _title;         /** Shortcut map title */
    Shortcuts       _shortcuts;     /** All shortcuts */
};

}
