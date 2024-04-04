#  ___________________________________________________________________________
#
#  Pyomo: Python Optimization Modeling Objects
#  Copyright (c) 2008-2024
#  National Technology and Engineering Solutions of Sandia, LLC
#  Under the terms of Contract DE-NA0003525 with National Technology and
#  Engineering Solutions of Sandia, LLC, the U.S. Government retains certain
#  rights in this software.
#  This software is distributed under the 3-clause BSD License.
#  ___________________________________________________________________________


def amplfunc_merge(amplfunc, pyomo_amplfunc):
    """Merge two AMPLFUNC variable strings eliminating duplicate lines"""
    amplfunc_lines = amplfunc.split("\n")
    existing = set(amplfunc_lines)
    for line in pyomo_amplfunc.split("\n"):
        # Skip lines we already have
        if line not in existing:
            amplfunc_lines.append(line)
    return "\n".join(amplfunc_lines)
