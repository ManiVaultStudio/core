#!/usr/bin/env python
# -*- coding: utf-8 -*-


from cpt.packager import ConanMultiPackager
import os
#def _is_not_shared(build):
#    return build.options['hdps-core:shared'] == False
    
if __name__ == "__main__":

    builder = ConanMultiPackager(reference="hdps-core/0.1.0")
    # builder.add_common_builds()
    builder.add()
    # builder.remove_build_if(_is_not_shared)  
    builder.run('action_build')