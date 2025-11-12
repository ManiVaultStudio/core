function Component()
{
	component.loaded.connect(this, this.installerLoaded);
}

Component.prototype.installerLoaded = function()
{
	installer.setDefaultPageVisible(QInstaller.TargetDirectory, false);
    installer.addWizardPage(component, "TargetWidget", QInstaller.TargetDirectory);

    targetDirectoryPage = gui.pageWidgetByObjectName("DynamicTargetWidget");
	
	targetDirectoryPage.windowTitle = "Choose Installation Directory";
    targetDirectoryPage.targetDirectory.textChanged.connect(this, this.targetDirectoryChanged);
    targetDirectoryPage.targetDirectory.setText(installer.value("TargetDir"));
    targetDirectoryPage.targetChooser.released.connect(this, this.targetChooserClicked);
}

Component.prototype.targetDirectoryChanged = function()
{
	var targetDir = targetDirectoryPage.targetDirectory.text;

	if (installer.fileExists(targetDir) && installer.fileExists(targetDir + "/maintenancetool.exe"))
        targetDirectoryPage.warning.setText("<p style=\"color: red\">Existing installation detected and will be overwritten.</p>");
    else if (installer.fileExists(targetDir))
        targetDirectoryPage.warning.setText("<p style=\"color: red\">Installing in existing directory. It will be wiped on uninstallation.</p>");
    else
        targetDirectoryPage.warning.setText("");

	installer.setValue("TargetDir", targetDir);
}

Component.prototype.targetChooserClicked = function()
{
    var pickedDir = QFileDialog.getExistingDirectory("", targetDirectoryPage.targetDirectory.text);

	if (installer.fileExists(pickedDir))
    	targetDirectoryPage.targetDirectory.setText(installer.toNativeSeparators(pickedDir + "\\Cytosplore Viewer"));
}

Component.prototype.createOperationsForArchive = function(archive)
{
    component.addOperation("Extract", archive, "@TargetDir@");
}

Component.prototype.createOperations = function()
{
    component.createOperations();

	if (systemInfo.productType === "windows") {
		component.addOperation("CreateShortcut", "@TargetDir@/CytosploreViewer.exe", "@DesktopDir@/Cytosplore Viewer.lnk", "description=Start Cytosplore Viewer", "workingDirectory=@TargetDir@");
		component.addOperation("CreateShortcut", "@TargetDir@/CytosploreViewer.exe", "@StartMenuDir@/BioVault/Cytosplore Viewer.lnk", "description=Cytosplore Viewer", "workingDirectory=@TargetDir@");

		var redistributablesInstalled 	= installer.execute("reg", new Array("QUERY", "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\14.0\\VC\\Runtimes\\x64", "/v", "Installed"))[0];
		var doInstall 					= false;

		if (!redistributablesInstalled) {
			doInstall = true;
		}
		else
		{
			var build 		= installer.execute("reg", new Array("QUERY", "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\14.0\\VC\\Runtimes\\x64", "/v", "Bld"))[0];
			var elements 	= build.split(" ");

			build = parseInt(elements[elements.length-1]);

			if (build < 35211)
				doInstall = true;
		}

		if (doInstall) {
			var installVcRedistResult 	= component.addElevatedOperation("Execute", "@TargetDir@\\vc_redist.x64.exe", "/norestart", "/quiet");
			var installVcRedistExitCode	= installVcRedistResult ? installVcRedistResult[0] : 1;

			if (installVcRedistExitCode === 0) {
				console.log("VC++ Redist installed (exit 0).");
			} else if (installVcRedistExitCode === 3010) {
				console.log("VC++ Redist installed; reboot required (exit 3010).");
				installer.setValue("NeedsReboot", "1");
			} else {
				throw "VC++ Redistributable install failed. Exit code: " + installVcRedistExitCode;
			}
		}

		component.addOperation("Delete", "@TargetDir@\\vc_redist.x64.exe");
	}
}

Component.prototype.installationFinished = function()
{
    QDesktopServices.openUrl("file:///" + installer.value("TargetDir") + "/launch.bat");     
}