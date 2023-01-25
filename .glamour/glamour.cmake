cmake_minimum_required(VERSION 3.19)

### glamour.cmake is referenced by all projects using the glamour Loader

function(clone_using_git URL GIT_DIR BRANCH)
  set(GIT_CLONE_COMMAND ${GIT_EXECUTABLE})
  list(APPEND GIT_CLONE_COMMAND "clone")

  if(NOT EXISTS ${GIT_DIR})
    if(DEFINED ENV{ADO_TOKEN})
      list(APPEND GIT_CLONE_COMMAND "-c")
      list(APPEND GIT_CLONE_COMMAND "http.extraheader=\"Basic${B64_ADO_TOKEN}\"")
    endif()

    list(APPEND GIT_CLONE_COMMAND "-b")
    list(APPEND GIT_CLONE_COMMAND ${BRANCH})
    list(APPEND GIT_CLONE_COMMAND ${URL})
    list(APPEND GIT_CLONE_COMMAND ${GIT_DIR})
    
    execute_process(
      COMMAND ${GIT_CLONE_COMMAND}
    )
  else()
    set(GIT_PULL_COMMAND ${GIT_EXECUTABLE})
    list(APPEND GIT_PULL_COMMAND "pull")
    list(APPEND GIT_PULL_COMMAND "origin")
    list(APPEND GIT_PULL_COMMAND ${BRANCH})

    execute_process(
      COMMAND ${GIT_PULL_COMMAND}
      WORKING_DIRECTORY ${GIT_DIR}
    )
  endif()
endfunction()

function(checkout_using_git GIT_DIR BRANCH SHA)
  set(GIT_CHECKOUT_COMMAND ${GIT_EXECUTABLE})
  list(APPEND GIT_CHECKOUT_COMMAND "checkout")
  list(APPEND GIT_CHECKOUT_COMMAND "-B")
  list(APPEND GIT_CHECKOUT_COMMAND ${BRANCH})
  list(APPEND GIT_CHECKOUT_COMMAND ${SHA})

  execute_process(
    COMMAND ${GIT_CHECKOUT_COMMAND}
    WORKING_DIRECTORY ${GIT_DIR}
  )
endfunction()

# Wrap packages for source are always local
if(IS_WRAP)
  set(IS_LOCAL ON)
endif()

# Setup glamour cloning
#  Get glamour json data
set(GLAMOUR_JSON ${CMAKE_SOURCE_DIR}/.glamour/glamour.json)

if(NOT EXISTS ${GLAMOUR_JSON})
  message(FATAL_ERROR "${GLAMOUR_JSON} does not exist")
endif()

file(READ ${GLAMOUR_JSON} GLAMOUR_JSON_DATA)

# Read url from glamour json data
if(DEFINED ADO_TOKEN)
  string(JSON GLAMOUR_URL GET ${GLAMOUR_JSON_DATA} glamour url https)
else()
  string(JSON GLAMOUR_URL GET ${GLAMOUR_JSON_DATA} glamour url ssh)
endif()

# Read sha from glamour json data
string(JSON GLAMOUR_SHA GET ${GLAMOUR_JSON_DATA} glamour sha)

# Read path from glamour json data
set(GLAMOUR_PATH_IDENTIFIER .glamour/${GLAMOUR_SHA})

if(IS_LOCAL)
  set(GLAMOUR_PATH ${CMAKE_BINARY_DIR}/.generated/${GLAMOUR_PATH_IDENTIFIER})
else()
  if(WIN32)
    set(GLAMOUR_BASE_PATH $ENV{HOMEDRIVE})
  else()
    set(GLAMOUR_BASE_PATH "$ENV{HOME}")
  endif()

  set(GLAMOUR_PATH ${GLAMOUR_BASE_PATH}/${GLAMOUR_PATH_IDENTIFIER})
endif()

# Read branch from glamour json data
string(JSON GLAMOUR_BRANCH GET ${GLAMOUR_JSON_DATA} glamour branch)

# Clone glamour
clone_using_git(${GLAMOUR_URL} ${GLAMOUR_PATH} ${GLAMOUR_BRANCH})
checkout_using_git(${GLAMOUR_PATH} ${GLAMOUR_BRANCH} ${GLAMOUR_SHA})
