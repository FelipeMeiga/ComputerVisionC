# Biblioteca de Visão Computacional em C

## Introdução
Esta biblioteca fornece funcionalidades básicas de visão computacional em C, incluindo carregamento, manipulação e salvamento de imagens no formato BMP. Com foco em operações como detecção de bordas e desfoque gaussiano, ela é adequada para projetos e aplicações que necessitam de processamento básico de imagens (mais funcionalidades serao adicionadas futuramente, no momento existem 2 somente).

## Instalação
Para usar a biblioteca, inclua os arquivos `compvision.h` e `compvision.c` no seu projeto C.

## Uso da Biblioteca
Para usar a biblioteca, inclua o cabeçalho com `#include "compvision.h"`. As funções principais disponíveis são:

- `loadBMP`: Carrega uma imagem BMP.
- `saveBMP`: Salva uma imagem BMP.
- `applyEdgeDetection`: Aplica a detecção de bordas na imagem.
- `applyGaussianBlur`: Aplica um desfoque gaussiano na imagem.

## Documentação da API

### Estruturas de Dados
- `bmp_file_header_t`: Cabeçalho de arquivo BMP.
- `bmp_bitmap_info_header_t`: Cabeçalho de informações do bitmap BMP.
- `bmp_palette_element_t`: Elemento da paleta de cores BMP.

### Funções
- `int loadBMP(const char *filename, unsigned char **data, int *width, int *height)`: Carrega uma imagem BMP.
- `int saveBMP(const char* filename, const unsigned char *data, int width, int height)`: Salva uma imagem BMP.
- `void applyEdgeDetection(unsigned char* data, int width, int height)`: Aplica detecção de bordas.
- `void applyGaussianBlur(unsigned char* data, int width, int height)`: Aplica desfoque gaussiano.

## Exemplos
Um exemplo básico de como usar a biblioteca:

```c
#include <stdio.h>
#include <stdlib.h>
#include "compvision.h"

int main() {
    const char* input_filename = "input.bmp";
    const char* output_filename = "output.bmp";

    unsigned char* image_data;
    int width, height;

    if (loadBMP(input_filename, &image_data, &width, &height)) {
        printf("Erro ao carregar a imagem '%s'\\n", input_filename);
        return 1;
    }

    applyEdgeDetection(image_data, width, height);

    if (saveBMP(output_filename, image_data, width, height)) {
        printf("Erro ao salvar a imagem '%s'\\n", output_filename);
        free(image_data);
        return 1;
    }

    free(image_data);
    return 0;
}
