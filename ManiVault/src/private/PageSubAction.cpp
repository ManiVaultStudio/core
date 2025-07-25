// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageSubAction.h"

using namespace mv::util;

PageSubAction::PageSubAction(const QIcon& icon, const TooltipCallback& tooltipCallback /*= {}*/, const ClickedCallback& clickedCallback /*= {}*/) :
    _clickedCallback(clickedCallback),
	_tooltipCallback(tooltipCallback),
    _iconLabel(icon)
{
    setIcon(icon);

    _iconLabel.setTooltipCallback([this]() -> QString {
        return getTooltip();
    });
}

void PageSubAction::setIcon(const QIcon& icon)
{
    _iconLabel.setIcon(icon);
}

QString PageSubAction::getTooltip() const
{
    if (_tooltipCallback)
        return _tooltipCallback();

    return {};
}

void PageSubAction::setVisible(bool visible)
{
	_iconLabel.setVisible(visible);
}

mv::gui::IconLabel& PageSubAction::getIconLabel()
{
    return _iconLabel;
}

CommentsPageSubAction::CommentsPageSubAction(const QString& comments) :
	PageSubAction(mv::util::StyledIcon("scroll"))
{
    setTooltipCallback([this, comments]() -> QString {
        return comments;
    });
}

TagsPageSubAction::TagsPageSubAction(const QStringList& tags) :
    PageSubAction(mv::util::StyledIcon("tags"))
{
    setTooltipCallback([this, tags]() -> QString {
        return tags.join(", ");
    });
}

PageDownloadSubAction::PageDownloadSubAction(const QString& downloadSize) :
    PageSubAction(mv::util::StyledIcon("download"))
{
    setTooltipCallback([this, downloadSize]() -> QString {
        return QString("Requires a %1 download").arg(downloadSize);
    });
}

PageCompatibilitySubAction::PageCompatibilitySubAction(const HardwareSpec::SystemCompatibilityInfo& systemCompatibilityInfo) :
    PageSubAction({})
{
	switch (systemCompatibilityInfo._compatibility) {
		case HardwareSpec::SystemCompatibility::Incompatible:
		{
			setIcon(StyledIcon("circle-exclamation").withColor(QColor::fromHsl(0, 100, 100)));
			setTooltipCallback([this]() -> QString {
				return "Your system does not meet the minimum requirements for this project, there might be problems with opening it, its stability and performance!";
			});

			break;
		}

        case HardwareSpec::SystemCompatibility::Minimum:
        {
            setIcon(StyledIcon("circle-exclamation").withColor(QColor::fromHsl(40, 100, 100)));
            setTooltipCallback([this]() -> QString {
                return "Your system does not meet the recommended requirements for this project, the interactivity might not be optimal!";
            });

            break;
        }

        case HardwareSpec::SystemCompatibility::Compatible:
        {
            setIcon(StyledIcon("check"));
            setTooltipCallback([this]() -> QString {
                return "Your system meets the minimum requirements for this project!";
            });
			
            break;
        }

        case HardwareSpec::SystemCompatibility::Unknown:
        {
            setIcon(StyledIcon("circle-question"));
            setTooltipCallback([this]() -> QString {
                return "Your system compatibility is unknown, the project does not specify hardware requirements!";
            });
			
            break;
        }
	}
}

ProjectPreviewPageSubAction::ProjectPreviewPageSubAction() :
    PageSubAction(StyledIcon("image"))
{
}

ContributorsPageSubAction::ContributorsPageSubAction(const QStringList& contributors) :
    PageSubAction(StyledIcon("user"))
{
    setTooltipCallback([this, contributors]() -> QString {
        return QString("Contributor(s): %1").arg(contributors.join(", "));
	});
}

ProjectsJsonUrlPageSubAction::ProjectsJsonUrlPageSubAction(const QUrl& projectJsonUrl) :
    PageSubAction(StyledIcon("globe"))
{
    setTooltipCallback([this, projectJsonUrl]() -> QString {
        return QString("Projects JSON: %1").arg(projectJsonUrl.toString());
	});
}

ProjectPluginsPageSubAction::ProjectPluginsPageSubAction(const QStringList& plugins) :
    PageSubAction(StyledIcon("plug"))
{
    setTooltipCallback([this, plugins]() -> QString {
        return QString("Plugin(s): %1").arg(plugins.isEmpty() ? "none" : plugins.join(", "));
	});
}