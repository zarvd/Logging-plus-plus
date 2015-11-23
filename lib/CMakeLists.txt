set(LIB_SRC log_handler.cc log_stream.cc)
add_library(logger STATIC ${LIB_SRC})
install(TARGETS logger DESTINATION /usr/local/lib)
