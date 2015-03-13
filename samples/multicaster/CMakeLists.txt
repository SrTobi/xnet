
################### scan for source ##################

buildx_scan_here(MULTICASTER_SAMPLE_SOURCE "cpp;hpp")
buildx_debug("'multicaster sample'-source: ${MULTICASTER_SAMPLE_SOURCE}" proj)
buildx_auto_group("${MULTICASTER_SAMPLE_SOURCE}")
buildx_auto_group("${XNET_PUBLIC_INCLUDES}")

add_executable(multicaster-sample ${MULTICASTER_SAMPLE_SOURCE} ${XNET_PUBLIC_INCLUDES})
target_link_libraries(multicaster-sample xnet)
if(WIN32)
	target_compile_definitions(multicaster-sample PRIVATE "_WIN32_WINNT=0x0501")
endif(WIN32)

buildx_copy_target_dependencies(multicaster-sample)