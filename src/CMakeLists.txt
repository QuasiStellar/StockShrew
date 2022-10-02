project(StockShrew)

set(HEADER_FILES Abbreviations.h Common.h Display.h Phase.h Piece.h Side.h State.h Type.h)

set(SOURCE_FILES Display.cpp Piece.cpp State.cpp)

add_library(src STATIC ${SOURCE_FILES} ${HEADER_FILES})
