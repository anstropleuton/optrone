# Dependency Manager

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/option_str.cmake")

function(depman_declare DEPENDENCY REPO TAG)
    option_str(${PROJECT_NAME}_dep_${DEPENDENCY}_repo "Git repo for ${DEPENDENCY}" ${REPO})
    option_str(${PROJECT_NAME}_dep_${DEPENDENCY}_tag "Git tag for ${DEPENDENCY}" ${TAG})

    option(${PROJECT_NAME}_use_external_${DEPENDENCY} "Use external ${DEPENDENCY} from ${PROJECT_NAME}_external_${DEPENDENCY}_path" OFF)
    option_str(${PROJECT_NAME}_external_${DEPENDENCY}_path "External path for ${DEPENDENCY}" "deps/${DEPENDENCY}")
endfunction()

function(depman_make_available DEPENDENCY)
    find_package(${DEPENDENCY} QUIET)
    if(${PROJECT_NAME}_use_external_${DEPENDENCY})
        message("=> Force adding subdirectory ${${PROJECT_NAME}_external_${DEPENDENCY}_path}")
        add_subdirectory(${${PROJECT_NAME}_external_${DEPENDENCY}_path})
    elseif(NOT ${DEPENDENCY}_FOUND)
        message("=> ${DEPENDENCY} not found in system, fetching from repo ${${PROJECT_NAME}_dep_${DEPENDENCY}_repo}")
        include(FetchContent)
        FetchContent_Declare(
            ${DEPENDENCY}
            GIT_REPOSITORY ${${PROJECT_NAME}_dep_${DEPENDENCY}_repo}
            GIT_TAG ${${PROJECT_NAME}_dep_${DEPENDENCY}_tag}
        )
        FetchContent_MakeAvailable(${DEPENDENCY})
    else()
        message("=> ${DEPENDENCY} found in system, using it instead")
    endif()
endfunction()
