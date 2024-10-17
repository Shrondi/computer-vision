#!/bin/bash

# Colores
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
BOLD='\033[1m'
PURPLE='\033[0;34m'
NC='\033[0m' # Sin color

# Verificar si se pasaron al menos 2 argumentos
if [ "$#" -lt 2 ]; then
    echo -e "Uso: $0 <ruta_carpeta_ej1> [<ruta_carpeta_ej2>] <login_uco>"
    exit 1
fi

# VARIABLES
folder_ej1="$1"
folder_ej2="$2"
folder_dest="${@: -1}" # último argumento
output_zip="${folder_dest}.zip"
folders_to_zip=("$folder_ej1")

# Si hay una segunda carpeta, agregarla a la lista
[ "$#" -eq 3 ] && folders_to_zip+=("$folder_ej2")

# Nombres de las carpetas
name_ej1="$(basename "$folder_ej1")"
name_ej2="$(basename "$folder_ej2")"
name_executable_test="test_common_code"

# FUNCIONES

# Función para eliminar carpetas build/ y data/
remove_build_data_from_temp() {
    local folder="$1"
    for dir in build data; do
        [ -d "$folder/$dir" ] && rm -rf "$folder/$dir" && echo -e "${BOLD}* Eliminado $folder/$dir${NC}"
    done
}

# Función para compilar un proyecto
compile_project() {
    local project_dir="$1"
    local project_name="$(basename "$project_dir")"
    echo -e "${BLUE}${BOLD}\nCompilando $project_name...${NC}"
    cd "$project_dir"
    mkdir -p build && cd build && cmake .. && make || {
        echo -e "${RED}Error: Falló la compilación de $project_name${NC}"
        return 1
    }
    cd ../..
}

# Crear archivo ZIP
prepare_zip() {
    echo -e "${PURPLE}--------------------------------${NC}"
    echo -e "${BOLD}${PURPLE}CREACIÓN ZIP${NC}"
    echo -e "${PURPLE}--------------------------------${NC}"

    # Verificar que las carpetas existan
    for folder in "${folders_to_zip[@]}"; do
        [ ! -d "$folder" ] && echo -e "${RED}Error: No existe el directorio $folder${NC}" && exit 1
    done

    # Limpiar archivos previos
    [ -f "$output_zip" ] && echo -e "${YELLOW}Aviso: $output_zip ya existe. Se eliminará${NC}" && rm "$output_zip"
    [ -d "$folder_dest" ] && echo -e "${YELLOW}Aviso: $folder_dest/ ya existe. Se eliminará${NC}" && rm -rf "$folder_dest"

    # Preparar la carpeta y copiar los proyectos
    mkdir "$folder_dest"
    for folder in "${folders_to_zip[@]}"; do
        cp -r "$folder" "$folder_dest/"
        echo -e "${GREEN}Copiado $folder a $folder_dest/${NC}"
    done

    # Limpiar carpetas build/ y data/
    for folder in "${folders_to_zip[@]}"; do
        remove_build_data_from_temp "$folder_dest/$(basename "$folder")"
    done

    # Crear el archivo ZIP
    echo -e "${BLUE}${BOLD}\nContenido del zip: ${NC}\n"
    tree "$folder_dest"
    zip -r "$output_zip" "$folder_dest" > /dev/null 2>&1
    rm -rf "$folder_dest"
    echo -e "${GREEN}${BOLD}\nSe ha creado $output_zip exitosamente${NC}\n"
}

# Función para compilar proyectos y ejecutar tests
compile_and_run_tests() {
    echo -e "${PURPLE}--------------------------------${NC}"
    echo -e "${BOLD}${PURPLE}COMPILACIÓN Y TESTS${NC}"
    echo -e "${PURPLE}--------------------------------${NC}"

    unzip "$output_zip" > /dev/null 2>&1
    cd "$folder_dest" || exit

    for folder in "${folders_to_zip[@]}"; do
        local project_name="$(basename "$folder")"
        echo -e "${YELLOW}\nEJERCICIO: $project_name${NC}\n"
        compile_project "$project_name" && "./$project_name/build/$name_executable_test" || {
            echo -e "${RED}Test omitidos debido a errores de compilación.${NC}"
        }
    done
}

# Ejecutar funciones
prepare_zip
compile_and_run_tests

