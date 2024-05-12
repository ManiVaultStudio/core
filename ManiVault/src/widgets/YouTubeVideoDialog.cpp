// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "YouTubeVideoDialog.h"

#include <Application.h>

#include <QVBoxLayout>
#include <QWebChannel>
#include <QWebEngineSettings>
#include <QWebEngineProfile>

#ifdef _DEBUG
    #define YOUTUBE_VIDEO_DIALOG_VERBOSE
#endif

namespace mv::gui {

YouTubeVideoDialog::YouTubeVideoDialog(const QString& videoId, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _videoId(videoId),
    _webEngineView(),
    _videoPage()
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("video"));
    setModal(true);

    auto channel = new QWebChannel(this);

    //channel->registerObject(QStringLiteral("content"), &_markdownDocument);

    connect(&_videoPage, &QWebEnginePage::loadFinished, this, [this]() -> void {
    });

    auto settings = _webEngineView.settings();

    //settings->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    //settings->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
    //settings->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
    //settings->setAttribute(QWebEngineSettings::SpatialNavigationEnabled, true);
    //settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    //settings->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);

    //_webEngineView.page()->setFeaturePermission(QUrl("https://www.youtube.com/watch?v=rNSnfXl1ZjU"), QWebEnginePage::MediaAudioVideoCapture,QWebEnginePage::PermissionGrantedByUser);

    //_webEngineView.setHtml(R"(<iframe width="800" height="600" src="https://www.youtube.com/embed/FOsYRGm071Q" title="Qt 6 - The Ultimate UX Development Platform" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>)");

    QWebEngineProfile::defaultProfile()->setHttpUserAgent("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.212 Safari/537.36");

    const auto videoUrl = QString("https://www.youtube.com/embed/%1").arg(_videoId);
    _webEngineView.setUrl(QUrl(videoUrl));

    //QString htmlContent = QString("<iframe width=\"560\" height=\"315\" src=\"https://www.youtube.com/embed/%1\" frameborder=\"0\" allowfullscreen></iframe>").arg(videoId);
    //_webEngineView.setHtml(htmlContent);

    auto layout = new QVBoxLayout();

    layout->addWidget(&_webEngineView);

    auto bottomLayout = new QHBoxLayout();

    bottomLayout->addStretch(1);

    layout->addLayout(bottomLayout);

    setLayout(layout);
}

void YouTubeVideoDialog::play(const QString& videoId)
{
    YouTubeVideoDialog youTubeVideoDialog(videoId);

    youTubeVideoDialog.exec();

}

}