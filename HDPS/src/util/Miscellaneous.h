#pragma once

#include <QString>
#include <QVector>

class QAction;

namespace hdps::util
{

/**
 * Returns a human readable string of an integer count
 * @param count Integer count
 * @return Human readable string of an integer count
 */
QString getIntegerCountHumanReadable(const float& count);

/**
 * Returns a human readable string of a byte count
 * @param noBytes Number of bytes
 * @return Human readable string of a byte count
 */
QString getNoBytesHumanReadable(float noBytes);

/**
 * Sort action based on their text
 * @param actions Actions to sort
 */
void sortActions(QVector<QAction*>& actions);

}
