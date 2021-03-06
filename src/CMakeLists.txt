project(bob_src)

# Global macro definition
# Resolves library locations for the "broken" pkg-config cmake bridge
#
# lib: The library name, like "z"
# path: Library path to search for lib
# l: The list where we should append the resolved library path
#
# Raises a WARNING if it cannot resolve the library using the given
# path. Raises a FATAL_ERROR if it cannot resolve the library path even
# using the standard cmake search paths.
macro(resolve_library library dirs l)
  if(${dirs})
    foreach(dir "${dirs}")
      find_library(newlib ${library} NO_DEFAULT_PATH NO_CMAKE_ENVIRONMENT_PATH HINTS ${dir})
      if (newlib)
        break()
      endif()
    endforeach()

    if(NOT newlib)
      message(WARNING "Could not resolve library path for 'lib${library}' using '${dirs}'. Trying with the system paths...")
    endif()
  endif()

  if(NOT newlib)
    foreach(dir "${dirs}")
      find_library(newlib ${library} HINTS ${dir})
      if (newlib)
        break()
      endif()
    endforeach()
  endif()

  if(NOT newlib)
    message(WARNING "Could not resolve library path for 'lib${library}' using '${dirs}' or cmake's standard paths. Stopping here.")
  endif()

  # if you survived to this point, just append.
  list(APPEND ${l} ${newlib})
  unset(newlib CACHE)
endmacro()

add_subdirectory(lbfgs)

# Required externals
include(blitz.cmake)
include(boost.cmake)
set(cxx_incdir ${Blitz_INCLUDE_DIR};${boost_INCLUDE_DIRS})

include(python.cmake)
set(py_incdir ${cxx_incdir};${python_INCLUDE_DIRS})

add_subdirectory(core)
add_subdirectory(python)

add_subdirectory(io)
add_subdirectory(measure)
add_subdirectory(sp)
add_subdirectory(math)
add_subdirectory(ip)
add_subdirectory(ap)

include(libsvm.cmake)
add_subdirectory(machine)
add_subdirectory(trainer)

find_package(Qt4 COMPONENTS QtCore QtGui)

if(QT4_FOUND)
  set(HAVE_QT4 ON CACHE BOOL "Has Qt4 installed")
  add_subdirectory(visioner)

  if (FFMPEG_FOUND) #daq also needs FFMPEG to be present
    add_subdirectory(daq)
  endif()

endif()

set(ENABLED_PACKAGES "${ENABLED_PACKAGES}" PARENT_SCOPE)
