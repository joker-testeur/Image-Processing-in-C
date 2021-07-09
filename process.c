#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array_utility.h"
#include "rotate.h"
#include "resize.h"
#include "histograms.h"
#include "contrast_enhancement.h"
#include "manipulate_hsi.h"
#include "histogram_equalization.h"
#include "kernel_filter.h"
#include "median_filter.h"
#include "gradient_filter.h"
#include "fourier_filter.h"
#include "edge_detection.h"
#include "lodepng.h"

float*** png_to_ppm (unsigned char* image, unsigned width, unsigned height)
{
    float*** img = alloc3df(3, height, width);

    int i, j;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            img[0][i][j] = image[3 * i * width + 3 * j + 0]; // Red.
            img[1][i][j] = image[3 * i * width + 3 * j + 1]; // Green.
            img[2][i][j] = image[3 * i * width + 3 * j + 2]; // Blue.
        }
    }

      return img;
}
unsigned char* ppm_to_png (float*** image, unsigned width, unsigned height)
{
    unsigned char* img = (unsigned char*) malloc(3 * width * height *
        sizeof(unsigned char));

    // On effectue un étirement linéaire (linear stretch) avant la convertion en PNG; c'est 
    // une sorte de normalisation.
    int i, j, k;

    for (k = 0; k < 3; k++) {
        float min = 9e9;
        float max = -min;

        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                float v = image[k][i][j];
                if (min > v) {
                    min = v;
                }
                if (max < v) {
                    max = v;
                }
            }
        }

        float v = 254.99 / (max - min);

        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                img[3 * i * width + 3 * j + k] = (image[k][i][j] - min) * v;
            }
        }
    }

      return img;
}

int main ()
{
    int action = 1;

    char filename[256];
    char saved_filename[256];

    printf("Entrer le nom de l'image PNG à traiter.\n");
    scanf("%s", filename);

    setbuf(stdout, NULL);

    // Load the PNG using LodePNG library.
    printf("Chargement du fichier en memoire...\n\n\n");
    unsigned error;
    unsigned char* image;
    unsigned width, height;

    error = lodepng_decode24_file(&image, &width, &height, filename);
    if (error) {
        printf("Erreur (%u) lors du chargement du fichier: %s\n", error,
            lodepng_error_text(error));
        exit(1);
    }

    // Then use array to load into a three-dimensional array of R, G,
    // and B values.
    float*** input = png_to_ppm(image, width, height);
    free(image);

    int i, j;
    int output_height, output_width;

    // Begin coding the user prompt menu.
    while (action > 0) {
        float*** output = (float ***) malloc(3 * sizeof(float**));

        printf("\n******************************************************************************************************************************\n");
        printf("\n Bienvenue sur l'interface de traitement d'image\n");
        printf("Quel traitement voulez-vous appliquer à %s: \n", filename);
        printf("1) Générer l'histogramme de densité pour l'image.\n");
        printf("2) Générer l'histogramme cumulée pour l'image.\n");
        printf("3) Améliorer le contraste de l'image.\n");
        printf("4) Redimensionner (hauteur,largeur) une image en fonction des facteurs.\n");
        printf("5) Pivoter une image d'un certain angle.\n");
        printf("6) Egalisateur d'histogramme pour l'image.\n");
        printf("7) Pivoter la teinte de l'image.\n");
        printf("8) Augmenter la saturation des couleurs de l'image.\n");
        printf("9) Augmenter l'intensité de l'image.\n");
        printf("10) Filtrage passe-bas à l'aide d'un noyau sur une image.\n");
        printf("11) Filtrage passe-haute à l'aide d'un noyau sur une image.");
        printf("\n");
        printf("12) Filtrage passe-haute utilisant le trasformé de Fourier\n");
        printf("13) Créer une image dégradée qui accentue les changement de");
        printf(" couleur en fonction de l'image donnée.\n");
        printf("14) Nettoyage de l'image à l'aide du filtre median.\n");
        printf("15) Détecter les contours de l'image.\n");
        printf("\n*****************************************************************************************************************\n");

        printf("Entrer le numero de l'action qui vous enterrèse ");
        printf("(-1 pour quitter): ");
        scanf("%d", &action);
        printf("----------------------------------------------------\n\n\n");

        // Act according to what the user selected.
        switch (action) {
            case 1:
                output_height = output_width = 256;
                for (i = 0; i < 3; i++) {
                  output[i] = generate_density_histogram(input[i],
                      output_height, height, width);
                }

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_densité_histogramme.png");
              break;
            case 2:
                output_height = output_width = 256;
                for (i = 0; i < 3; i++) {
                  output[i] = generate_cumulative_histogram(input[i],
                      output_height, height, width);
                }

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_cumu_histogramme.png");
                break;
            case 3:
                output_height = height;
                output_width = width;
                output = enhance_contrast(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_with_contraste.png");
                break;
            case 4:
                output = resize(input, height, width, &output_height,
                   &output_width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_redimensioner.png");
                break;
            case 5:
                output_height = height;
                output_width = width;
                output = rotate(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_pivoter.png");
                break;
            case 6:
                output_height = height;
                output_width = width;
                output = equalize(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_equalization.png");
                break;
            case 7:
                output_height = height;
                output_width = width;
                output = rotate_hue(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_rotation_teinte.png");
                break;
            case 8:
                output_height = height;
                output_width = width;
                output = increase_saturation(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_augmentation_saturation.png");
                break;
            case 9:
                output_height = height;
                output_width = width;
                output = increase_intensity(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_augmentation_intensité.png");
                break;
            case 10:
                output_height = height;
                output_width = width;
                output = lowpass_filter(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_lowpass_filter.png");
                break;
            case 11:
                output_height = height;
                output_width = width;
                output = highpass_filter(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_highpass_filter.png");
                break;
            case 12:
                output_height = height;
                output_width = width;
                output = fourier_filter(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_fourier_filter.png");
                break;
            case 13:
                output_height = height;
                output_width = width;
                output = gradient_filter(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_gradient_image.png");
                break;
            case 14:
                output_height = height;
                output_width = width;
                output = median_filter(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_median_filter.png");
                break;
            case 15:
                output_height = height;
                output_width = width;
                output = detect_edges(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_detected_edges.png");
                break;
            default:
                break;
        }

        if (action > 0) {
            // Save the file and indicate what the file was saved as.
            unsigned char* output_png = ppm_to_png(output, output_width,
                output_height);
            dealloc3df(output, 3, output_height, output_width);
            error = lodepng_encode24_file(saved_filename, output_png,
                output_width, output_height);
            if (error) {
                printf("Error (%u) sauvegarde du fichier: %s\n\n\n", error,
                    lodepng_error_text(error));
            } else {
                printf("le fichier a été bien sauvegarder as %s.\n\n\n",
                    saved_filename);
            }
        }
    }

    dealloc3df(input, 3, height, width);
}
