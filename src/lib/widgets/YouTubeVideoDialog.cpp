// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "YouTubeVideoDialog.h"

#include "util/StyledIcon.h"

#include <QVBoxLayout>
#include <QWebEngineView>

#ifdef _DEBUG
    #define YOUTUBE_VIDEO_DIALOG_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui {

YouTubeVideoDialog::YouTubeVideoDialog(const QString& videoId, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _videoId(videoId)
{
    setWindowIcon(StyledIcon("video"));
    setModal(true);

    //const auto videoUrl = QUrl(QString("https://www.youtube.com/embed/%1").arg("Ai0fitThkz8"));// _videoId));

    //auto wev = new QWebEngineView(this);

    //wev->load(videoUrl);

    //auto layout = new QVBoxLayout();

    //layout->addWidget(wev);

    //setLayout(layout);
}

void YouTubeVideoDialog::play(const QString& videoId)
{
    YouTubeVideoDialog youTubeVideoDialog(videoId);

    youTubeVideoDialog.exec();

}

}