ADD_EXECUTABLE( ${PROJECT_NAME}
	
		"main.cpp"
		"stdafx.cpp"
		"VideoPlayer.cpp"
)
TARGET_INCLUDE_DIRECTORIES( ${PROJECT_NAME}
	PRIVATE
		${PROJECT_SOURCE_DIR}/include
)

TARGET_LINK_LIBRARIES( ${PROJECT_NAME}
	PRIVATE
		"avcodec.lib"
		"avformat.lib"
		"avutil.lib" 
		"avdevice.lib"
		"avfilter.lib"
		"postproc.lib"
		"swresample.lib"
		"swscale.lib"
		"SDL2.lib"
)