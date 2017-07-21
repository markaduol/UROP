add_library(FunctionRenamePass MODULE
    # List your source files here.
    Skeleton.cpp
    FunctionRename.cpp
)

# Use C++11 to compile your pass (i.e., supply -std=c++11).
# 'target_compile_features' not supported in cmake 2.8
# target_compile_features(FunctionRenamePass PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(FunctionRenamePass PROPERTIES
    COMPILE_FLAGS "-fno-rtti -std=c++11"
)

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
if(APPLE)
  set_target_properties(FunctionRenamePass PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)
