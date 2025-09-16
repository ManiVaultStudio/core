// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SplashScreenBridge.h"

#include <QDebug>

namespace mv::util
{

SplashScreenBridge::SplashScreenBridge(QObject* parent):
	QObject(parent)
{
}

int SplashScreenBridge::progress() const
{
	return _progress;
}

QString SplashScreenBridge::progressDescription() const
{
	return _progressDescription;
}

void SplashScreenBridge::setProgress(int progress)
{
	progress = std::clamp(progress, 0, 100);

	if (progress == _progress)
        return;

	_progress = progress;

	emit progressChanged(_progress);
}

void SplashScreenBridge::setProgressDescription(const QString& progressDescription)
{
    if (progressDescription == _progressDescription)
        return;

    _progressDescription = progressDescription;

    emit progressDescriptionChanged(_progressDescription);
}

void SplashScreenBridge::requestInitial()
{
	emit progressChanged(_progress);
    emit progressDescriptionChanged(_progressDescription);
}

}
