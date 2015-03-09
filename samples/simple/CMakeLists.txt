
################### scan for source ##################

buildx_scan_here(SIMPLE_SAMPLE_SOURCE "cpp;hpp")
buildx_debug("'simple sample'-source: ${SIMPLE_SAMPLE_SOURCE}" proj)
buildx_auto_group("${SIMPLE_SAMPLE_SOURCE}")
buildx_auto_group("${XNET_PUBLIC_INCLUDES}")

add_executable(simple-sample ${SIMPLE_SAMPLE_SOURCE} ${XNET_PUBLIC_INCLUDES})
target_link_libraries(simple-sample xnet)
buildx_copy_target_dependencies(simple-sample)