// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

class QWidget;

namespace mv::gui {

class CORE_EXPORT MultiSelectComboBox : public QComboBox {

	Q_OBJECT

public:
    explicit MultiSelectComboBox(QWidget* parent = nullptr);

    void init();

protected:
    void showPopup() override;

    void hidePopup() override;

    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void toggleItemCheckState(const QModelIndex& index) const;

    void updateDisplayText();

private:
    bool _popupOpen = false;
    bool _preventHidePopup = false;
};

}
