# -*- mode: cmake; -*-

file(GLOB msg_hdrs *.h *.hpp)
install(FILES ${msg_hdrs} DESTINATION include/libmysmartgrid)

include_directories(${CMAKE_SOURCE_DIR}
                    ${CURL_INCLUDE_DIR}
                    ${JSONCPP_INCLUDE_DIR}
                    ${OPENSSL_INCLUDE_DIR}
)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

file(GLOB msg_src *.cpp *.hpp)

ADD_LIBRARY(mysmartgrid STATIC ${msg_src}
  )

target_link_libraries(mysmartgrid
  ${Boost_LIBRARIES}
  ${CURL_LIBRARIES}
  ${JSONCPP_LIBRARY}
  ${OPENSSL_CRYPTO_LIBRARY}
)

install(TARGETS mysmartgrid 
  EXPORT ${PROJECT_NAME}Exports
  ARCHIVE DESTINATION lib
  COMPONENT static-libraries)
