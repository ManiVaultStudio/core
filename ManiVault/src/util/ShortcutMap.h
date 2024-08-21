// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QObject>
#include <QKeySequence>

#include "widgets/ShortcutMapOverlayWidget.h"

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
        QString         _title;
        QString         _description;
    };

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    explicit ShortcutMap(QObject* parent = nullptr);

    /** No need for custom destructor */
    virtual ~ShortcutMap() override = default;

    /**
     * Create shortcut cheat sheet overlay widget on top of the \p source widget
     * @param source Pointer to source widget
     * @return Shared pointer to shortcut map overlay widget
     */
    std::shared_ptr<gui::ShortcutMapOverlayWidget> createShortcutMapOverlayWidget(QWidget* source) const;

signals:

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
};

}
