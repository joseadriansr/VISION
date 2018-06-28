//  ------------------------------------------------------------------------------------------
//      main.c
//      II EXAMEN PROGRAMADO - VISION POR COMPUTADOR
//      Basado en:
//      visionPorComputador_Prog_Ref_No1
//
//      Created by Dr. Geovanni Martinez on 9/27/17.
//      Copyright © 2017 Dr. Geovanni Martinez. All rights reserved.
//
//  ------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//  ------------------------------------------------------------------------------------------
//  PROTOTIPOS DE FUNCIONES
//  ------------------------------------------------------------------------------------------

void geoLeerParametrosDeControlDeArchivoDeTexto();
void readRGBImageFromBMPFile(char *filename);
void geoInsertYourCodeHere();
void geoGetIntensityImageFromRGBimage();
void geoSaveIntensityImageIn_YUV400_file(unsigned char *pIntensity, char* filename);
void geoChangeImageCoordinateSystemFromLowerLeftSideToUpperLeftSide(unsigned char *pIntensity, unsigned char *presult);
int SaveIntensityImageIn_BMP_file(unsigned char *pintensity, char *filename);
int SaveRGBImageIn_BMP_file(unsigned char *prgb, char *filename);

void jsrGetHistogram();
void jsrGetProbability();
void jsrGetThreshold();

double jsrGetVariance1(int th);
double jsrGetMean1(int th);
double jsrGetWeight1(int th);

double jsrGetVariance2(int th);
double jsrGetMean2(int th);
double jsrGetWeight2(int th);

double jsrGetVariancew();

int jsrKittler_th(int th);
int jsrOtsu_th(int th);
int jsrKurita_th(int th);
int jsrMartinez_th(int th);
//  ------------------------------------------------------------------------------------------
//  STRUCTS (CONTENEDORES)
//  ------------------------------------------------------------------------------------------

//Contenedor de imagenes
struct pInputImage
{
    int width;  //ancho de imagenes
    int height; //alto de imagenes
    unsigned char *prgb; //imagen rgb de entrada
    unsigned char *pintensity; //imagen de intensidad
    unsigned char *pthresholdedIntensity; //imagen resultado
};

//Contenedor de parametros de control
struct contenedor_de_parametros_de_control
{
    int width; //ancho de las imagenes
    int height; //alto de las imagenes
    char pathAndInputImageFileName[256]; //directorio de entrada
    char pathOfOutputDirectory[256]; //directorio de salida
};

//Contenedor de resultados
struct contenedor_de_resultados{

    int th_kurita;
    int th_otsu;
    int th_kittler;
    int th_martinez;

    double c1;
    double c2;

    double mean1;
    double mean2;

    double var1;
    double var2;
    double varw;

    int N;

    int hist[256]; //Para almacenar los valores del histograma de la imagen
    double prob[256]; //Para almacenar la probabilidad de cada color

};

//  ------------------------------------------------------------------------------------------
// VARIABLES GLOBALES
//  ------------------------------------------------------------------------------------------

struct contenedor_de_parametros_de_control *p_parametros;
struct pInputImage *pInputImage;
struct contenedor_de_resultados *p_resultados;
int numeroDeDatosLeidos=0;

#define PI 3.141592652

//  ------------------------------------------------------------------------------------------
//  MAIN
//  ------------------------------------------------------------------------------------------
int main()
{
    //VARIABLES LOCALES
    int i;
    int width, height;

    //AUTORIA
    printf("==================================================================\n");
    printf("** II EXAMEN PROGRAMADO                                         **\n");
    printf("** JOSE ADRIAN SANABRIA ROSELLO - B46420                        **\n");
    printf("==================================================================\n");
    printf("** Basado en:                                                   **\n");
    printf("** visionPorComputador_Prog_Ref_No1                             **\n");
    printf("** Calculo de imagen de intensidad y dibujo de figuras basicas  **\n");
    printf("** IE-0449 Vision por Computador                                **\n");
    printf("** Prof. Dr.-Ing. Geovanni Martínez - IPCV-LAB                  **\n");
    printf("** II-2017                                                      **\n");
    printf("** Versión v004, 24 Octubre 2017, 11:30                         **\n");
    printf("==================================================================\n");
    printf("\n");

    //Reservando memoria de contenedor p_resultados
    p_resultados = (struct contenedor_de_resultados *)malloc(sizeof(struct contenedor_de_resultados));

    //Reservando memoria de contenedor p_parametros
    p_parametros = (struct contenedor_de_parametros_de_control *)malloc(sizeof(struct contenedor_de_parametros_de_control));

    //Esta función lee los parámetros de control de flujo del programa desde un archivo de texto y los almacena en el
    //contenedor p_parametros
    geoLeerParametrosDeControlDeArchivoDeTexto();

    //Reservando memoria para la estructura pInputImage
    pInputImage = (struct pInputImage *)malloc(sizeof(struct pInputImage));
    pInputImage->width=p_parametros->width;
    pInputImage->height=p_parametros->height;

    //Reservando e inicializando la memoria de las imagenes del contenedor pInputImage
    width=p_parametros->width;
    height=p_parametros->height;
    pInputImage->prgb = (unsigned char*)malloc(sizeof(unsigned char)*width*height*3);
    pInputImage->pintensity =(unsigned char*)malloc(sizeof(unsigned char)*width*height);
    pInputImage->pthresholdedIntensity =(unsigned char*)malloc(sizeof(unsigned char)*width*height);

    //Cada píxel se inicializa en cero
    for (i=0;i<width*height*3;i++) pInputImage->prgb[i]=0;
    for (i=0;i<width*height;i++) {
        pInputImage->pintensity[i]=0;
        pInputImage->pthresholdedIntensity[i]=0;
    }

    //Leyendo la imagen RGB de archivo en formato BMP
    readRGBImageFromBMPFile(p_parametros->pathAndInputImageFileName);

    //Insertar codigo en esta funcion
    geoInsertYourCodeHere();

    //Liberando memoria de los contenedores e imagenes
    free(pInputImage->prgb);
    free(pInputImage->pintensity);
    free(pInputImage->pthresholdedIntensity);
    free(pInputImage);
    free(p_parametros);

    return 0;
}

//  ------------------------------------------------------------------------------------------
//  FUNCION PARA INSERTAR FUNCIONES
//  ------------------------------------------------------------------------------------------

void geoInsertYourCodeHere()
{
    char pathAndFileName[256];
    strcpy(pathAndFileName,"output/rgb.bmp");
    SaveRGBImageIn_BMP_file(pInputImage->prgb, pathAndFileName);

    //Calculando la imagen de intensidad
    geoGetIntensityImageFromRGBimage();
    p_resultados->N = pInputImage->height*pInputImage->width;

    //Almacenando resultado en archivo en formato BMP
    strcpy(pathAndFileName,"output/imagenDeIntensidad.bmp");
    SaveIntensityImageIn_BMP_file(pInputImage->pintensity, pathAndFileName);

    jsrGetHistogram();

    printf("\nCantidad total de pixeles: %d \n",p_resultados->N);
    printf("\nHistograma: \n");
    int p;
    for(p=0; p<256; p++){
        printf("   h(%d) = %d\n", p, p_resultados->hist[p]);
    }

    jsrGetProbability();
    printf("\nProbabilidad: \n");
    int f;
    for(f=0; f<256; f++){
        printf("   p(%d) = %.10lf\n", f, p_resultados->prob[f]);
    }

}

//  ------------------------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------
//  FUNCIONES NUEVAS CREADAS PARA EL EXAMEN
//  ------------------------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------


//  ------------------------------------------------------------------------------------------
//  FUNCION PARA OBTENER EL HISTOGRAMA DE LA IMAGEN DE INTENSIDAD
//  ------------------------------------------------------------------------------------------
void jsrGetHistogram(){
    int k;
    int q;
    int g;
    for(g=0; g<256; g++){
        p_resultados->hist[g]=0;
    }
    for(k=0; k<p_resultados->N; k++){
        for(q=0; q<256; q++){
            if( pInputImage->pintensity[k] == (unsigned char)q ){
                p_resultados->hist[q]++;
            }
        }
    }
}

//  ------------------------------------------------------------------------------------------
//  FUNCION PARA OBTENER LA PROBABILIDAD p(n) DE CADA ELEMENTO DE LA IMAGEN
//  ------------------------------------------------------------------------------------------
void jsrGetProbability(){
    int k;
    for(k=0; k<256; k++){
        p_resultados->prob[k] = (double)p_resultados->hist[k]/(double)p_resultados->N;
    }
}

//  ------------------------------------------------------------------------------------------
//  FUNCION PARA OBTENER EL UMBRAL OPTIMO th_op MEDIANTE EL METODO DE MARTINEZ
//  ------------------------------------------------------------------------------------------
void jsrGetThreshold(){
    int x;
    for(x=0;x<256;x++){
        jsrKittler_th(x);
    }
}

int jsrKittler_th(int th){

}

//  ------------------------------------------------------------------------------------------
//  FUNCION PARA OBTENER LOS PESOS c1 Y c2 CON EL UMBRAL ENVIADO th
//  ------------------------------------------------------------------------------------------
double jsrGetWeight1(int th){
    int n;
    double c1=0;
    for(n=0; n<=th; n++){
        c1 += p_resultados->prob[n];
    }
    return c1;
}
double jsrGetWeight2(int th){
    int d;
    double c2=0;
    for(d=th+1; d<256; d++){
        c2 += p_resultados->prob[d];
    }
    return c2;
}

//  ------------------------------------------------------------------------------------------
//  FUNCION PARA OBTENER LAS MEDIAS m1 Y m2 CON EL UMBRAL ENVIADO th
//  ------------------------------------------------------------------------------------------
double jsrGetMean1(int th){
    int h;
    double m1=0;
    for(h=0; h<=th; h++){
        m1 += (h*p_resultados->prob[h]);
    }
    m1 = m1/p_resultados->c1;
    return m1;
}
double jsrGetMean2(int th){
    int h;
    double m2=0;
    for(h=th+1; h<256; h++){
        m2 += (h*p_resultados->prob[h]);
    }
    m2 = m2/p_resultados->c2;
    return m2;

}
//  ------------------------------------------------------------------------------------------
//  FUNCION PARA OBTENER LAS VARIANZAS var1 Y var2 CON EL UMBRAL ENVIADO th
//  ------------------------------------------------------------------------------------------
double jsrGetVariance1(int th){
    int h;
    double var1=0;
    for(h=0; h<=th; h++){
        var1 += pow(h-p_resultados->mean1,2)*p_resultados->prob[h];
    }
    var1 = var1/p_resultados->c1;
    return var1;
}
double jsrGetVariance2(int th){
    int h;
    double var2=0;
    for(h=th; h<256; h++){
        var2 += pow(h-p_resultados->mean2,2)*p_resultados->prob[h];
    }
    var2 = var2/p_resultados->c2;
    return var2;
}


//  ------------------------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------
//  OBTENER IMAGEN DE INTENSIDAD DE UNA IMAGEN RGB
//  ------------------------------------------------------------------------------------------
void geoGetIntensityImageFromRGBimage()
{
    int i,j;
    int width, height;

    //Renombrando para facilitar código
    width= pInputImage->width;
    height=pInputImage->height;

    //Calculando la imagen de intensidad. El resultado será almacenado
    //en el espacio que fue alocado para tal fin en nuestra estructura
    //pInputImage
    for (j=0;j<height; j++) {
        for (i=0;i<width; i++) {
            pInputImage->pintensity[j*width+i] =
            (unsigned char)(0.299*(double)pInputImage->prgb[3*j*width+3*i]+
                            0.587*(double)pInputImage->prgb[3*j*width+3*i+1]+
                            0.114*(double)pInputImage->prgb[3*j*width+3*i+2]);
        }
    }
}

//  ------------------------------------------------------------------------------------------
//  LECTURA DE PARAMETROS DE CONTROL
//  ------------------------------------------------------------------------------------------
void geoLeerParametrosDeControlDeArchivoDeTexto()
{
    FILE *archivo;
    char d1[256], d2[256], d3[256];
    int res;

    printf("Leyendo los datos de entrada:\n");

    //Abriendo archivo en mode de lectura
    char nombreDeArchivo[256]="current_control_parameters.txt";
    archivo = fopen(nombreDeArchivo, "r");
    if (!archivo) {
        printf("No se pudo abrir el archivo: current_control_parameters.txt\n");
        exit(1);
        }

    //Leyendo datos linea por linea
    //Brincando la primera y segunda lineas
    res=fscanf(archivo, "%s %s %s\n", d1, d2, d3);
    if (res==0) {printf("Error leyendo dato No. %d de archivo de parametros de control\n", numeroDeDatosLeidos); exit(0);}
    res=fscanf(archivo, "\n");
    numeroDeDatosLeidos++;

    printf("  Dimensiones de las imagenes\n");

    //Leyendo width
    res=fscanf(archivo, "%s %s\n", d1, d2);
    if (res==0) {printf("Error leyendo dato No. %d de archivo de parametros de control\n", numeroDeDatosLeidos); exit(0);}
    p_parametros->width=(int)atof(d2);
    printf("   width: %d\n", p_parametros->width);
    numeroDeDatosLeidos++;

    //Leyendo height
    res=fscanf(archivo, "%s %s\n", d1, d2);
    if (res==0) {printf("Error leyendo dato No. %d de archivo de parametros de control\n", numeroDeDatosLeidos); exit(0);}
    p_parametros->height=(int)atof(d2);
    printf("   height: %d\n", p_parametros->height);
    numeroDeDatosLeidos++;

    //Brincando linea de texto
    res=fscanf(archivo, "\n");

    printf("  Imagen de entrada y directorio de salida\n");

    //Leyendo path y nombre de imagen de entrada
    res=fscanf(archivo, "%s %s %s\n", d1, d2, d3);
    if (res==0) {printf("Error leyendo dato No. %d de archivo de parametros de control\n", numeroDeDatosLeidos); exit(0);}
    strcpy(p_parametros->pathAndInputImageFileName,d3);
    printf("   imagen de entrada: %s\n", p_parametros->pathAndInputImageFileName);
    numeroDeDatosLeidos++;

    //Leyendo directorio de salida
    res=fscanf(archivo, "%s %s %s\n", d1, d2, d3);
    if (res==0) {printf("Error leyendo dato No. %d de archivo de parametros de control\n", numeroDeDatosLeidos); exit(0);}
    strcpy(p_parametros->pathOfOutputDirectory,d3);
    printf("   directorio de salida: %s\n", p_parametros->pathOfOutputDirectory);
    numeroDeDatosLeidos++;

    res=fscanf(archivo, "\n");

    printf("  Numero de datos leidos: %d\n", numeroDeDatosLeidos);
    //Cerrando archivo
    fclose(archivo);
}

//  ------------------------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------
//  FUNCIONES DE LECTURA Y ESCRITURA DE IMAGENES
//  ------------------------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------

struct BMPHeader
{
    char bfType[3];       /* "BM" */
    int bfSize;           /* Size of file in bytes */
    int bfReserved;       /* set to 0 */
    int bfOffBits;        /* Byte offset to actual bitmap data (= 54) */
    int biSize;           /* Size of BITMAPINFOHEADER, in bytes (= 40) */
    int biWidth;          /* Width of image, in pixels */
    int biHeight;         /* Height of images, in pixels */
    short biPlanes;       /* Number of planes in target device (set to 1) */
    short biBitCount;     /* Bits per pixel (24 in this case) */
    int biCompression;    /* Type of compression (0 if no compression) */
    int biSizeImage;      /* Image size, in bytes (0 if no compression) */
    int biXPelsPerMeter;  /* Resolution in pixels/meter of display device */
    int biYPelsPerMeter;  /* Resolution in pixels/meter of display device */
    int biClrUsed;        /* Number of colors in the color table (if 0, use
                           maximum allowed by biBitCount) */
    int biClrImportant;   /* Number of important colors.  If 0, all colors
                           are important */
};

//Esta funcion lee imagen RGB de archivo en formato BMP
void readRGBImageFromBMPFile(char *filename)
{
    FILE *fd;
    int width, height;
    int i, j;
    int n;

    fd = fopen(filename, "rb");
    if (fd == NULL)
    {
        printf("Error: fopen failed\n");
        return;
    }

    unsigned char header[54];

    // Read header
    n=fread(header, sizeof(unsigned char), 54, fd);
    if (n==0) {printf("No se pudieron leer datos\n"); exit(0);}

    // Capture dimensions
    width = *(int*)&header[18];
    height = *(int*)&header[22];

    int padding = 0;

    // Calculate padding
    while ((width * 3 + padding) % 4 != 0)
    {
        padding++;
    }

    // Compute new width, which includes padding
    int widthnew = width * 3 + padding;

    // Allocate temporary memory to read widthnew size of data
    unsigned char* data = (unsigned char *)malloc(widthnew * sizeof (unsigned int));

    // Read row by row of data and remove padded data.
    for (i = 0; i<height; i++)
    {
        // Read widthnew length of data
        n=fread(data, sizeof(unsigned char), widthnew, fd);
        if (n==0) {printf("No se pudieron leer datos\n"); exit(0);}

        // Retain width length of data, and swizzle RB component.
        // BMP stores in BGR format, my usecase needs RGB format
        for (j = 0; j < width * 3; j += 3)
        {
            int index = (i * width * 3) + (j);
            pInputImage->prgb[index + 0] = data[j + 2];
            pInputImage->prgb[index + 1] = data[j + 1];
            pInputImage->prgb[index + 2] = data[j + 0];
        }
    }

    free(data);
    fclose(fd);
}

//Esta funcion almacena una imagen de intensidad en
//archivo en formato YUV400
void geoSaveIntensityImageIn_YUV400_file(unsigned char *pintensity, char* filename)
{
    int numread, j, jj, i;
    FILE *fpointer;
    int width, height;
    unsigned char *ptempImage;

    width=pInputImage->width;
    height=pInputImage->height;

    //Imagen de uso local y temporal
    ptempImage = (unsigned char *)malloc(sizeof(unsigned char)*width*height);

    printf("Saving unsigned char image in: %s\n", filename);

    //Abriendo archivo
    fpointer = fopen(filename, "w");
    if (fpointer == NULL) {
        printf("could not read the file: %s\n", filename);
        exit(0);
    }

    //Cambiando posición del sistema de coordenadas de la equina inferior
    //izquierda a la esquina superior izquierda.
    for (i=0;i<width*height;i++) ptempImage[i]=0;
    jj=0;
    for (j=height-1;j>=0;j--) {
        for (i=0;i<width;i++) {
            ptempImage[jj*width+i]= pintensity[j*width+i];
        }
        jj++;
    }

    //Almacenando valores de intensidad en archivo
    numread = (int)fwrite(ptempImage, sizeof(unsigned char), (unsigned int)(height*width), fpointer);

    if (numread==0) {
        printf("Por alguna razon no se pudo escribir dato alguno en archivo\n");
        exit(0);
    }

    fclose(fpointer);
    free(ptempImage);
}

//Esta funcion almacena una imagen de intensidad en
//archivo en formato BMP
int SaveIntensityImageIn_BMP_file(unsigned char *pintensity, char *filename)
{
    int i, j, jj, ipos;
    int bytesPerLine;
    unsigned char *line;
    unsigned char *ptempImage;
    int height, width;

    height=pInputImage->height;
    width=pInputImage->width;

    FILE *file;
    struct BMPHeader bmph;

    /* The length of each line must be a multiple of 4 bytes */

    bytesPerLine = (3 * (width + 1) / 4) * 4;

    strcpy(bmph.bfType, "BM");
    bmph.bfOffBits = 54;
    bmph.bfSize = bmph.bfOffBits + bytesPerLine * height;
    bmph.bfReserved = 0;
    bmph.biSize = 40;
    bmph.biWidth = width;
    bmph.biHeight = height;
    bmph.biPlanes = 1;
    bmph.biBitCount = 24;
    bmph.biCompression = 0;
    bmph.biSizeImage = bytesPerLine * height;
    bmph.biXPelsPerMeter = 0;
    bmph.biYPelsPerMeter = 0;
    bmph.biClrUsed = 0;
    bmph.biClrImportant = 0;

    file = fopen (filename, "wb");
    if (file == NULL) return(0);

    fwrite(&bmph.bfType, 2, 1, file);
    fwrite(&bmph.bfSize, 4, 1, file);
    fwrite(&bmph.bfReserved, 4, 1, file);
    fwrite(&bmph.bfOffBits, 4, 1, file);
    fwrite(&bmph.biSize, 4, 1, file);
    fwrite(&bmph.biWidth, 4, 1, file);
    fwrite(&bmph.biHeight, 4, 1, file);
    fwrite(&bmph.biPlanes, 2, 1, file);
    fwrite(&bmph.biBitCount, 2, 1, file);
    fwrite(&bmph.biCompression, 4, 1, file);
    fwrite(&bmph.biSizeImage, 4, 1, file);
    fwrite(&bmph.biXPelsPerMeter, 4, 1, file);
    fwrite(&bmph.biYPelsPerMeter, 4, 1, file);
    fwrite(&bmph.biClrUsed, 4, 1, file);
    fwrite(&bmph.biClrImportant, 4, 1, file);

    line = (unsigned char*) malloc(bytesPerLine);
    if (line == NULL)
    {
        fprintf(stderr, "Can't allocate memory for BMP file.\n");
        return(0);
    }

    //Cambiando posición del sistema de coordenadas de la equina inferior
    //izquierda a la esquina superior izquierda.
    ptempImage = (unsigned char *)malloc(sizeof(unsigned char)*width*height);
    for (i=0;i<width*height;i++) ptempImage[i]=0;
    jj=0;
    for (j=height-1;j>=0;j--) {
        for (i=0;i<width;i++) {
            ptempImage[jj*width+i]= pintensity[j*width+i];
        }
        jj++;
    }

    for (i = height - 1; i >= 0; i--)
    {
        for (j = 0; j < width; j++)
        {
            ipos = (width * i + j);
            line[3*j] = ptempImage[ipos];
            line[3*j+1] = ptempImage[ipos];
            line[3*j+2] = ptempImage[ipos];
        }
        fwrite(line, bytesPerLine, 1, file);
    }

    free(line);
    fclose(file);

    free(ptempImage);

    return(1);
}

//Esta funcion almacena una imagen RGB en
//archivo en formato BMP
int SaveRGBImageIn_BMP_file(unsigned char *prgb, char *filename)
{
    int i, j, jj, ipos;
    int bytesPerLine;
    unsigned char *line;
    unsigned char *ptempImage;
    int height, width;

    height=pInputImage->height;
    width=pInputImage->width;

    FILE *file;
    struct BMPHeader bmph;

    /* The length of each line must be a multiple of 4 bytes */

    bytesPerLine = (3 * (width + 1) / 4) * 4;

    strcpy(bmph.bfType, "BM");
    bmph.bfOffBits = 54;
    bmph.bfSize = bmph.bfOffBits + bytesPerLine * height;
    bmph.bfReserved = 0;
    bmph.biSize = 40;
    bmph.biWidth = width;
    bmph.biHeight = height;
    bmph.biPlanes = 1;
    bmph.biBitCount = 24;
    bmph.biCompression = 0;
    bmph.biSizeImage = bytesPerLine * height;
    bmph.biXPelsPerMeter = 0;
    bmph.biYPelsPerMeter = 0;
    bmph.biClrUsed = 0;
    bmph.biClrImportant = 0;

    file = fopen (filename, "wb");
    if (file == NULL) return(0);

    fwrite(&bmph.bfType, 2, 1, file);
    fwrite(&bmph.bfSize, 4, 1, file);
    fwrite(&bmph.bfReserved, 4, 1, file);
    fwrite(&bmph.bfOffBits, 4, 1, file);
    fwrite(&bmph.biSize, 4, 1, file);
    fwrite(&bmph.biWidth, 4, 1, file);
    fwrite(&bmph.biHeight, 4, 1, file);
    fwrite(&bmph.biPlanes, 2, 1, file);
    fwrite(&bmph.biBitCount, 2, 1, file);
    fwrite(&bmph.biCompression, 4, 1, file);
    fwrite(&bmph.biSizeImage, 4, 1, file);
    fwrite(&bmph.biXPelsPerMeter, 4, 1, file);
    fwrite(&bmph.biYPelsPerMeter, 4, 1, file);
    fwrite(&bmph.biClrUsed, 4, 1, file);
    fwrite(&bmph.biClrImportant, 4, 1, file);

    line = (unsigned char*) malloc(bytesPerLine);
    if (line == NULL)
    {
        fprintf(stderr, "Can't allocate memory for BMP file.\n");
        return(0);
    }

    //Cambiando posición del sistema de coordenadas de la equina inferior
    //izquierda a la esquina superior izquierda.
    ptempImage = (unsigned char *)malloc(sizeof(unsigned char)*width*height*3);
    for (i=0;i<width*height*3;i++) ptempImage[i]=0;
    jj=0;
    for (j=height-1;j>=0;j--) {
        for (i=0;i<width;i++) {
            ptempImage[jj*3*width+3*i]= prgb[j*3*width+3*i];
            ptempImage[jj*3*width+3*i+1]= prgb[j*3*width+3*i+1];
            ptempImage[jj*3*width+3*i+2]= prgb[j*3*width+3*i+2];
        }
        jj++;
    }

    for (i = height - 1; i >= 0; i--)
    {
        for (j = 0; j < width; j++)
        {
            ipos = 3*(width * i + j);
            line[3*j] = ptempImage[ipos+2];
            line[3*j+1] = ptempImage[ipos+1];
            line[3*j+2] = ptempImage[ipos];
        }
        fwrite(line, bytesPerLine, 1, file);
    }

    free(line);
    fclose(file);

    free(ptempImage);

    return(1);
}
