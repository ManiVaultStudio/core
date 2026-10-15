// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QObject>
#include <QString>
#include <QPersistentModelIndex>

namespace mv
{
    class AbstractHeadsUpDisplayModel;
}

namespace mv::util
{

class HeadsUpDisplayItem;

using HeadsUpDisplayItemSharedPtr = std::shared_ptr<HeadsUpDisplayItem>;

/**
 * Heads-up display item class
 *
 * Used in the heads-up display model classes.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT HeadsUpDisplayItem final : public QObject
{
public:

    /**
     * Construct with \p title, \p value, \p description and \p parent
     * @param title Title
     * @param value Value
     * @param description Description
     * @param parent Shared pointer to parent item (default: nullptr)
     */
    HeadsUpDisplayItem(const QString& title, const QString& value, const QString& description, const HeadsUpDisplayItemSharedPtr& parent = nullptr);

protected:

    /**
     * Set persistent model \p index of the item
     * @param index Persistent model index of the item
     */
    void setIndex(const QPersistentModelIndex& index);

public: // Getters

    QString getId() const { return _id; }
    QString getTitle() const { return _title; }
    QString getValue() const { return _value; }
    QString getDescription() const { return _description; }
    HeadsUpDisplayItemSharedPtr getParent() const { return _parent; }
    QPersistentModelIndex getIndex() const { return _index; }

private:
    const QString                   _id;            /** Globally unique identifier of the serializable object */
    QString                         _title;         /** Title */
    QString                         _value;         /** Value */
    QString                         _description;   /** Description */
    HeadsUpDisplayItemSharedPtr     _parent;        /** Shared pointer to parent item (maybe nullptr) */
    QPersistentModelIndex           _index;         /** Persistent model index of the item */

    friend class mv::AbstractHeadsUpDisplayModel;
};

}
