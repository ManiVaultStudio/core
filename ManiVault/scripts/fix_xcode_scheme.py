import sys
import xml.etree.ElementTree as ET

def set_debug_executable(scheme_file, executable_path):
    # Parse the .xcscheme XML file
    tree = ET.parse(scheme_file)
    root = tree.getroot()

    # Find the LaunchAction node
    launch_action = root.find(".//LaunchAction")
    if launch_action is not None:
        launch_action.set("selectedDebuggerIdentifier", "Xcode.DebuggerFoundation.Debugger.LLDB")
        launch_action.set("selectedLauncherIdentifier", "Xcode.DebuggerFoundation.Launcher.LLDB")
        launch_action.set("launchStyle", "0")  # Launch in debugger

        # Find or create the PathRunnable element
        path_runnable = launch_action.find("PathRunnable")
        if path_runnable is None:
            # If PathRunnable does not exist, create it
            path_runnable = ET.SubElement(launch_action, "PathRunnable")
        
        # Set the filePath attribute to the new executable path
        path_runnable.set("filePath", executable_path)

        # Save changes to the scheme file
        tree.write(scheme_file)
    else:
        print("LaunchAction not found in scheme")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: adjust_xcode_scheme.py <scheme_file> <executable_path>")
        sys.exit(1)

    scheme_file_path = sys.argv[1]
    executable_path = sys.argv[2]

    set_debug_executable(scheme_file_path, executable_path)
