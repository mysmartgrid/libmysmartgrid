# -*- mode: cmake; -*-
# locates the nacl library
# This file defines:
# * NACL_FOUND if nacl was found
# * NACL_LIBRARY The lib to link to (currently only a static unix lib) 
# * NACL_INCLUDE_DIR

if (NOT NACL_FIND_QUIETLY)
  message(STATUS "FindNACL check")
endif (NOT NACL_FIND_QUIETLY)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  include(FindPackageHandleStandardArgs)

  if (NOT WIN32)
    include(FindPkgConfig)
    if ( PKG_CONFIG_FOUND OR NOT ${PKG_CONFIG_EXECUTABLE} STREQUAL "PKG_CONFIG_EXECUTABLE-NOTFOUND")

      set(NACL_DEFINITIONS ${PC_NACL_CFLAGS_OTHER})
      message(STATUS "==> '${PC_NACL_CFLAGS_OTHER}'")
    else(PKG_CONFIG_FOUND OR NOT ${PKG_CONFIG_EXECUTABLE} STREQUAL "PKG_CONFIG_EXECUTABLE-NOTFOUND")
      message(STATUS "==> N '${PC_NACL_CFLAGS_OTHER}'")
    endif(PKG_CONFIG_FOUND OR NOT ${PKG_CONFIG_EXECUTABLE} STREQUAL "PKG_CONFIG_EXECUTABLE-NOTFOUND")
  endif (NOT WIN32)

  #
  # set defaults
  set(_nacl_HOME "/usr/local")
  set(_nacl_INCLUDE_SEARCH_DIRS
    ${CMAKE_INCLUDE_PATH}
    /usr/local/include
    /usr/local/opt/nacl/include
    /usr/include
    )

  set(_nacl_LIBRARIES_SEARCH_DIRS
    ${CMAKE_LIBRARY_PATH}
    /usr/local/lib
    /usr/local/opt/nacl/lib
    /usr/lib
    )

  ##
  if( "${NACL_HOME}" STREQUAL "")
    if("" MATCHES "$ENV{NACL_HOME}")
      message(STATUS "NACL_HOME env is not set, setting it to /usr/local")
      set (NACL_HOME ${_nacl_HOME})
    else("" MATCHES "$ENV{NACL_HOME}")
      set (NACL_HOME "$ENV{NACL_HOME}")
    endif("" MATCHES "$ENV{NACL_HOME}")
  else( "${NACL_HOME}" STREQUAL "")
    message(STATUS "NACL_HOME is not empty: \"${NACL_HOME}\"")
  endif( "${NACL_HOME}" STREQUAL "")
  ##

  message(STATUS "Looking for nacl in ${NACL_HOME}")

  if( NOT ${NACL_HOME} STREQUAL "" )
    set(_nacl_INCLUDE_SEARCH_DIRS ${NACL_HOME}/include ${_nacl_INCLUDE_SEARCH_DIRS})
    set(_nacl_LIBRARIES_SEARCH_DIRS ${NACL_HOME}/lib ${_nacl_LIBRARIES_SEARCH_DIRS})
    set(_nacl_HOME ${NACL_HOME})
  endif( NOT ${NACL_HOME} STREQUAL "" )

  if( NOT $ENV{NACL_INCLUDEDIR} STREQUAL "" )
    set(_nacl_INCLUDE_SEARCH_DIRS $ENV{NACL_INCLUDEDIR} ${_nacl_INCLUDE_SEARCH_DIRS})
  endif( NOT $ENV{NACL_INCLUDEDIR} STREQUAL "" )

  if( NOT $ENV{NACL_LIBRARYDIR} STREQUAL "" )
    set(_nacl_LIBRARIES_SEARCH_DIRS $ENV{NACL_LIBRARYDIR} ${_nacl_LIBRARIES_SEARCH_DIRS})
  endif( NOT $ENV{NACL_LIBRARYDIR} STREQUAL "" )

  if( NACL_HOME )
    set(_nacl_INCLUDE_SEARCH_DIRS ${NACL_HOME}/include ${_nacl_INCLUDE_SEARCH_DIRS})
    set(_nacl_LIBRARIES_SEARCH_DIRS ${NACL_HOME}/lib ${_nacl_LIBRARIES_SEARCH_DIRS})
    set(_nacl_HOME ${NACL_HOME})
  endif( NACL_HOME )

  # find the include files
  find_path(NACL_INCLUDE_DIR nacl/crypto_sign.h
    HINTS
    ${_nacl_INCLUDE_SEARCH_DIRS}
    ${PC_NACL_INCLUDEDIR}
    ${PC_NACL_INCLUDE_DIRS}
    ${CMAKE_INCLUDE_PATH}
    )
  message("==> NACL_INCLUDE_DIR='${NACL_INCLUDE_DIR}'")

  # locate the library
  if(WIN32)
    set(NACL_LIBRARY_NAMES ${NACL_LIBRARY_NAMES} libnacl.lib)
    set(NACL_STATIC_LIBRARY_NAMES ${NACL_LIBRARY_NAMES} libnacl.lib)
  else(WIN32)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      # On MacOS
      set(NACL_LIBRARY_NAMES ${NACL_LIBRARY_NAMES} libnacl.dylib)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
      # On Linux
      set(NACL_LIBRARY_NAMES ${NACL_LIBRARY_NAMES} libnacl.so)
    else()
      set(NACL_LIBRARY_NAMES ${NACL_LIBRARY_NAMES} libnacl.a)
    endif()
    set(NACL_STATIC_LIBRARY_NAMES ${NACL_STATIC_LIBRARY_NAMES} libnacl.a)
  endif(WIN32)

  if( PC_NACL_STATIC_LIBRARIES )
    foreach(lib ${PC_NACL_STATIC_LIBRARIES})
      string(TOUPPER ${lib} _NAME_UPPER)

      find_library(NACL_${_NAME_UPPER}_LIBRARY NAMES "lib${lib}.a"
	HINTS
	${_nacl_LIBRARIES_SEARCH_DIRS}
	${PC_NACL_LIBDIR}
	${PC_NACL_LIBRARY_DIRS}
	)
      #list(APPEND NACL_LIBRARIES ${_dummy})
    endforeach()
    set(_NACL_LIBRARIES "")
    set(_NACL_STATIC_LIBRARIES "")
    set(_NACL_SHARED_LIBRARIES "")
    foreach(lib ${PC_NACL_STATIC_LIBRARIES})
      if ( ${lib} STREQUAL "gnutls" )
	include(FindGnutls)
	set(_NACL_LIBRARIES ${_NACL_LIBRARIES} ${GNUTLS_LIBRARIES})
      #elseif ( ${lib} STREQUAL "ldap" )
	#include(FindLdap)
	#set(_NACL_LIBRARIES ${_NACL_LIBRARIES} ${LDAP_LIBRARIES})
      else()
	string(TOUPPER ${lib} _NAME_UPPER)
	if( NOT ${NACL_${_NAME_UPPER}_LIBRARY} STREQUAL "NACL_${_NAME_UPPER}_LIBRARY-NOTFOUND" )
	  set(_NACL_LIBRARIES ${_NACL_LIBRARIES} ${NACL_${_NAME_UPPER}_LIBRARY})
	  set(_NACL_STATIC_LIBRARIES ${_NACL_STATIC_LIBRARIES} ${NACL_${_NAME_UPPER}_LIBRARY})
	  #set(_NACL_LIBRARIES ${_NACL_LIBRARIES} -l${lib})
	else( NOT ${NACL_${_NAME_UPPER}_LIBRARY} STREQUAL "NACL_${_NAME_UPPER}_LIBRARY-NOTFOUND" )
	  set(_NACL_LIBRARIES ${_NACL_LIBRARIES} -l${lib})
	  set(_NACL_SHARED_LIBRARIES ${_NACL_SHARED_LIBRARIES} -l${lib})
	endif( NOT ${NACL_${_NAME_UPPER}_LIBRARY} STREQUAL "NACL_${_NAME_UPPER}_LIBRARY-NOTFOUND" )
      endif()
    endforeach()
    # set(NACL_LIBRARIES ${PC_NACL_STATIC_LDFLAGS} CACHE FILEPATH "")
    # set(NACL_LIBRARIES ${_NACL_LIBRARIES} CACHE FILEPATH "")
    #set(NACL_STATIC_LIBRARIES ${_NACL_STATIC_LIBRARIES} )#CACHE FILEPATH "")
    set(NACL_SHARED_LIBRARIES ${_NACL_SHARED_LIBRARIES} )#CACHE FILEPATH "")
  endif( PC_NACL_STATIC_LIBRARIES )
  message("==> NACL_LIBRARIES='${NACL_LIBRARIES}'")

  message("==> NACL_LIBRARY_NAMES='${NACL_LIBRARY_NAMES}'")
  message("==> NACL_STATIC_LIBRARY_NAMES='${NACL_STATIC_LIBRARY_NAMES}'")

  # message("Looking for ${NACL_LIBRARY_NAMES} in location ${_nacl_LIBRARIES_SEARCH_DIRS}")
  find_library(NACL_LIBRARIES NAMES ${NACL_LIBRARY_NAMES}
    HINTS
    ${_nacl_LIBRARIES_SEARCH_DIRS}
    ${PC_NACL_LIBDIR}
    ${PC_NACL_LIBRARY_DIRS}
    )

  # message("Looking for ${NACL_STATIC_LIBRARY_NAMES} in location ${_nacl_LIBRARIES_SEARCH_DIRS}")
  find_library(NACL_STATIC_LIBRARIES NAMES ${NACL_STATIC_LIBRARY_NAMES}
    HINTS
    ${_nacl_LIBRARIES_SEARCH_DIRS}
    ${PC_NACL_LIBDIR}
    ${PC_NACL_LIBRARY_DIRS}
    )

  message("==> NACL_LIBRARIES='${NACL_LIBRARIES}'")
  message("==> NACL_STATIC_LIBRARIES='${NACL_STATIC_LIBRARIES}'")
  message("==> NACL_SHARED_LIBRARIES='${NACL_SHARED_LIBRARIES}'")
  message("==> NACL_SHARED_LIBRARY='${NACL_SHARED_LIBRARY}'")

  # find_package_handle_standard_args(NACL DEFAULT_MSG NACL_LIBRARIES NACL_SHARED_LIBRARIES NACL_STATIC_LIBRARIES NACL_INCLUDE_DIR)
  find_package_handle_standard_args(NACL DEFAULT_MSG NACL_LIBRARIES NACL_INCLUDE_DIR)

else(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  set(NACL_FOUND true)
  set(NACL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/nacl ${CMAKE_BINARY_DIR}/nacl)
  set(NACL_LIBRARY_DIR "")
  set(NACL_LIBRARY nacl)
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
