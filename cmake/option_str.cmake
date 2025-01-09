# Option, but for string

function(option_str OPTION_NAME OPTION_DESCRIPTION OPTION_DEFAULT)
    if(NOT DEFINED ${OPTION_NAME})
        set(${OPTION_NAME} ${OPTION_DEFAULT} CACHE STRING ${OPTION_DESCRIPTION})
    endif()
endfunction()
