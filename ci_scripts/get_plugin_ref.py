import sys
from plugin_branch_utils import get_plugin_release_branch, get_repo_main_name

if __name__ == "__main__":
    """provide the following arguments in order:
    org_name (string): name of the github organization
    repo_name (string): name of the git hub repo
    access_token (string): a Personal Access Token with at least repo permissions
    core_version (string): core version to match - optional

    Returns a GitHub reference to the correct branch of the plugin
    either the "refs/heads/core_<core_version>/<plugin_version>" or
    "refs/heads/master"
    """
    release_branch = None
    if len(sys.argv) == 5:
        release_branch = get_plugin_release_branch(
            sys.argv[1], sys.argv[2], sys.argv[4], sys.argv[3]
        )
    if (len(sys.argv) == 4) or (release_branch is None):
        main_name = get_repo_main_name(sys.argv[1], sys.argv[2], sys.argv[3])
        print(f"refs/heads/{main_name}")
    else:
        print(f"refs/heads/{release_branch}")
