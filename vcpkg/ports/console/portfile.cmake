vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO strinque/console
    REF 00698e42684205c00a5bfd5b373a407c75528b3b
    SHA512 f1421715bc9292137735cd4ad96ff1515bdc18361896fee69f9abc82ac3e089e1b84b2ba9d259f889cd68bbfc0cb8a613b5ea93ac1b1d62c4794309598cfb2cd
    HEAD_REF master
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)
