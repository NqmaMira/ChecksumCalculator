add_library(external_hashes STATIC 
    "hash-library-master/md5.cpp" 
    "hash-library-master/md5.h"
)

target_include_directories(external_hashes PUBLIC "hash-library-master")