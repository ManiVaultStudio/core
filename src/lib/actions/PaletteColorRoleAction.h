// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "HorizontalGroupAction.h"
#include "PaletteColorAction.h"
#include "StringAction.h"

#include <QPalette>

namespace mv::gui {

/**
 * Palette color role class
 *
 * For editing a palette color role
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT PaletteColorRoleAction : public HorizontalGroupAction
{
    Q_OBJECT

public:

    /** Palette color actions */
    using PaletteColorActions = std::array<std::unique_ptr<PaletteColorAction>, QPalette::ColorGroup::NColorGroups>;

    /**
     * Construct a palette color rolew action with a \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param colorRole Color role of the palette color
     */
    Q_INVOKABLE PaletteColorRoleAction(QObject* parent, const QString& title, const QPalette::ColorRole& colorRole = QPalette::ColorRole::Window);

    /**
     * Initialize with the palette \p colorRole
     * @param colorRole Color role of the palette color
     */
    void initialize(const QPalette::ColorRole& colorRole);

public: // Action getters

    StringAction& getNameAction() { return _nameAction; }
    PaletteColorAction& getPaletteColorAction(QPalette::ColorRole& colorRole) { return *_paletteColorActions[colorRole]; }

private:
    StringAction            _nameAction;            /** Name of the palette color role */
    QPalette::ColorRole     _colorRole;             /** Color role of the palette color */
    PaletteColorActions     _paletteColorActions;   /** Palette color actions for each palette color group */

protected:
    static QStringList  colorGroupNames;    /** Names of the color groups */

    friend class PaletteColorAction;
};

}

Q_DECLARE_METATYPE(mv::gui::PaletteColorRoleAction)

inline const auto paletteColorRoleActionActionMetaTypeId = qRegisterMetaType<mv::gui::PaletteColorRoleAction*>("mv::gui::PaletteColorRoleAction");
