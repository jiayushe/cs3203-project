# - Try to find Autotester library
#
# The following variables are optionally searched for defaults
#  AUTOTESTER_ROOT:            Base directory where all autotester components are found
#
# The followings are set after configuration is done:
#  AUTOTESTER_FOUND
#  AUTOTESTER_LIBRARIES

include(FindPackageHandleStandardArgs)

find_library(AUTOTESTER_LIBRARY_DEBUG autotester
        PATHS ${AUTOTESTER_ROOT}/debug
        PATH_SUFFIXES
        lib
        lib64)

find_library(AUTOTESTER_LIBRARY_RELEASE autotester
        PATHS ${AUTOTESTER_ROOT}/release
        PATH_SUFFIXES
        lib
        lib64)

set(AUTOTESTER_LIBRARY
        debug ${AUTOTESTER_LIBRARY_DEBUG}
        optimized ${AUTOTESTER_LIBRARY_RELEASE})

find_package_handle_standard_args(AUTOTESTER DEFAULT_MSG AUTOTESTER_LIBRARY)

if (AUTOTESTER_FOUND)
    set(AUTOTESTER_LIBRARIES ${AUTOTESTER_LIBRARY})
endif ()
