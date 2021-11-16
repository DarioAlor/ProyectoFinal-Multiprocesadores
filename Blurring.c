#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM_THREADS 1

int main() {
    double t1, t2, tiempo;
    omp_set_num_threads(NUM_THREADS);
    const double startTime = omp_get_wtime();

    FILE *image, *outputImage, *lecturas;
    image = fopen("f1.bmp","rb");          //Imagen original a transformar
    outputImage = fopen("f2_dd.bmp","wb");    //Imagen transformada
    long ancho;
    long alto;
    int nLinea=0;
    int iLinea=0;
    int promedio=0;
    int n=1;
    unsigned char r, g, b;               //Pixel
    unsigned char* ptr;

    unsigned char xx[54];
    int cuenta = 0;
    for(int i=0; i<54; i++) {
      xx[i] = fgetc(image);
      fputc(xx[i], outputImage);   //Copia cabecera a nueva imagen
    }

    ancho = (long)xx[20]*65536+(long)xx[19]*256+(long)xx[18];
    alto = (long)xx[24]*65536+(long)xx[23]*256+(long)xx[22];
    printf("Largo de la imgagen: %li\n",alto);
    printf("Ancho de la imagen: %li\n",ancho);

    ptr = (unsigned char*)malloc(alto*ancho*3* sizeof(unsigned char));

    while(!feof(image)){
      b = fgetc(image);
      g = fgetc(image);
      r = fgetc(image);

      unsigned char pixel = 0.21*r+0.72*g+0.07*b;
      ptr[cuenta] = pixel; //b
      ptr[cuenta+1] = pixel; //g
      ptr[cuenta+2] = pixel; //r
      
      cuenta++;
    }                                        //Grises

    cuenta = 0;
    int fLinea=ancho;
    # pragma omp parallel
    {
      # pragma omp for schedule(dynamic)
      for (int i = 0; i < alto*ancho; i++) {
        if(nLinea <= 3 || i <= iLinea+3 || i >= fLinea-3 || nLinea >= alto-3) {
          fputc(ptr[i], outputImage);
          fputc(ptr[i], outputImage);
          fputc(ptr[i], outputImage);
        } 
        else {
          for(int j = -4; j <= 4 ; j++){
            n = i+(ancho*j); 
            promedio = promedio + ((ptr[n]+ptr[n-1]+ptr[n-2]+ptr[n-3]+ptr[n-4]+ptr[n+1]+ptr[n+2]+ptr[n+3]+ptr[n+4])/81);
          }   

          fputc(promedio, outputImage);
          fputc(promedio, outputImage);
          fputc(promedio, outputImage);
          promedio=0;

        }
        cuenta++;

        if(cuenta==ancho) {
          cuenta=0;
          nLinea++;
          iLinea=nLinea*ancho;
          fLinea=iLinea+ancho;
        }
      }
    }

    free(ptr);
    fclose(image);
    fclose(outputImage);

    const double endTime = omp_get_wtime();
    tiempo = t2-t1;
    printf("Tomo (%lf) segundos, con %d thread(s)\n", (endTime-startTime), NUM_THREADS);

    return 0;
}