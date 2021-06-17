import sys
from plugin_branch_utils import get_core_release_version_from_ref

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("")
    print(get_core_release_version_from_ref(sys.argv[1]))