// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QComboBox>

namespace mv::gui {

/**
 * Multi-select combobox widget class
 *
 * Allows to select multiple items during a single popup sessions.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT MultiSelectComboBox : public QComboBox {

    Q_OBJECT

public:

    
    /**
     * Construct with pointer to \p parent widget
     * @param parent Pointer to parent widget
     */
    explicit MultiSelectComboBox(QWidget* parent = nullptr);

    /** Call this post QComboBox::setView() to setup connections */
    void init();

protected:

    /** Override to customize popup hide behaviour */
    void hidePopup() override;

    
    /**
     * Called when \p event happens for \p watched object
     * @param watched Pointer to watched object
     * @param event Pointer to event that occurred
     * @return Whether the event was handled or not
     */
    bool eventFilter(QObject* watched, QEvent* event) override;

private:

    
    /**
     * Toggle the check state of \p index
     * @param index Index of which to toggle the check state
     */
    void toggleItemCheckState(const QModelIndex& index) const;

private:
    bool _preventHidePopup = false; /** Whether to prevent the popup from closing or not */
};

}
