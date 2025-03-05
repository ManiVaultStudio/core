// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QProxyStyle>

/**
 * No proxy rectangles Fusion style class
 *
 * Create a custom class to remove the visually disturbing proxy rectangles from the Fusion style
 *
 * @author Thomas Kroes
 */
class NoProxyRectanglesFusionStyle : public QProxyStyle {
public:

    /** Default construction */
    NoProxyRectanglesFusionStyle();

    /**
     * Get style hint
     * @param hint Style hint
     * @param option Style option
     * @param widget Widget
     * @param returnData Return data
     * @return Style hint
     */
    int styleHint(StyleHint hint, const QStyleOption* option = nullptr, const QWidget* widget = nullptr, QStyleHintReturn* returnData = nullptr) const override;
};
