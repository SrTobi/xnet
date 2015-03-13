buildx_auto_group("${XNET_PUBLIC_INCLUDES}")

macro(add_sample SAMPLE_NAME)

	################### scan for source ##################

	buildx_scan(${SAMPLE_NAME}_SAMPLE_SOURCE ${SAMPLE_NAME} "cpp;hpp")
	buildx_debug("'${SAMPLE_NAME} sample'-source: ${MULTICASTER_SAMPLE_SOURCE}" proj)
	buildx_auto_group("${${SAMPLE_NAME}_SAMPLE_SOURCE}")

	add_executable(${SAMPLE_NAME}-sample ${${SAMPLE_NAME}_SAMPLE_SOURCE} ${XNET_PUBLIC_INCLUDES})
	target_link_libraries(${SAMPLE_NAME}-sample xnet)
	if(WIN32)
		target_compile_definitions(${SAMPLE_NAME}-sample PRIVATE "_WIN32_WINNT=0x0501")
	endif(WIN32)

	buildx_copy_target_dependencies(${SAMPLE_NAME}-sample)
endmacro(add_sample)




add_sample(multicaster)
add_sample(network_interface_lister)