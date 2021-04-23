#
# CMake script to modify README.md to create simpler README file
#
# This is equivalent to: fgrep -v "[!["
#

set(SourceFile "${PROJ_SOURCE_DIR}/README.md")
set(DestFile "${PROJ_SOURCE_DIR}/README")

# Below is based on https://stackoverflow.com/a/30227627

file(READ ${SourceFile} Contents)

# Set the variable "Esc" to the ASCII value 27 as a special escape value
string(ASCII 27 Esc)

# Turn the contents into a list of strings, each ending with Esc
string(REGEX REPLACE "\n" "${Esc};" ContentsAsList "${Contents}")

unset(ModifiedContents)
foreach(Line ${ContentsAsList})
  if(NOT "${Line}" MATCHES "\\[!\\[")
    # Swap the appended Esc character back out in favour of a line feed
    string(REGEX REPLACE "${Esc}" "\n" Line ${Line})
    set(ModifiedContents "${ModifiedContents}${Line}")
  endif()
endforeach()
file(WRITE ${DestFile} ${ModifiedContents})
