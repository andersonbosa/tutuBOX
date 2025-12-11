#!/bin/bash
# Script para remover headers de licença dos arquivos
# Uso: ./remove_license_headers.sh [--dry-run]

set -e

DRY_RUN=false
if [[ "$1" == "--dry-run" ]]; then
    DRY_RUN=true
    echo "🔍 Modo DRY-RUN: nenhum arquivo será modificado"
    echo ""
fi

# Diretório base
BASE_DIR="$(dirname "$0")/tutuBOX"

# Contador
count=0
modified=0

# Padrão 1: Header em 4 linhas (/* ... */ na mesma linha)
# Padrão 2: Header em 5 linhas (*/ em linha separada)

remove_header() {
    local file="$1"
    local temp_file=$(mktemp)
    
    # Detecta qual padrão está presente
    if head -5 "$file" | grep -q "^/\* ____________________________"; then
        # Verifica se é padrão de 4 ou 5 linhas
        if head -4 "$file" | tail -1 | grep -q "\*/"; then
            # Padrão A: 4 linhas
            lines_to_skip=4
        elif head -5 "$file" | tail -1 | grep -q "^\*/"; then
            # Padrão B: 5 linhas
            lines_to_skip=5
        else
            echo "  ⚠️  Padrão não reconhecido em: $file"
            return 1
        fi
        
        # Remove as linhas do header
        tail -n +$((lines_to_skip + 1)) "$file" > "$temp_file"
        
        # Remove linhas vazias do início do arquivo
        sed -i.bak '/./,$!d' "$temp_file" 2>/dev/null || sed '/./,$!d' "$temp_file" > "${temp_file}.clean" && mv "${temp_file}.clean" "$temp_file"
        
        if [[ "$DRY_RUN" == "true" ]]; then
            echo "  ✓ Removeria $lines_to_skip linhas de: $file"
            rm -f "$temp_file" "${temp_file}.bak"
        else
            mv "$temp_file" "$file"
            rm -f "${temp_file}.bak"
            echo "  ✓ Removido header ($lines_to_skip linhas) de: $file"
        fi
        return 0
    fi
    
    rm -f "$temp_file"
    return 1
}

echo "🗑️  Removendo headers de licença"
echo "================================================"
echo ""

# Processa arquivos .cpp, .h, .ino
while IFS= read -r -d '' file; do
    ((count++))
    if remove_header "$file"; then
        ((modified++))
    fi
done < <(find "$BASE_DIR" -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.ino" \) -print0 2>/dev/null)

echo ""
echo "================================================"
echo "📊 Resumo:"
echo "   Arquivos verificados: $count"
echo "   Arquivos modificados: $modified"

if [[ "$DRY_RUN" == "true" ]]; then
    echo ""
    echo "💡 Execute sem --dry-run para aplicar as mudanças"
fi

