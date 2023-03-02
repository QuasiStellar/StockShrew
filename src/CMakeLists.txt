project(StockShrew)

set(HEADER_FILES Abbreviations.h Common.h Console.h Display.h LookupTables.h Phase.h Piece.h Side.h State.h StateRecord.h Type.h)

set(SOURCE_FILES Display.cpp LookupTables.cpp Piece.cpp State.cpp StateRecord.cpp)

add_library(src STATIC ${SOURCE_FILES} ${HEADER_FILES})
