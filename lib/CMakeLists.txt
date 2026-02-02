add_library(external_hashes STATIC 
    "hash-library-master/md5.h" 
    "hash-library-master/md5.cpp"
)

target_include_directories(external_hashes PUBLIC "hash-library-master")