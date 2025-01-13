// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QDialog>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>

namespace mv::util {
    class ShortcutMap;
}

namespace mv::plugin {
    class ViewPlugin;
}

namespace mv::gui
{

/**
 * Plugin shortcuts dialog class
 *
 * Shortcuts cheat sheet dialog
 *  
 * @author Thomas Kroes
 */
class CORE_EXPORT PluginShortcutsDialog : public QDialog
{
public:

    /**
     * Construct with pointer to source \p view plugin and pointer to \p parent widget
     * @param viewPlugin Pointer to source view plugin
     * @param parent Pointer to parent widget (maybe nullptr)
     */
    PluginShortcutsDialog(mv::plugin::ViewPlugin* viewPlugin, QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return { 450, 600 };
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:
    const mv::util::ShortcutMap&    _shortcutMap;           /** Const reference to the shortcut map for which to create the overlay */
    QScrollArea                     _textScrollArea;        /** Scroll area for the shortcut */
    QWidget                         _textWidget;            /** Widget with the shortcuts label */
    QVBoxLayout                     _textWidgetLayout;      /** Widget with the shortcuts label */
    QLabel                          _textBodyLabel;         /** Shortcut cheatsheet HTML */
};

}
