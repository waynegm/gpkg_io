include(FetchContent)

FetchContent_Declare(
    libgpkg
    GIT_REPOSITORY	https://github.com/luciad/libgpkg.git
    GIT_TAG		9f90a9330d75fd41ff6a857c28f9865c1aa18b7b
)
FetchContent_MakeAvailable( libgpkg )
