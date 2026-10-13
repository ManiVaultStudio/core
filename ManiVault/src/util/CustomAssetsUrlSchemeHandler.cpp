// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "CustomAssetsUrlSchemeHandler.h"

#include "util/StandardPaths.h"

#include <QWebEngineUrlRequestJob>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QMimeDatabase>

using namespace mv::util;

namespace mv::util
{

CustomAssetsUrlSchemeHandler::CustomAssetsUrlSchemeHandler(QObject* parent):
	QWebEngineUrlSchemeHandler(parent),
	_rootDir(QDir::cleanPath(StandardPaths::getCustomizationDirectory() + "/assets"))
{
}

void CustomAssetsUrlSchemeHandler::requestStarted(QWebEngineUrlRequestJob* job)
{
    const auto url = job->requestUrl();

    const QFileInfo rootInfo(_rootDir);

    if (!rootInfo.exists() || !rootInfo.isDir()) {
        job->fail(QWebEngineUrlRequestJob::UrlNotFound);
        return;
    }

	auto assetRelative = QDir::cleanPath(url.path()); // "/logo.png"

	if (assetRelative.startsWith('/'))
        assetRelative.remove(0, 1);

	const auto abs              = QDir(_rootDir).absoluteFilePath(assetRelative);
	const auto canonicalRoot   = rootInfo.canonicalFilePath();
	const auto canonicalAsset  = QFileInfo(abs).canonicalFilePath();

	if (canonicalRoot.isEmpty() || canonicalAsset.isEmpty()) {
        job->fail(QWebEngineUrlRequestJob::UrlNotFound);
        return;
    }

	const auto canonRoot = QDir::cleanPath(canonicalRoot + QDir::separator());
	const auto canonAbs  = canonicalAsset;

	if (!canonAbs.startsWith(canonRoot) || !QFile::exists(canonAbs)) {
		job->fail(QWebEngineUrlRequestJob::UrlNotFound);
		return;
	}

	auto assetFile = new QFile(canonAbs, job);

	if (!assetFile->open(QIODevice::ReadOnly)) {
		job->fail(QWebEngineUrlRequestJob::RequestAborted);
		return;
	}

	QMimeDatabase mimeDatabase;

	const auto mime = mimeDatabase.mimeTypeForFile(canonAbs).name().toUtf8();

	job->reply(mime, assetFile);
}

}
