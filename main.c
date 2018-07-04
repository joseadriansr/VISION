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
void geoSalvarResultadosEnArchivoDeTexto();

void jsrGetHistogram();
void jsrGetProbability();
void jsrGetThreshold();

void jsrGetWeight1(int th);
void jsrGetMean1(int th);
void jsrGetVariance1(int th);

void jsrGetWeight2(int th);
void jsrGetMean2(int th);
void jsrGetVariance2(int th);

void jsrGetVariancew();

void jsrKittler_th();
void jsrOtsu_th();
void jsrKurita_th();
void jsrMartinez_th();

double jsrKittler_vth();
double jsrKurita_vth();
double jsrOtsu_vth();

void jsrImageSegmentation(int th);

double jsrMartinez_pmKurita(int n);
double jsrMartinez_pmOtsu(int n);
double jsrMartinez_pmKittler(int n);
void jsrMartinez_mse();

void jsrSobelMask();
void jsrBordes(int u);
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

    double *pGradientesX; //gradientes en X
    unsigned char *pImagenGradientesX; //imagen de gradientes

    double *pGradientesY; //gradientes en Y
    unsigned char *pImagenGradientesY; //imagen de gradientes

    double *pMagnitudGradientes; //magnitud de los gradientes double
    unsigned char *pImagenMagnitudGradientes;
    unsigned char *pImagenBordes;
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

    double vth_kittler[256];
    double vth_kurita[256];
    double vth_otsu[256];

    double c1;
    double c2;

    double mean1;
    double mean2;

    double var1;
    double var2;
    double varw;

    int N; //Cantidad total de pixeles

    int hist[256]; //Para almacenar los valores del histograma de la imagen
    double prob[256]; //Para almacenar la probabilidad de cada color

    double pmKurita[256];
    double pmKittler[256];
    double pmOtsu[256];

    double mseKurita;
    double mseKittler;
    double mseOtsu;

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

    pInputImage->pGradientesX =(double*)malloc(sizeof(double)*width*height);
    pInputImage->pGradientesY =(double*)malloc(sizeof(double)*width*height);

    pInputImage->pImagenGradientesX =(unsigned char*)malloc(sizeof(unsigned char)*width*height);
    pInputImage->pImagenGradientesY =(unsigned char*)malloc(sizeof(unsigned char)*width*height);

    pInputImage->pMagnitudGradientes =(double*)malloc(sizeof(double)*width*height);
    pInputImage->pImagenMagnitudGradientes =(unsigned char*)malloc(sizeof(unsigned char)*width*height);
    pInputImage->pImagenBordes =(unsigned char*)malloc(sizeof(unsigned char)*width*height);

    //Cada píxel se inicializa en cero
    for (i=0;i<width*height*3;i++) pInputImage->prgb[i]=0;
    for (i=0;i<width*height;i++) {
        pInputImage->pintensity[i]=0;
        pInputImage->pthresholdedIntensity[i]=0;
        pInputImage->pGradientesX[i]=0.0;
        pInputImage->pGradientesY[i]=0.0;
        pInputImage->pImagenGradientesX[i]=0;
        pInputImage->pImagenGradientesY[i]=0;
        pInputImage->pMagnitudGradientes[i]=0.0;
        pInputImage->pImagenMagnitudGradientes[i]=0;
        pInputImage->pImagenBordes[i]=0;
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
    free(p_resultados);
    free(pInputImage->pGradientesX);
    free(pInputImage->pGradientesY);
    free(pInputImage->pImagenGradientesX);
    free(pInputImage->pImagenGradientesY);
    free(pInputImage->pMagnitudGradientes);
    free(pInputImage->pImagenMagnitudGradientes);
    free(pInputImage->pImagenBordes);

    return 0;
}

//  ------------------------------------------------------------------------------------------
//  FUNCION PARA INSERTAR FUNCIONES
//  ------------------------------------------------------------------------------------------

void geoInsertYourCodeHere()
{
    char pathAndFileName[256];

//    strcpy(pathAndFileName,"output/rgb.bmp");
//    SaveRGBImageIn_BMP_file(pInputImage->prgb, pathAndFileName);

    //Calculando la imagen de intensidad
    geoGetIntensityImageFromRGBimage();
    p_resultados->N = pInputImage->height*pInputImage->width;

    //Almacenando resultado en archivo en formato BMP
    strcpy(pathAndFileName,"output/imagenDeIntensidad.bmp");
    SaveIntensityImageIn_BMP_file(pInputImage->pintensity, pathAndFileName);

    printf("\nCantidad total de pixeles: %d \n",p_resultados->N);

    jsrGetHistogram();
//    printf("\nHistograma: \n");
//    int p;
//    for(p=0; p<256; p++){
//        printf("   h(%d) = %d\n", p, p_resultados->hist[p]);
//    }

    jsrGetProbability();
//    printf("\nProbabilidad: \n");
//    int f;
//    for(f=0; f<256; f++){
//        printf("   p(%d) = %.10lf\n", f, p_resultados->prob[f]);
//    }

    jsrKittler_th();

    printf("\n------------------------------------------------------------------\n");
    printf("\nResultados segun el algoritmo de Kittler:");
    printf("\n\n      Umbral optimo: %d\n", p_resultados->th_kittler);
    printf("\n      Peso 1 = %.10lf", p_resultados->c1);
    printf("\n      Peso 2 = %.10lf", p_resultados->c2);
    printf("\n      Media 1 = %.10lf", p_resultados->mean1);
    printf("\n      Media 2 = %.10lf", p_resultados->mean2);
    printf("\n      Varianza 1 = %.10lf", p_resultados->var1);
    printf("\n      Varianza 2 = %.10lf\n", p_resultados->var2);

    jsrImageSegmentation(p_resultados->th_kittler);

    strcpy(pathAndFileName,"output/imagenSegmentadaKittler.bmp");
    SaveIntensityImageIn_BMP_file(pInputImage->pthresholdedIntensity, pathAndFileName);

    jsrKurita_th();

    printf("\n------------------------------------------------------------------\n");
    printf("\nResultados segun el algoritmo de Kurita:");
    printf("\n\n      Umbral optimo: %d\n", p_resultados->th_kurita);
    printf("\n      Peso 1 = %.10lf", p_resultados->c1);
    printf("\n      Peso 2 = %.10lf", p_resultados->c2);
    printf("\n      Media 1 = %.10lf", p_resultados->mean1);
    printf("\n      Media 2 = %.10lf", p_resultados->mean2);
    printf("\n      Varianza 1 = %.10lf", p_resultados->var1);
    printf("\n      Varianza 2 = %.10lf\n", p_resultados->var2);
    printf("\n      Varianza w = %.10lf\n", p_resultados->varw);

    jsrImageSegmentation(p_resultados->th_kurita);

    strcpy(pathAndFileName,"output/imagenSegmentadaKurita.bmp");
    SaveIntensityImageIn_BMP_file(pInputImage->pthresholdedIntensity, pathAndFileName);

    jsrOtsu_th();

    printf("\n------------------------------------------------------------------\n");
    printf("\nResultados segun el algoritmo de Otsu:");
    printf("\n\n      Umbral optimo: %d\n", p_resultados->th_otsu);
    printf("\n      Peso 1 = %.10lf", p_resultados->c1);
    printf("\n      Peso 2 = %.10lf", p_resultados->c2);
    printf("\n      Media 1 = %.10lf", p_resultados->mean1);
    printf("\n      Media 2 = %.10lf", p_resultados->mean2);
    printf("\n      Varianza 1 = %.10lf", p_resultados->var1);
    printf("\n      Varianza 2 = %.10lf\n", p_resultados->var2);
    printf("\n      Varianza w = %.10lf\n", p_resultados->varw);

    jsrImageSegmentation(p_resultados->th_otsu);

    strcpy(pathAndFileName,"output/imagenSegmentadaOtsu.bmp");
    SaveIntensityImageIn_BMP_file(pInputImage->pthresholdedIntensity, pathAndFileName);

    jsrMartinez_th();

    printf("\n------------------------------------------------------------------\n");
    printf("\nResultados segun el metodo del Prof.Dr.Ing.Geovanni Martinez:");
    if(p_resultados->th_martinez==p_resultados->th_kittler){
        printf("\n\n      El umbral optimo corresponde al umbral segun Kittler: %d\n", p_resultados->th_martinez);
    }
    else if(p_resultados->th_martinez==p_resultados->th_kurita){
        printf("\n\n      El umbral optimo corresponde al umbral segun Kurita: %d\n", p_resultados->th_martinez);
    }
    else if(p_resultados->th_martinez==p_resultados->th_otsu){
        printf("\n\n      El umbral optimo corresponde al umbral segun Otsu: %d\n", p_resultados->th_martinez);
    }
    printf("\n------------------------------------------------------------------\n");

    geoSalvarResultadosEnArchivoDeTexto();
    printf("\n... Guardando los resultados en un archivo .txt ...\n");
    printf("\n------------------------------------------------------------------\n");

    jsrSobelMask();
    printf("\n... Calculando la imagen de gradientes en X ...\n");
    printf("\n... Calculando la imagen de gradientes en Y ...\n");

    strcpy(pathAndFileName,"output/imagenGradientesX.bmp");
    SaveIntensityImageIn_BMP_file(pInputImage->pImagenGradientesX, pathAndFileName);

    strcpy(pathAndFileName,"output/imagenGradientesY.bmp");
    SaveIntensityImageIn_BMP_file(pInputImage->pImagenGradientesY, pathAndFileName);
    printf("\n------------------------------------------------------------------\n");

    int u = 10;
    jsrBordes(u);
    printf("\n... Calculando la magnitud de los gradientes ...\n");
    printf("\n... Calculando la imagen de bordes con un umbral u=%d ...\n",u);

    strcpy(pathAndFileName,"output/imagenBordes.bmp");
    SaveIntensityImageIn_BMP_file(pInputImage->pImagenBordes, pathAndFileName);
    printf("\n------------------------------------------------------------------\n");
    printf("\nPara ver los resultados, abra la carpeta /output \n");
    printf("\n------------------------------------------------------------------\n");

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
    printf("\n... Calculando histograma h(n) ...");
    int k; int q; int g; //Variables auxiliares
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
    printf("\n... Calculando las probabilidades p(n) ...");
    int k;
    for(k=0; k<256; k++){
        p_resultados->prob[k] = (double)p_resultados->hist[k]/(double)p_resultados->N;
    }
}

//  ------------------------------------------------------------------------------------------
//  FUNCION PARA OBTENER EL UMBRAL OPTIMO th_op MEDIANTE EL METODO DE MARTINEZ
//  ------------------------------------------------------------------------------------------
void jsrGetThreshold(){
    jsrKittler_th();
    jsrKurita_th();
    jsrOtsu_th();


}

//  ------------------------------------------------------------------------------------------
//  FUNCION PARA OBTENER EL th_op SEGUN EL ALGORITMO DE KITTLER
//  ------------------------------------------------------------------------------------------
void jsrKittler_th(){
    int th; int k; int e; int i; int j; //Variables auxiliares

    for(e=0; e<256; e++){p_resultados->vth_kittler[e] = 0.0;} //Inicializando el arreglo que contiene v(th)
    //Probando el algoritmo para todos los valores de th
    for(th=0; th<256; th++){
        jsrGetWeight1(th);
        jsrGetWeight2(th);
        jsrGetMean1(th);
        jsrGetMean2(th);
        jsrGetVariance1(th);
        jsrGetVariance2(th);

        p_resultados->vth_kittler[th] = jsrKittler_vth(); //Guardando todos los valores de v(th)
    }

    //Arreglo y variable auxiliar para encontrar el th_op
    double aux;
    double arregloaux[256];
    //Ordenando el arreglo auxiliar
    for(k=0; k<256; k++){arregloaux[k] = p_resultados->vth_kittler[k];}
    for (i=0; i<255; i++){
        for (j=i+1; j<256; j++){
            if(arregloaux[i]>arregloaux[j]){
             aux = arregloaux[i];
             arregloaux[i] = arregloaux[j];
             arregloaux[j] = aux;
            }
        }
    }

    int w = 255;
    while( arregloaux[w]==0 ){w--;} //Encontrando el mayor valor de v(th)

    int th_op = 0;
    while( p_resultados->vth_kittler[th_op] != arregloaux[w] ){th_op++;} //Encontrando el th_op

    //Almacenando los resultados optenido con el th_op en el contenedor de resultados
    p_resultados->th_kittler = th_op;
    jsrGetWeight1(th_op);
    jsrGetWeight2(th_op);
    jsrGetMean1(th_op);
    jsrGetMean2(th_op);
    jsrGetVariance1(th_op);
    jsrGetVariance2(th_op);

}

//  ------------------------------------------------------------------------------------------
//  FUNCION DE VEROSIMILITUD SEGUN EL ALGORITMO DE KITTLER
//  ------------------------------------------------------------------------------------------
double jsrKittler_vth(){
    double vth=0.0;
    //Si no hay valores que indefinan los logaritmos se procede con la operacion
    if(p_resultados->c1!=0.0 && p_resultados->c2!=0.0 && p_resultados->var1!=0.0 && p_resultados->var2!=0.0){
        vth = (double)(p_resultados->N)*( p_resultados->c1*log(p_resultados->c1)+p_resultados->c2*log(p_resultados->c2) -0.50*log(2.0*PI) -0.50*(p_resultados->c1*log(p_resultados->var1)+p_resultados->c2*log(p_resultados->var2)) -0.50 );
        return vth;
    }
    else{return vth;}
}

//  ------------------------------------------------------------------------------------------
//  FUNCION PARA OBTENER EL th_op SEGUN EL ALGORITMO DE KURITA
//  ------------------------------------------------------------------------------------------
void jsrKurita_th(){
    int th; int k; int e; int i; int j; //Variables auxiliares

    for(e=0; e<256; e++){ p_resultados->vth_kurita[e] = 0.0; } //Inicializando el arreglo que contiene v(th)
    //Probando el algoritmo para todos los valores de th
    for(th=0; th<256; th++){
        jsrGetWeight1(th);
        jsrGetWeight2(th);
        jsrGetMean1(th);
        jsrGetMean2(th);
        jsrGetVariance1(th);
        jsrGetVariance2(th);
        jsrGetVariancew();

        p_resultados->vth_kurita[th] = jsrKurita_vth(); //Guardando todos los valores de v(th)
    }
    //Arreglo y variable auxiliar para encontrar el th_op
    double aux;
    double arregloaux[256];

    for(k=0; k<256; k++){ arregloaux[k] = p_resultados->vth_kurita[k];}

    //Ordenando el arreglo auxiliar
    for (i=0; i<255; i++){
        for (j=i+1; j<256; j++){
            if(arregloaux[i]>arregloaux[j])
            {
             aux = arregloaux[i];
             arregloaux[i] = arregloaux[j];
             arregloaux[j] = aux;
            }
        }
    }

    int w = 255;
    while( arregloaux[w]==0 ){ w--;} //Encontrando el mayor valor de v(th)

    int th_op = 0;
    while( p_resultados->vth_kurita[th_op] != arregloaux[w] ){ th_op++;} //Encontrando el th_op

    //Almacenando los resultados optenido con el th_op en el contenedor de resultados
    p_resultados->th_kurita = th_op;
    jsrGetWeight1(th_op);
    jsrGetWeight2(th_op);
    jsrGetMean1(th_op);
    jsrGetMean2(th_op);
    jsrGetVariance1(th_op);
    jsrGetVariance2(th_op);
    jsrGetVariancew();

}

//  ------------------------------------------------------------------------------------------
//  FUNCION DE VEROSIMILITUD SEGUN EL ALGORITMO DE KURITA
//  ------------------------------------------------------------------------------------------
double jsrKurita_vth(){
    double vth=0.0;
    if(p_resultados->c1!=0.0 && p_resultados->c2!=0.0 && p_resultados->var1!=0.0 && p_resultados->varw!=0.0 && p_resultados->var2!=0.0){
        vth = (double)(p_resultados->N)*( p_resultados->c1*log(p_resultados->c1)+p_resultados->c2*log(p_resultados->c2) -0.50*log(2*PI) -0.50*log(p_resultados->varw) -0.50 );
        return vth;
    }
    else{return vth;}
}

//  ------------------------------------------------------------------------------------------
//  FUNCION PARA OBTENER EL th_op SEGUN EL ALGORITMO DE otsu
//  ------------------------------------------------------------------------------------------
void jsrOtsu_th(){
    int th; int k; int e; int i; int j; //Variables auxiliares

    for(e=0; e<256; e++){ p_resultados->vth_otsu[e] = 0.0; } //Inicializando el arreglo que contiene v(th)
    //Probando el algoritmo para todos los valores de th
    for(th=0; th<256; th++){
        jsrGetWeight1(th);
        jsrGetWeight2(th);
        jsrGetMean1(th);
        jsrGetMean2(th);
        jsrGetVariance1(th);
        jsrGetVariance2(th);
        jsrGetVariancew();

        p_resultados->vth_otsu[th] = -1.0*jsrOtsu_vth(); //Guardando todos los valores de v(th)
//        printf("\nv(%d)= %.10lf\n", th, p_resultados->vth_otsu[th]);
    }
    //Arreglo y variable auxiliar para encontrar el th_op
    double aux;
    double arregloaux[256];

    for(k=0; k<256; k++){ arregloaux[k] = p_resultados->vth_otsu[k];}

    //Ordenando el arreglo auxiliar
    for (i=0; i<255; i++){
        for (j=i+1; j<256; j++){
            if(arregloaux[i]>arregloaux[j])
            {
             aux = arregloaux[i];
             arregloaux[i] = arregloaux[j];
             arregloaux[j] = aux;
            }
        }
    }

//    int l;
//    for(l=0;l<256;l++){printf("\narregloaux(%d)= %.10lf\n", l, arregloaux[l]);}

    int w = 255;
    while( arregloaux[w]==0 ){ w--;} //Encontrando el mayor valor de v(th)

    int th_op = 0;
    while( p_resultados->vth_otsu[th_op] != arregloaux[w] ){ th_op++;} //Encontrando el th_op

    //Almacenando los resultados optenido con el th_op en el contenedor de resultados
    p_resultados->th_otsu = th_op;
    jsrGetWeight1(th_op);
    jsrGetWeight2(th_op);
    jsrGetMean1(th_op);
    jsrGetMean2(th_op);
    jsrGetVariance1(th_op);
    jsrGetVariance2(th_op);
    jsrGetVariancew();

}

//  ------------------------------------------------------------------------------------------
//  FUNCION DE VEROSIMILITUD SEGUN EL ALGORITMO DE OTSU
//  ------------------------------------------------------------------------------------------
double jsrOtsu_vth(){
    double vth=0.0;
    if(p_resultados->c1!=0.0 && p_resultados->c2!=0.0 && p_resultados->var1!=0.0 && p_resultados->varw!=0.0 && p_resultados->var2!=0.0){
        vth = (double)(p_resultados->N)*(-0.50)*( log(2.0*PI)-1.0*log(p_resultados->varw)-1.0 );
        return vth;
    }
    else{return vth;}
}

//  ------------------------------------------------------------------------------------------
//  FUNCION PARA OBTENER LOS PESOS c1 Y c2 CON EL UMBRAL ENVIADO th
//  ------------------------------------------------------------------------------------------
void jsrGetWeight1(int th){
    int n;
    p_resultados->c1=0.0;
    for(n=0; n<=th; n++){
        p_resultados->c1 += p_resultados->prob[n];
    }
}
void jsrGetWeight2(int th){
    int n;
    p_resultados->c2=0.0;
    for(n=th+1; n<256; n++){
        p_resultados->c2 += p_resultados->prob[n];
    }
}

//  ------------------------------------------------------------------------------------------
//  FUNCION PARA OBTENER LAS MEDIAS m1 Y m2 CON EL UMBRAL ENVIADO th
//  ------------------------------------------------------------------------------------------
void jsrGetMean1(int th){
    int n;
    p_resultados->mean1=0.0;
    if(p_resultados->c1 != 0.0){
        for(n=0; n<=th; n++){
            p_resultados->mean1 += ((double)n*p_resultados->prob[n]);
        }
        p_resultados->mean1 = p_resultados->mean1/p_resultados->c1;
    }
}
void jsrGetMean2(int th){
    int n;
    p_resultados->mean2=0.0;
    if(p_resultados->c2!=0.0){
        for(n=th+1; n<256; n++){
            p_resultados->mean2 += ((double)n*p_resultados->prob[n]);
        }
        p_resultados->mean2 = p_resultados->mean2/p_resultados->c2;
    }
}
//  ------------------------------------------------------------------------------------------
//  FUNCION PARA OBTENER LAS VARIANZAS var1 Y var2 CON EL UMBRAL ENVIADO th
//  ------------------------------------------------------------------------------------------
void jsrGetVariance1(int th){
    int n;
    p_resultados->var1=0.0;
    if(p_resultados->c1!=0.0){
        for(n=0; n<=th; n++){
            p_resultados->var1 += ((double)n-p_resultados->mean1)*((double)n-p_resultados->mean1)*p_resultados->prob[n];
        }
        p_resultados->var1 = p_resultados->var1/p_resultados->c1;
    }
}
void jsrGetVariance2(int th){
    int n;
    p_resultados->var2=0.0;
    if(p_resultados->c2!=0.0){
        for(n=th+1; n<256; n++){
            p_resultados->var2 += ((double)n-p_resultados->mean2)*((double)n-p_resultados->mean2)*p_resultados->prob[n];
        }
        p_resultados->var2 = p_resultados->var2/p_resultados->c2;
    }
}

//  ------------------------------------------------------------------------------------------
//  FUNCION PARA OBTENER LA VARIANZA varw CON EL UMBRAL ENVIADO th PARA EL ALGORITMO DE KURITA Y OTSU
//  ------------------------------------------------------------------------------------------
void jsrGetVariancew(){
    p_resultados->varw = p_resultados->c1*p_resultados->var1 + p_resultados->c2*p_resultados->var2;
}

//  ------------------------------------------------------------------------------------------
//  FUNCION PARA SEGMENTAR LA IMAGEN CON UN VALOR DE UMBRALIZACION th
//  ------------------------------------------------------------------------------------------
void jsrImageSegmentation(int th){
    int n;
    for(n=0;n<p_resultados->N;n++){
        if(pInputImage->pintensity[n]<=(unsigned char)th){
            pInputImage->pthresholdedIntensity[n] = 255;
        }
        else{
            pInputImage->pthresholdedIntensity[n] = 0;
        }
    }
}

//  ------------------------------------------------------------------------------------------
//  MODELO DE POBLACION MIXTA ASOCIADO CON LOS UMBRALES
//  ------------------------------------------------------------------------------------------
double jsrMartinez_pmKurita(int n){
    double pm = 0.0;
    double aux1; double aux2; double aux3; double aux4;
    if(p_resultados->c1!=0.0 && p_resultados->c2!=0.0 && p_resultados->var1!=0.0 && p_resultados->varw!=0.0 && p_resultados->var2!=0.0){
        aux1 =  ( (p_resultados->c1)*(double)p_resultados->th_kurita )/( sqrt( 2.0*PI*p_resultados->varw*(double)p_resultados->th_kurita ) );
        aux2 = exp( (-1.0*((double)n-(p_resultados->mean1*(double)p_resultados->th_kurita))*((double)n-p_resultados->mean1*(double)p_resultados->th_kurita))/(2.0*p_resultados->varw*(double)p_resultados->th_kurita));
        aux3 =  ( (p_resultados->c2)*(double)p_resultados->th_kurita )/( sqrt( 2.0*PI*p_resultados->varw*(double)p_resultados->th_kurita ) );
        aux4 = exp( (-1.0*((double)n-(p_resultados->mean2*(double)p_resultados->th_kurita))*((double)n-p_resultados->mean2*(double)p_resultados->th_kurita))/(2.0*p_resultados->varw*(double)p_resultados->th_kurita));
        pm = aux1*aux2 + aux3*aux4;
        return pm;
    }
    return pm;
}
double jsrMartinez_pmOtsu(int n){
    double pm = 0.0;
    double aux1; double aux2; double aux3; double aux4;
    if(p_resultados->c1!=0.0 && p_resultados->c2!=0.0 && p_resultados->var1!=0.0 && p_resultados->varw!=0.0 && p_resultados->var2!=0.0){
        aux1 =  ( (p_resultados->c1)*(double)p_resultados->th_otsu )/( sqrt( 2.0*PI*p_resultados->varw*(double)p_resultados->th_otsu ) );
        aux2 = exp( (-1.0*((double)n-(p_resultados->mean1*(double)p_resultados->th_otsu))*((double)n-p_resultados->mean1*(double)p_resultados->th_otsu))/(2.0*p_resultados->varw*(double)p_resultados->th_otsu));
        aux3 =  ( (p_resultados->c2)*(double)p_resultados->th_otsu )/( sqrt( 2.0*PI*p_resultados->varw*(double)p_resultados->th_otsu ) );
        aux4 = exp( (-1.0*((double)n-(p_resultados->mean2*(double)p_resultados->th_otsu))*((double)n-p_resultados->mean2*(double)p_resultados->th_otsu))/(2.0*p_resultados->varw*(double)p_resultados->th_otsu));
        pm = aux1*aux2 + aux3*aux4;
        return pm;
    }
    return pm;
}
double jsrMartinez_pmKittler(int n){
    double pm = 0.0;
    double aux1; double aux2; double aux3; double aux4;
    if(p_resultados->c1!=0.0 && p_resultados->c2!=0.0 && p_resultados->var1!=0.0 && p_resultados->varw!=0.0 && p_resultados->var2!=0.0){
        aux1 =  ( (p_resultados->c1)*(double)p_resultados->th_kittler )/( sqrt( 2.0*PI*p_resultados->var1*(double)p_resultados->th_kittler ) );
        aux2 = exp( (-1.0*((double)n-(p_resultados->mean1*(double)p_resultados->th_kittler))*((double)n-p_resultados->mean1*(double)p_resultados->th_kittler))/(2.0*p_resultados->var1*(double)p_resultados->th_kittler));
        aux3 =  ( (p_resultados->c2)*(double)p_resultados->th_kittler )/( sqrt( 2.0*PI*p_resultados->var2*(double)p_resultados->th_kittler ) );
        aux4 = exp( (-1.0*((double)n-(p_resultados->mean2*(double)p_resultados->th_kittler))*((double)n-p_resultados->mean2*(double)p_resultados->th_kittler))/(2.0*p_resultados->var2*(double)p_resultados->th_kittler));
        pm = aux1*aux2 + aux3*aux4;
        return pm;
    }
    return pm;
}

//  ------------------------------------------------------------------------------------------
//  FUNCION PARA ENCONTRAR EL th_op SEGUN EL METODO DE MARTINEZ
//  ------------------------------------------------------------------------------------------
void jsrMartinez_th(){
    jsrMartinez_mse();
    p_resultados->th_martinez=0;
    if(p_resultados->mseKittler <= p_resultados->mseKurita && p_resultados->mseKittler <= p_resultados->mseOtsu){
        p_resultados->th_martinez = p_resultados->th_kittler;
    }
    else if(p_resultados->mseKurita <= p_resultados->mseKittler && p_resultados->mseKurita <= p_resultados->mseOtsu){
        p_resultados->th_martinez = p_resultados->th_kurita;
    }
    else if(p_resultados->mseOtsu <= p_resultados->mseKittler && p_resultados->mseOtsu <= p_resultados->mseKurita){
        p_resultados->th_martinez = p_resultados->th_otsu;
    }
}

//  ------------------------------------------------------------------------------------------
//  FUNCION PARA ENCONTRAR EL ERROR CUADRATICO MEDIO mse DE CADA MODELO DE POBLACION MIXTA
//  ------------------------------------------------------------------------------------------
void jsrMartinez_mse(){
    int a; int b; int c; int n;
    for(a=0; a<256; a++){
        p_resultados->pmKurita[a] = jsrMartinez_pmKurita(a);
    }
    for(b=0; b<256; b++){
        p_resultados->pmKittler[b] = jsrMartinez_pmKittler(b);
    }
    for(c=0; c<256; c++){
        p_resultados->pmOtsu[c] = jsrMartinez_pmOtsu(c);
    }
    p_resultados->mseKittler=0.0; p_resultados->mseKurita=0.0; p_resultados->mseOtsu=0.0;
    for(n=0; n<256; n++){
        p_resultados->mseKittler += (p_resultados->pmKittler[n]-p_resultados->prob[n])*(p_resultados->pmKittler[n]-p_resultados->prob[n]);
        p_resultados->mseKurita += (p_resultados->pmKurita[n]-p_resultados->prob[n])*(p_resultados->pmKurita[n]-p_resultados->prob[n]);
        p_resultados->mseOtsu += (p_resultados->pmOtsu[n]-p_resultados->prob[n])*(p_resultados->pmOtsu[n]-p_resultados->prob[n]);
    }
    p_resultados->mseKittler = p_resultados->mseKittler/256.0;
    p_resultados->mseKurita = p_resultados->mseKurita/256.0;
    p_resultados->mseOtsu = p_resultados->mseOtsu/256.0;
}


//  ------------------------------------------------------------------------------------------
//  FUNCION PARA APLICAR LA MASCARA DE SOBEL EN X Y EN Y
//  ------------------------------------------------------------------------------------------
void jsrSobelMask(){
    int y; int x; int b; int i; int c; int p; int f; //Contadores
    int w = pInputImage->width; int h = pInputImage->height; //Para simplificar la lectura
    double pY[w*h]; //Para simplificar la lectura
    for(b=0;b<w*h;b++){
        pY[b] = (double)pInputImage->pintensity[b]; //La imagen de gradientes es double, se hace casting a la imagen de intensidad
    }
    //Calculando los gradientes en X
    for(y=1;y<h-1;y++){
        for(x=1;x<w-1;x++){
            pInputImage->pGradientesX[y*w+x] = (1.0/8.0)*( pY[(y+1)*w+(x+1)] +2.0*pY[y*w+(x+1)] +pY[(y-1)*w+(x+1)] -pY[(y+1)*w+(x-1)] -2.0*pY[y*w+(x-1)] -pY[(y-1)*w+(x-1)] );
        }
    }
    //Calculando los gradientes en Y
    for(i=1; i<h-1; i++){
        for(c=1; c<w-1; c++){
            pInputImage->pGradientesY[i*w+c] = (1.0/8.0)*( pY[(i+1)*w+(c-1)] +2.0*pY[(i+1)*w+c] +pY[(i+1)*w+(c+1)] -pY[(i-1)*w+(c-1)] -2.0*pY[(i-1)*w+c] -pY[(i-1)*w+(c+1)] );
        }
    }
    //Calculando los valores unsigned char necesarios para las imagenes
    for(p=0; p<w*h; p++){
        if(pInputImage->pGradientesX[p]<=0.0){
            pInputImage->pImagenGradientesX[p] = 0;
        }
        else if(pInputImage->pGradientesX[p]>=255.0){
            pInputImage->pImagenGradientesX[p] = 255;
        }
        else{
            pInputImage->pImagenGradientesX[p] = (unsigned char)pInputImage->pGradientesX[p];
        }
    }
    for(f=0; f<w*h; f++){
        if(pInputImage->pGradientesY[f]<=0.0){
            pInputImage->pImagenGradientesY[f] = 0;
        }
        else if(pInputImage->pGradientesY[f]>=255.0){
            pInputImage->pImagenGradientesY[f] = 255;
        }
        else{
            pInputImage->pImagenGradientesY[f] = (unsigned char)pInputImage->pGradientesY[f];
        }
    }
}

//  ------------------------------------------------------------------------------------------
//  FUNCION PARA CALCULAR LA MAGNITUD DE LOS GRADIENTES Y LOS BORDES
//  ------------------------------------------------------------------------------------------
void jsrBordes(int u){
    int n; int z; int j;
    for(n=0; n<p_parametros->height*p_parametros->width; n++){
        pInputImage->pMagnitudGradientes[n] = sqrt( pInputImage->pGradientesX[n]*pInputImage->pGradientesX[n] + pInputImage->pGradientesY[n]*pInputImage->pGradientesY[n] );
    }
    for(z=0; z<p_parametros->height*p_parametros->width; z++){
        if(pInputImage->pMagnitudGradientes[z]>=255.0){
            pInputImage->pImagenMagnitudGradientes[z] = 255;
        }
        else if(pInputImage->pMagnitudGradientes[z]<=0.0){
            pInputImage->pImagenMagnitudGradientes[z] = 0;
        }
        else{
            pInputImage->pImagenMagnitudGradientes[z] = (unsigned char)pInputImage->pMagnitudGradientes[z];
        }
    }
    for(j=0; j<p_parametros->height*p_parametros->width; j++){
        if(pInputImage->pImagenMagnitudGradientes[j] >= u){
            pInputImage->pImagenBordes[j] = 255;
        }
        else{
            pInputImage->pImagenBordes[j] = 0;
        }
    }
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
    char nombreDeArchivo[256]="current_control_parameters.txt";  //Para la imagen del cuadro
//    char nombreDeArchivo[256]="current_control_parameters1.txt"; //Para la imagen de Claire
//    char nombreDeArchivo[256]="current_control_parameters2.txt"; //Para la imagen de las palmeras
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

void geoSalvarResultadosEnArchivoDeTexto()
{
    FILE *archivo;
    //Abriendo archivo en modo de escritura
    char nombreDeArchivo[256]="output/resultadosOptimos.txt";
    archivo = fopen(nombreDeArchivo, "w");
    if (!archivo) {
        printf("No se pudo abrir el archivo: resultados.txt\n");
        exit(1);
    }

    fprintf(archivo,"===========================================================================\n");
    fprintf(archivo,"** II EXAMEN PROGRAMADO                                                  **\n");
    fprintf(archivo,"** JOSE ADRIAN SANABRIA ROSELLO - B46420                                 **\n");
    fprintf(archivo,"===========================================================================\n");
    fprintf(archivo,"** Basado en el programa:                                                **\n");
    fprintf(archivo,"** visionPorComputador_Prog_Ref_No1                                      **\n");
    fprintf(archivo,"** Prof. Dr.-Ing. Geovanni Martínez - IPCV-LAB                           **\n");
    fprintf(archivo,"** II-2017                                                               **\n");
    fprintf(archivo,"** Versión v004, 24 Octubre 2017, 11:30                                  **\n");
    fprintf(archivo,"===========================================================================\n");
    fprintf(archivo,"                            ARCHIVO DE RESULTADOS                          \n");
    fprintf(archivo,"===========================================================================\n");

    fprintf(archivo,"\nCantidad total de pixeles: %d \n",p_resultados->N);

    fprintf(archivo,"\n------------------------------------------------------------------\n");
    fprintf(archivo,"\nResultados segun el algoritmo de Kittler:");
    fprintf(archivo,"\n\n      Umbral optimo: %d\n", p_resultados->th_kittler);
    fprintf(archivo,"\n      Peso 1 = %.10lf", p_resultados->c1);
    fprintf(archivo,"\n      Peso 2 = %.10lf", p_resultados->c2);
    fprintf(archivo,"\n      Media 1 = %.10lf", p_resultados->mean1);
    fprintf(archivo,"\n      Media 2 = %.10lf", p_resultados->mean2);
    fprintf(archivo,"\n      Varianza 1 = %.10lf", p_resultados->var1);
    fprintf(archivo,"\n      Varianza 2 = %.10lf\n", p_resultados->var2);

    fprintf(archivo,"\n------------------------------------------------------------------\n");
    fprintf(archivo,"\nResultados segun el algoritmo de Kurita:");
    fprintf(archivo,"\n\n      Umbral optimo: %d\n", p_resultados->th_kurita);
    fprintf(archivo,"\n      Peso 1 = %.10lf", p_resultados->c1);
    fprintf(archivo,"\n      Peso 2 = %.10lf", p_resultados->c2);
    fprintf(archivo,"\n      Media 1 = %.10lf", p_resultados->mean1);
    fprintf(archivo,"\n      Media 2 = %.10lf", p_resultados->mean2);
    fprintf(archivo,"\n      Varianza 1 = %.10lf", p_resultados->var1);
    fprintf(archivo,"\n      Varianza 2 = %.10lf\n", p_resultados->var2);
    fprintf(archivo,"\n      Varianza w = %.10lf\n", p_resultados->varw);

    fprintf(archivo,"\n------------------------------------------------------------------\n");
    fprintf(archivo,"\nResultados segun el algoritmo de Otsu:");
    fprintf(archivo,"\n\n      Umbral optimo: %d\n", p_resultados->th_otsu);
    fprintf(archivo,"\n      Peso 1 = %.10lf", p_resultados->c1);
    fprintf(archivo,"\n      Peso 2 = %.10lf", p_resultados->c2);
    fprintf(archivo,"\n      Media 1 = %.10lf", p_resultados->mean1);
    fprintf(archivo,"\n      Media 2 = %.10lf", p_resultados->mean2);
    fprintf(archivo,"\n      Varianza 1 = %.10lf", p_resultados->var1);
    fprintf(archivo,"\n      Varianza 2 = %.10lf\n", p_resultados->var2);
    fprintf(archivo,"\n      Varianza w = %.10lf\n", p_resultados->varw);
    fprintf(archivo,"\n------------------------------------------------------------------\n");

    fprintf(archivo,"\nResultados segun el metodo del Prof.Dr.Ing.Geovanni Martinez:");
    if(p_resultados->th_martinez==p_resultados->th_kittler){
        fprintf(archivo,"\n\n      El umbral optimo corresponde al umbral segun Kittler: %d\n", p_resultados->th_martinez);
    }
    else if(p_resultados->th_martinez==p_resultados->th_kurita){
        fprintf(archivo,"\n\n      El umbral optimo corresponde al umbral segun Kurita: %d\n", p_resultados->th_martinez);
    }
    else if(p_resultados->th_martinez==p_resultados->th_otsu){
        fprintf(archivo,"\n\n      El umbral optimo corresponde al umbral segun Otsu: %d\n", p_resultados->th_martinez);
    }
    fprintf(archivo,"------------------------------------------------------------------\n");

    //Cerrando archivo
    fclose(archivo);
}


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
