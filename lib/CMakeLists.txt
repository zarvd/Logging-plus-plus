set(LIB_SRC Logger.cpp LogHandler.cpp LogStream.cpp)
add_library(logger STATIC ${LIB_SRC})
install(TARGETS logger DESTINATION /usr/local/lib)
