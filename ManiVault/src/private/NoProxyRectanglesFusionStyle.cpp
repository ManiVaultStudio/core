// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "NoProxyRectanglesFusionStyle.h"

#include <QStyleFactory>

NoProxyRectanglesFusionStyle::NoProxyRectanglesFusionStyle():
	QProxyStyle(QStyleFactory::create("Fusion"))
{
}

int NoProxyRectanglesFusionStyle::styleHint(StyleHint hint, const QStyleOption* option, const QWidget* widget, QStyleHintReturn* returnData) const
{
	if (hint == QStyle::SH_Widget_Animate)
		return 0;  // Disable proxy rectangles & animation effects

	return QProxyStyle::styleHint(hint, option, widget, returnData);
}
