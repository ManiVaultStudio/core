// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WatchVideoAction.h"

#include <QDesktopServices>

#include "util/LearningCenterVideo.h"

using namespace mv::util;

namespace mv::gui {

WatchVideoAction::WatchVideoAction(QObject* parent, const QString& title, const util::LearningCenterVideo* video /*= nullptr*/) :
    TriggerAction(parent, title),
    _video(video)
{
    Q_ASSERT(_video);

    if (_video) {
        switch (_video->getType()) {
			case LearningCenterVideo::Type::YouTube:
	            setIconByName("youtube");
	            break;

	        case LearningCenterVideo::Type::GIF:
	            setIconByName("video");
	            break;
        }

        connect(this, &TriggerAction::triggered, this, &WatchVideoAction::watch);
    }
}

void WatchVideoAction::watch() const
{
    Q_ASSERT(_video);

    if (!_video)
        return;

    switch (_video->getType()) {
	    case LearningCenterVideo::Type::YouTube: {
	    	QDesktopServices::openUrl(QString("https://www.youtube.com/watch?v=%1").arg(_video->getResource()));
	    	break;
	    }

    	case LearningCenterVideo::Type::GIF: {
	    	qDebug() << "Watch GIF";
	    	break;
    	}
    }
}

}
