// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowMetric.h"

#include "util/Miscellaneous.h"

using namespace mv::util;

namespace mv::workflow
{

QString WorkflowMetric::formatMetricValue(const WorkflowMetric& metric)
{
	const auto unit = metric._unit.trimmed().toLower();

	bool ok = false;

	const auto numericValue = metric._value.toDouble(&ok);

	if (!ok)
		return metric._value.toString();

	if (unit == "b" || unit == "bytes") {
		return getNoBytesHumanReadable(static_cast<std::uint64_t>(numericValue));
	}

	if (unit == "ms") {
		return getElapsedTimeHumanReadable(static_cast<std::uint64_t>(numericValue));
	}

	if (unit == "s" || unit == "sec" || unit == "seconds") {
		return getElapsedTimeHumanReadable(static_cast<std::uint64_t>(numericValue * 1000.0));
	}

	if (unit == "b/s" || unit == "bytes/s") {
		return QString("%1/s").arg(getNoBytesHumanReadable(static_cast<std::uint64_t>(numericValue)));
	}

	if (unit == "%" || unit == "percent") {
		return QString("%1%").arg(QString::number(numericValue, 'f', 1));
	}

	if (std::floor(numericValue) == numericValue) {
		if (metric._unit.isEmpty()) {
			return getIntegerCountHumanReadable(numericValue);
		}

		return QString("%1 %2").arg(getIntegerCountHumanReadable(numericValue), metric._unit);
	}

	const int precision = std::abs(numericValue) >= 1000.0 ? 0 : std::abs(numericValue) >= 100.0 ? 1 : std::abs(numericValue) >= 10.0 ? 2 : 3;

	if (metric._unit.isEmpty()) {
		return QString::number(numericValue, 'f', precision);
	}

	return QString("%1 %2")
	       .arg(QString::number(numericValue, 'f', precision))
	       .arg(metric._unit);
}

QString WorkflowMetric::toNotificationString(const WorkflowMetric& metric)
{
	return QString("%1: %2").arg(metric._name, formatMetricValue(metric));
}

QString WorkflowMetric::toHtmlNotificationString(const WorkflowMetric& metric)
{
	return QString("<b>%1</b>: %2").arg(metric._name.toHtmlEscaped(), formatMetricValue(metric).toHtmlEscaped());
}

QString WorkflowMetric::toNotificationSummary(const QList<WorkflowMetric>& metrics, const QString& separator)
{
	QStringList lines;

	for (const auto& metric : metrics)
		lines << toNotificationString(metric);

	return lines.join(separator);
}

QString WorkflowMetric::getWorkflowMetricsHtmlNotificationSummary(const QList<WorkflowMetric>& metrics)
{
	QStringList lines;

	for (const auto& metric : metrics)
		lines << toHtmlNotificationString(metric);

	return lines.join("<br>");
}

}
