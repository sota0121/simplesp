﻿set(target "sp_calibration")
message(STATUS "${target}")

include(../../cmake_base.txt)

set_target_properties(${target} PROPERTIES
    FOLDER "sp"
)
