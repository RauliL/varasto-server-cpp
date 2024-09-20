CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(varasto-server CXX)

ADD_SUBDIRECTORY(./ext/cpp-httplib)
ADD_SUBDIRECTORY(./ext/peelo-json)
ADD_SUBDIRECTORY(./ext/peelo-unicode)
ADD_SUBDIRECTORY(./ext/stduuid)

FIND_PACKAGE(httplib)
FIND_PACKAGE(PeeloJson)
FIND_PACKAGE(PeeloResult)
FIND_PACKAGE(PeeloUnicode)
FIND_PACKAGE(stduuid)

ADD_EXECUTABLE(
  varasto-server
  ./src/filesystem-storage.cpp
  ./src/main.cpp
  ./src/server.cpp
  ./src/slug.cpp
  ./src/storage.cpp
  ./src/utils.cpp
)

TARGET_COMPILE_FEATURES(
  varasto-server
  PRIVATE
    cxx_std_17
)

IF(MSVC)
  TARGET_COMPILE_OPTIONS(
    varasto-server
    PRIVATE
      /W4 /WX
  )
ELSE()
  TARGET_COMPILE_OPTIONS(
    varasto-server
    PRIVATE
      -Wall -Werror
  )
ENDIF()

TARGET_INCLUDE_DIRECTORIES(
  varasto-server
  PRIVATE
    ./ext/cpp-httplib
    ./ext/peelo-result/include
    ./ext/stduuid
    ./ext/stduuid/include
)

TARGET_LINK_LIBRARIES(
  varasto-server
  PUBLIC
    httplib
    PeeloJson
    PeeloResult
    PeeloUnicode
    stduuid
)

INSTALL(
  TARGETS
    varasto-server
  RUNTIME DESTINATION
    bin
)
