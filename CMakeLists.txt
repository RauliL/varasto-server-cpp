CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(varasto-server CXX)

FIND_PACKAGE(httplib)
FIND_PACKAGE(nlohmann_json)
FIND_PACKAGE(PeeloResult)
FIND_PACKAGE(stduuid)

ADD_EXECUTABLE(
  varasto-server
  ./src/filesystem-storage.cpp
  ./src/main.cpp
  ./src/server.cpp
  ./src/slug.cpp
  ./src/storage.cpp
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
    ./ext/json/include
    ./ext/peelo-result/include
    ./ext/stduuid
    ./ext/stduuid/include
)

TARGET_LINK_LIBRARIES(
  varasto-server
  PUBLIC
    httplib
    nlohmann_json
    PeeloResult
    stduuid
)

INSTALL(
  TARGETS
    varasto-server
  RUNTIME DESTINATION
    bin
)
