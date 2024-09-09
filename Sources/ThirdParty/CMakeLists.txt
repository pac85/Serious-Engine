cmake_minimum_required(VERSION 3.7.2)
project(ThirdPartyLibs)

include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}/libogg"
  "${CMAKE_CURRENT_SOURCE_DIR}/libogg/src"
  "${CMAKE_CURRENT_SOURCE_DIR}/libvorbis"
  "${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/lib"
)

set(THIRDPARTY_SRCS "")

if(NOT USE_SYSTEM_ZLIB)
  set(THIRDPARTY_SRCS
    ${THIRDPARTY_SRCS}
    "zlib/adler32.c"
    "zlib/compress.c"
    "zlib/deflate.c"
    "zlib/infblock.c"
    "zlib/infcodes.c"
    "zlib/inffast.c"
    "zlib/inflate.c"
    "zlib/inftrees.c"
    "zlib/infutil.c"
    "zlib/trees.c"
    "zlib/uncompr.c"
    "zlib/zutil.c"
  )
endif()

set(THIRDPARTY_SRCS
  ${THIRDPARTY_SRCS}
  "libogg/src/bitwise.c"
  "libogg/src/framing.c"
  "libvorbis/lib/bitrate.c"
  "libvorbis/lib/block.c"
  "libvorbis/lib/codebook.c"
  "libvorbis/lib/envelope.c"
  "libvorbis/lib/floor0.c"
  "libvorbis/lib/floor1.c"
  "libvorbis/lib/info.c"
  "libvorbis/lib/lookup.c"
  "libvorbis/lib/lpc.c"
  "libvorbis/lib/lsp.c"
  "libvorbis/lib/mapping0.c"
  "libvorbis/lib/mdct.c"
  "libvorbis/lib/psy.c"
  #"libvorbis/lib/psytune.c"
  "libvorbis/lib/registry.c"
  "libvorbis/lib/res0.c"
  "libvorbis/lib/sharedbook.c"
  "libvorbis/lib/smallft.c"
  "libvorbis/lib/synthesis.c"
  "libvorbis/lib/tone.c"
  "libvorbis/lib/vorbisenc.c"
  "libvorbis/lib/vorbisfile.c"
  "libvorbis/lib/window.c"
  "libvorbis/lib/analysis.c"
  "libvorbis/lib/barkmel.c"
)

add_library(ThirdPartyLibs STATIC ${THIRDPARTY_SRCS})
set_target_properties(ThirdPartyLibs PROPERTIES ENABLE_EXPORTS ON)
