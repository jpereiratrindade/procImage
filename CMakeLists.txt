cmake_minimum_required(VERSION 3.5)

project(procImagem LANGUAGES C)

# Localiza a biblioteca GDAL
find_package(GDAL REQUIRED)

# Localiza a biblioteca libtiff (mantida caso você precise usar diretamente)
find_package(TIFF REQUIRED)

# Adiciona o executável
add_executable(procImagem main.c)

# Liga o executável às bibliotecas GDAL e libtiff
target_link_libraries(procImagem GDAL::GDAL TIFF::TIFF)

# Define os diretórios de instalação
include(GNUInstallDirs)
install(TARGETS procImagem
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
