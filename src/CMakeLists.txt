# grab source
buildx_scan_here(XNET_SOURCE "cpp;hpp")
buildx_debug("source: ${XNET_SOURCE}" proj)
buildx_auto_group("${XNET_SOURCE}" PREFIX source)
buildx_auto_group("${XNET_PUBLIC_INCLUDES}")

# add library
include_directories(.)
if(WIN32)
	SET(Boost_USE_STATIC_LIBS ON)
endif()


add_library(xnet STATIC ${XNET_SOURCE} ${XNET_PUBLIC_INCLUDES})
target_link_libraries(xnet ${Boost_LIBRARIES})
buildx_export_header(xnet
					DESTINATION_FILE "XNET_PUBLIC_INCLUDES"
					DESTINATION_DIR "XNET_EXPORT_API_DIR"
					SUB_DIR xnet)
if(WIN32)
	target_compile_definitions(xnet PRIVATE "_WIN32_WINNT=0x0501")
endif(WIN32)

install(TARGETS xnet EXPORT xnetTargets
  LIBRARY DESTINATION lib/${XNET_VERSION_DIR}
  ARCHIVE DESTINATION lib/${XNET_VERSION_DIR}
  RUNTIME DESTINATION bin/${XNET_VERSION_DIR}
)

buildx_target_includes(	TARGETS xnet
						FILES ${XNET_PUBLIC_INCLUDES}
						BASE_DIRECTORIES
							${XNET_PUBLIC_INCLUDE_DIR}
							${XNET_EXPORT_API_DIR}
						DESTINATION include/${XNET_VERSION_DIR})

buildx_add_external_test(external-test ${PROJECT_SOURCE_DIR}/tests/interface TEST_TARGETS xnet)
#buildx_add_internal_test(internal-test ${PROJECT_SOURCE_DIR}/tests/internal TEST_TARGET xnet)