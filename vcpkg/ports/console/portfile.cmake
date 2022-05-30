vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/console
    REF v1.0.0
    SHA512 545e51668a66765f50886824c0d9d6ba10ece9565c7a475a7b5859838024685ab1e03286dd3f5bbb3959b8a6934b2e0290cc1e2f07126c85c9277902a77f23d1
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)
