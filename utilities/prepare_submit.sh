#!/bin/bash

# SCRIPT CREADO PARA LA AUTOMATIZACIÓN DE LAS ENTREGAS Y EJECUCIÓN DE LOS TEST DE LAS PŔACTICAS DE FSIV
# Autor: Carlos Lucena

# Colores
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
BOLD='\033[1m'
PURPLE='\033[0;34m'
NC='\033[0m' # Sin color

# Verificar si se pasaron los 3 argumentos
if [ "$#" -ne 3 ]; then
    echo -e "Uso: $0 <ruta_carpeta_ej1> <ruta_carpeta_ej2> <login_uco>"
    exit 1
fi

# VARIABLES
folder_ej1="$1"
folder_ej2="$2"
folder_dest="$3"
output_zip="${folder_dest}.zip"

name_ej1="$(basename "$folder_ej1")"
name_ej2="$(basename "$folder_ej2")"

name_executable_test="test_common_code"

# FUNCIONES

# Función para eliminar las carpetas build/ y/o data/
remove_build_data_from_temp() {
    local folder="$1"
    if [ -d "$folder/build" ]; then
        rm -rf "$folder/build"
        echo -e "${BOLD}* Eliminado directorio $folder/build${NC}"
    fi
    if [ -d "$folder/data" ]; then
        rm -rf "$folder/data"
        echo -e "${BOLD}* Eliminado directorio $folder/data${NC}"
    fi
}

# Función para compilar un proyecto
compile_project() {
    local project_dir="$1"
    local project_name="$(basename "$project_dir")"

    echo -e "${BLUE}${BOLD}\nCompilando $project_name...${NC}"

    cd "$project_dir"
    mkdir "build" && cd "build" && cmake .. && make

    # Si falla la compilación, retornar un error
    if [ $? -ne 0 ]; then
        echo -e "${RED}\nError: Falló la compilación de $project_name${NC}"
        cd ../..
        return 1
    fi

    # Volver al directorio raíz
    cd ../..
    return 0
}




echo -e "${PURPLE}--------------------------------${NC}"
echo -e "${BOLD}${PURPLE}CREACIÓN ZIP${NC}"
echo -e "${PURPLE}--------------------------------${NC}"

# Verificar que las subcarpetas existan
if [ ! -d "$folder_ej1" ]; then
    echo -e "${RED}Error: No existe el directorio $folder_ej1${NC}"
    exit 1
fi

if [ ! -d "$folder_ej2" ];then
    echo -e "${RED}Error: No existe el directorio $folder_ej2${NC}"
    exit 1
fi

# Verifica que las carpetas tengan nombres diferentes
if [ $folder_ej1 == $folder_ej2 ]; then
    echo -e "${RED}Error: Se han elegido los mismos directorios${NC}"
    exit 1
fi

# Verificar si ya existe el archivo .zip y eliminarlo
if [ -f "$output_zip" ]; then
    echo -e "${YELLOW}Aviso: $output_zip ya existe. Se eliminará${NC}"
    rm "$output_zip"
fi

# Verificar si ya existe la carpeta temporal y eliminarla
if [ -d "$folder_dest" ]; then
    echo -e "${YELLOW}Aviso: $folder_dest/ ya existe. Se eliminará${NC}"
    rm -rf "$folder_dest"
fi

# Crear la carpeta temporal que contendrá las dos carpetas
mkdir "$folder_dest"
printf "\nCreada carpeta $folder_dest/\n\n"

# Copiar las carpetas en la carpeta temporal
cp -r "$folder_ej1" "$folder_dest/"
cp -r "$folder_ej2" "$folder_dest/"

echo -e "${GREEN}Copiado $folder_ej1 a $folder_dest/${NC}"
echo -e "${GREEN}Copiado $folder_ej2 a $folder_dest/${NC}"

echo -e

# Llamar a la función para eliminar carpetas build o data en la carpeta temporal
remove_build_data_from_temp "$folder_dest/$name_ej1"
remove_build_data_from_temp "$folder_dest/$name_ej2"

echo -e "${BLUE}${BOLD}\nContenido del zip: ${NC}\n"

tree $folder_dest

# Crear el archivo .zip sin mostrar salida
zip -r "$output_zip" "$folder_dest" > /dev/null 2>&1

# Eliminar la carpeta temporal
rm -rf "$folder_dest"

echo -e "${GREEN}${BOLD}\nSe ha creado $output_zip exitosamente${NC}\n"



echo -e "${PURPLE}--------------------------------${NC}"
echo -e "${BOLD}${PURPLE}COMPILACIÓN${NC}"
echo -e "${PURPLE}--------------------------------${NC}"

unzip "$output_zip" > /dev/null 2>&1
cd $folder_dest

# Variables para guardar el estado de la compilación
compile_subfolder1_success=true
compile_subfolder2_success=true

# Compilar los dos proyectos
compile_project "./$(basename "$folder_ej1")"
if [ $? -ne 0 ]; then
    compile_subfolder1_success=false
fi

compile_project "./$(basename "$folder_ej2")"
if [ $? -ne 0 ]; then
    compile_subfolder2_success=false
fi




echo -e "\n${PURPLE}--------------------------------${NC}"
echo -e "${BOLD}${PURPLE}EJECUCIÓN TESTS${NC}"
echo -e "${PURPLE}--------------------------------${NC}"

# Ejecutar test solo si la compilación fue exitosa
echo -e "${YELLOW}\nEJERCICIO 1: $name_ej1${NC}\n"

if [ "$compile_subfolder1_success" = true ]; then
    ./"$name_ej1"/build/"$name_executable_test"
else
    echo -e "${RED}Test omitidos debido a errores de compilación.${NC}"
fi

echo -e "${YELLOW}\n\nEJERCICIO 2: $name_ej2${NC}\n"

if [ "$compile_subfolder2_success" = true ]; then
    ./"$name_ej2"/build/"$name_executable_test"
else
    echo -e "${RED}Test omitidos debido a errores de compilación.${NC}"
fi
