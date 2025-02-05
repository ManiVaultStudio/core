// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Notifications.h"

#include <QMainWindow>

namespace mv::util
{
     // Seed for random number generation

    QString generateLoremIpsum(int minWords, int maxWords) {
        const QStringList loremIpsumWords = {
            "Lorem", "ipsum", "dolor", "sit", "amet", "consectetur", "adipiscing", "elit", "Sed", "do",
            "eiusmod", "tempor", "incididunt", "ut", "labore", "et", "dolore", "magna", "aliqua", "Ut",
            "enim", "ad", "minim", "veniam", "quis", "nostrud", "exercitation", "ullamco", "laboris",
            "nisi", "ut", "aliquip", "ex", "ea", "commodo", "consequat", "Duis", "aute", "irure", "dolor",
            "in", "reprehenderit", "in", "voluptate", "velit", "esse", "cillum", "dolore", "eu", "fugiat",
            "nulla", "pariatur", "Excepteur", "sint", "occaecat", "cupidatat", "non", "proident", "sunt",
            "in", "culpa", "qui", "officia", "deserunt", "mollit", "anim", "id", "est", "laborum",
            "Curabitur", "pretium", "tincidunt", "lacus", "Nulla", "gravida", "orci", "a", "odio",
            "Nullam", "varius", "turpis", "et", "commodo", "pharetra", "est", "eros", "bibendum", "elit",
            "nec", "luctus", "magna", "felis", "sollicitudin", "mauris", "Integer", "in", "mauris", "eu",
            "nibh", "euismod", "gravida", "Duis", "ac", "tellus", "et", "risus", "vulputate", "vehicula",
            "Donec", "lobortis", "risus", "a", "elit", "Etiam", "tempor", "Ut", "ullamcorper", "ligula",
            "eu", "tempor", "congue", "eros", "est", "euismod", "turpis", "id", "tincidunt", "sapien",
            "risus", "a", "quam", "Maecenas", "fermentum", "consequat", "mi", "Donec", "fermentum",
            "Pellentesque", "malesuada", "nulla", "a", "mi", "Duis", "sapien", "sem", "aliquet", "nec",
            "commodo", "eget", "consequat", "quis", "neque", "Aliquam", "faucibus", "elit", "ut", "dictum",
            "aliquet", "felis", "nisl", "adipiscing", "sapien", "sed", "malesuada", "diam", "lacus",
            "eget", "erat", "Cras", "mollis", "scelerisque", "nunc", "Nullam", "arcu", "Aliquam",
            "consequat", "Curabitur", "augue", "lorem", "dapibus", "quis", "laoreet", "et", "pretium",
            "ac", "nisi", "Aenean", "magna", "nisl", "mollis", "quis", "molestie", "eu", "feugiat", "in",
            "orci", "In", "hac", "habitasse", "platea", "dictumst", "Fusce", "convallis", "mauris",
            "imperdiet", "gravida", "bibendum", "nisl", "turpis", "suscipit", "mauris", "sed", "placerat",
            "ipsum", "urna", "sed", "risus", "Class", "aptent", "taciti", "sociosqu", "ad", "litora",
            "torquent", "per", "conubia", "nostra", "per", "inceptos", "himenaeos"
        };

        QStringList result;
        

        int wordCount = minWords + std::rand() % (maxWords - minWords + 1);

        for (int i = 0; i < wordCount; ++i) {
            int index = std::rand() % loremIpsumWords.size();
            result.append(loremIpsumWords[index]);
        }

        return result.join(' ');
    }

Notifications::Notifications(QWidget* parent, Position position) :
	QObject(parent),
	_parentWidget(parent),
	_position(position)
{
    std::srand(std::time(nullptr));
}

void Notifications::showMessage(const QString& title, const QString& description, const QIcon& icon)
{
    Q_ASSERT(_parentWidget);

    if (!_parentWidget)
        return;

	auto notification = new Notification(title, QString("%1: %2").arg(QString::number(_notifications.size()), generateLoremIpsum(1,  100)), icon, _notifications.isEmpty() ? nullptr : _notifications.last(), _parentWidget);

	notification->show();

	_notifications.append(notification);

	connect(notification, &Notification::finished, this, [this, notification]() {
		_notifications.removeOne(notification);
	});
}

void Notifications::setParentWidget(QWidget* parentWidget)
{
    _parentWidget = parentWidget;

    Application::getMainWindow()->installEventFilter(this);
}

bool Notifications::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::Resize) {
        for (auto notification : _notifications)
            QTimer::singleShot(10, notification, &Notification::updatePosition);
    }

	return QObject::eventFilter(watched, event);
}
}
