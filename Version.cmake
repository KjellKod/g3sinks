set( MAJOR_VERSION 2)
SET(MINOR_VERSION 1)
if (${VERSION}.x STREQUAL ".x")
   if (MSVC)
      message("windows: Extracting git software version")
      execute_process(COMMAND powershell "(git rev-list --branches HEAD | Measure-Object -line).Lines" OUTPUT_VARIABLE GIT_VERSION WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
   else()
      if(UNIX OR ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
         message("nix: Extracting git software version")
      else()
         message("unknown platform: extracting git software version")
      endif()
      execute_process(COMMAND bash "-c" "git rev-list --branches HEAD | wc -l | tr -d ' ' | tr -d '\n'" OUTPUT_VARIABLE GIT_VERSION WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
   endif() 
   # TODO: Kjell, remove this after evaluation
   # math(EXPR VERSION-BASE ${GIT_VERSION}/255)
   # math(EXPR VERSION-REMAINDER ${GIT_VERSION}%255)
   # SET(BUILD_NUMBER ${VERSION-BASE})
   SET(BUILD_NUMBER ${GIT_VERSION})
   SET(VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${BUILD_NUMBER})
endif()
message("Software Version: ${VERSION}")