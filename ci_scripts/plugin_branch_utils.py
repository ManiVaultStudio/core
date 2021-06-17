import os
import re
from github import Github
from packaging import version


def list_all_repo_branches(org_name, access_token):
    """Print all the repo branches for the organisation
    """
    g = Github(access_token)
    org = g.get_organization(org_name)
    repos = org.get_repos()
    for r in repos:
        print(f"Repo: {r.name}")
        branches = r.get_branches()
        for b in branches:
            print(f"    Branch: {b.name}")


def get_plugin_release_branch(org_name, repo_name, core_version, access_token):
    """Get the the plugin branch name for the core_version with
        the highest version.

    Args:
        org_name (string): name of the github organization
        repo_name (string): name of the git hub repo
        core_version (string): core version to match
        access_token (string): a Personal Access Token with at least repo permissions

    Returns:
        string: the plugin branch name for the core_version with 
        the highest version
    """
    g = Github(access_token)
    org = g.get_organization(org_name)
    repo = org.get_repo(repo_name)
    # using python's built in versioning pattern that accomodated
    # many more features than we need
    branch_pattern = re.compile(fr'release/core_{core_version}/(?P<release>[0-9]+(?:\.[0-9]+)*)')
    branches = repo.get_branches()
    highest_version = None
    highest_name = None
    for b in branches:
        m = branch_pattern.match(b.name)
        if m is not None:
            plugin_version = m.group('release')
            if highest_version is None:
                highest_version = plugin_version
                highest_name = b.name
            elif version.parse(plugin_version) > version.parse(highest_version):
                highest_version = plugin_version
                highest_name = b.name

    if highest_version is None:
        return None
    return highest_name


def get_repo_main_name(org_name, repo_name, access_token):
    """Return the name "master" or "main" depending
    on what exists in the repo.

    Args:
        org_name ([type]): [description]
        repo_name ([type]): [description]
        access_token ([type]): [description]

    Return: (string) If both are present it returns "main"
        If none are present returns None
    """
    g = Github(access_token)
    org = g.get_organization(org_name)
    repo = org.get_repo(repo_name)
    branches = repo.get_branches()
    has_master = False
    for b in branches:
        if b.name == "main":
            return "main"
        if b.name == "master":
            has_master = True

    if has_master:
        return "master"
    return None


def get_core_release_version_from_ref(github_ref):
    branch_pattern = re.compile('refs/heads/(?P<branch>.*)')
    m = branch_pattern.match(github_ref)
    if m is None:
        return None
    branch_name = m.group('branch')
    release_pattern = re.compile('release/(?P<version>.*)')
    m = release_pattern.match(branch_name)
    if m is None:
        return None
    return m.group('version')


if __name__ == "__main__":
    access_token = os.environ["REPO_ACCESS_TOKEN"]
    list_all_repo_branches("hdps", access_token)
