#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include <time.h>
#include <math.h>

#define NBIM 200
#define NBLIGNE 1080
#define NBCOL 1080

char video[NBIM][NBLIGNE][NBCOL];
char videoseuil[NBIM][NBLIGNE][NBCOL];
char videoseuilOld[NBLIGNE][NBCOL];
char videoDiff[NBIM][NBLIGNE][NBCOL];
char videoDiffErod[NBIM][NBLIGNE][NBCOL];
int videoSil[NBIM][NBLIGNE][NBCOL];


int main (void){

    int ncores;

    int i,j,k;
    int seuil = 15;
    int compteur = 0;
    int zoneDetect;
    double start, end, times , timep ;
    ncores = omp_get_num_procs();
    printf("%d coeurs disponibles\n",ncores);

    //version sequentielle

    //initialisations

    for(i=0;i<NBIM;i++){
        for(j=0;j<NBLIGNE;j++){
            for(k=0;k<NBCOL;k++){
                video[i][j][k]=(i+j+k)%255;
                videoDiff[i][j][k] = 0;
                videoDiffErod[i][j][k] = 0;
                videoSil[i][j][k]= 0;
                videoseuil[i][j][k]=0 ;
            }
        }
    }


    for(j=0;j<NBLIGNE;j++){
        for(k=0;k<NBCOL;k++){
            videoseuilOld[j][k]=0;
        }
    }

    start=omp_get_wtime();
    //etape 1 seuillage

    for(i=0;i<NBIM;i++){
        for(j=0;j<NBLIGNE;j++){
            for(k=0;k<NBCOL;k++){
                if(video[i][j][k]<seuil)
                    videoseuil[i][j][k] = 0;
                else
                    videoseuil[i][j][k] = 1;
                    }}}

    //etape 2 comparaison

    for(i=0;i<NBIM;i++){

        for(j=0;j<NBLIGNE;j++){
            for(k=0;k<NBCOL;k++){
                if(videoseuil[i][j][k] != videoseuilOld[j][k])
                    videoDiff[i][j][k] = 1;
                else
                    videoDiff[i][j][k] = 0;
                videoseuilOld[j][k] = videoseuil[i][j][k];
            }}}

    //etape 3 erosion horizontale

    for(i=0;i<NBIM;i++){
        for(j=0;j<NBLIGNE;j++){
            for(k=0;k<NBCOL;k++){
                if (k==0){
                    if (videoDiff[i][j][1]==1 && videoDiff[i][j][0]==1)
                        videoDiffErod[i][j][k]=1;
                    else
                        videoDiffErod[i][j][k]=0;
                } else
                if (k==NBCOL-1){
                    if (videoDiff[i][j][NBCOL-1]==1 && videoDiff[i][j][NBCOL-2]==1)
                        videoDiffErod[i][j][NBCOL-1]=1;
                    else
                        videoDiffErod[i][j][NBCOL-1]=0;
                }else
                if(videoDiff[i][j][k-1]==1 && videoDiff[i][j][k]==1 && videoDiff[i][j][k+1]==1)
                    videoDiffErod[i][j][k]=1;
                else
                    videoDiffErod[i][j][k]=0;

            }}}

    //etape 4 silhouette horizontale

    for(i=0;i<NBIM;i++){
            for(j=0;j<NBLIGNE;j++){
                zoneDetect =0;
                for(k=0;k<NBCOL;k++){
                    if(!zoneDetect)
                        if(videoDiffErod[i][j][k]==1)  {
                            videoSil[i][j][k] =1;
                            zoneDetect = 1;}
                        else{
                            videoSil[i][j][k] =0;
                        }
                    else
                        {
                        if(videoDiffErod[i][j][k]==0)  {
                            videoSil[i][j][k] =1;
                            zoneDetect = 0;}
                        else{
                            videoSil[i][j][k] =0;
                        }
                    }
                }}}
    end=omp_get_wtime();

    //utilisation des resultats

    for(i=0;i<NBIM;i++){
            for(j=0;j<NBLIGNE;j++){
                for(k=0;k<NBCOL;k++){
                        compteur+= videoSil[i][j][k];}}}
    printf("*******************************************\n");
    printf("Version sequentielle\n");
    printf ("res : %d \n",compteur);
    compteur =0;
    times=(end-start);
    printf ("temps d'execution seq: %g \n",times);

    /*********************************************/
    for(i=0;i<NBIM;i++){
        for(j=0;j<NBLIGNE;j++){
            for(k=0;k<NBCOL;k++){
                video[i][j][k]=(i+j+k)%255;
                videoDiff[i][j][k] = 0;
                videoDiffErod[i][j][k] = 0;
                videoSil[i][j][k]= 0;
                videoseuil[i][j][k]=0 ;
            }
        }
    }

    for(j=0;j<NBLIGNE;j++){
        for(k=0;k<NBCOL;k++){
            videoseuilOld[j][k]=0;
        }
    }

    start=omp_get_wtime();
    //etape 1 seuillage
    #pragma omp parallel for num_threads(4) private(i,j,k)
    for(i=0;i<NBIM;i++){
        for(j=0;j<NBLIGNE;j++){
            for(k=0;k<NBCOL;k++){
                if(video[i][j][k]<seuil)
                    videoseuil[i][j][k] = 0;
                else
                    videoseuil[i][j][k] = 1;
                    }}}

    //etape 2 comparaison
    for(i=0;i<NBIM;i++){
        #pragma omp parallel for num_threads(4) private(j,k)
        for(j=0;j<NBLIGNE;j++){
            for(k=0;k<NBCOL;k++){
                if(videoseuil[i][j][k] != videoseuilOld[j][k])
                    videoDiff[i][j][k] = 1;
                else
                    videoDiff[i][j][k] = 0;
                videoseuilOld[j][k] = videoseuil[i][j][k];
            }}}

    //etape 3 erosion horizontale
    #pragma omp parallel for num_threads(4) private(i,j,k)
    for(i=0;i<NBIM;i++){
        for(j=0;j<NBLIGNE;j++){
            for(k=0;k<NBCOL;k++){
                if (k==0){
                    if (videoDiff[i][j][1]==1 && videoDiff[i][j][0]==1)
                        videoDiffErod[i][j][k]=1;
                    else
                        videoDiffErod[i][j][k]=0;
                } else
                if (k==NBCOL-1){
                    if (videoDiff[i][j][NBCOL-1]==1 && videoDiff[i][j][NBCOL-2]==1)
                        videoDiffErod[i][j][NBCOL-1]=1;
                    else
                        videoDiffErod[i][j][NBCOL-1]=0;
                }else
                if(videoDiff[i][j][k-1]==1 && videoDiff[i][j][k]==1 && videoDiff[i][j][k+1]==1)
                    videoDiffErod[i][j][k]=1;
                else
                    videoDiffErod[i][j][k]=0;

            }}}

    //etape 4 silhouette horizontale
    #pragma omp parallel for num_threads(4) private(i,j,k,zoneDetect)
    for(i=0;i<NBIM;i++){
            for(j=0;j<NBLIGNE;j++){
                zoneDetect =0;
                for(k=0;k<NBCOL;k++){
                    if(!zoneDetect)
                        if(videoDiffErod[i][j][k]==1)  {
                            videoSil[i][j][k] =1;
                            zoneDetect = 1;}
                        else{
                            videoSil[i][j][k] =0;
                        }
                    else
                        {
                        if(videoDiffErod[i][j][k]==0)  {
                            videoSil[i][j][k] =1;
                            zoneDetect = 0;}
                        else{
                            videoSil[i][j][k] =0;
                        }
                    }
                }}}
    end=omp_get_wtime();

    //utilisation des resultats
    for(i=0;i<NBIM;i++){
            for(j=0;j<NBLIGNE;j++){
                for(k=0;k<NBCOL;k++){
                        compteur+= videoSil[i][j][k];}}}


    timep=(end-start);
    printf("*******************************************\n");
    printf("Version parallele 4 threads\n");
    printf ("res : %d \n",compteur);
    printf ("temps d'execution par: %g \n",timep);
    printf ("Acceleration : %g\n",times/timep);
    printf ("Efficactite : %g\n",(times/timep)/4);
    compteur =0;
    printf("*******************************************\n");
    /*********************************************/

    for(i=0;i<NBIM;i++){
        for(j=0;j<NBLIGNE;j++){
            for(k=0;k<NBCOL;k++){
                video[i][j][k]=(i+j+k)%255;
                videoDiff[i][j][k] = 0;
                videoDiffErod[i][j][k] = 0;
                videoSil[i][j][k]= 0;
                videoseuil[i][j][k]=0 ;
            }
        }
    }

    for(j=0;j<NBLIGNE;j++){
        for(k=0;k<NBCOL;k++){
            videoseuilOld[j][k]=0;
        }
    }

    start=omp_get_wtime();
    //etape 1 seuillage
    #pragma omp parallel for num_threads(6) private(i,j,k)
    for(i=0;i<NBIM;i++){
        for(j=0;j<NBLIGNE;j++){
            for(k=0;k<NBCOL;k++){
                if(video[i][j][k]<seuil)
                    videoseuil[i][j][k] = 0;
                else
                    videoseuil[i][j][k] = 1;
                    }}}

    //etape 2 comparaison
    for(i=0;i<NBIM;i++){
        #pragma omp parallel for num_threads(6) private(j,k)
        for(j=0;j<NBLIGNE;j++){
            for(k=0;k<NBCOL;k++){
                if(videoseuil[i][j][k] != videoseuilOld[j][k])
                    videoDiff[i][j][k] = 1;
                else
                    videoDiff[i][j][k] = 0;
                videoseuilOld[j][k] = videoseuil[i][j][k];
            }}}

    //etape 3 erosion horizontale
    #pragma omp parallel for num_threads(6) private(i,j,k)
    for(i=0;i<NBIM;i++){
        for(j=0;j<NBLIGNE;j++){
            for(k=0;k<NBCOL;k++){
                if (k==0){
                    if (videoDiff[i][j][1]==1 && videoDiff[i][j][0]==1)
                        videoDiffErod[i][j][k]=1;
                    else
                        videoDiffErod[i][j][k]=0;
                } else
                if (k==NBCOL-1){
                    if (videoDiff[i][j][NBCOL-1]==1 && videoDiff[i][j][NBCOL-2]==1)
                        videoDiffErod[i][j][NBCOL-1]=1;
                    else
                        videoDiffErod[i][j][NBCOL-1]=0;
                }else
                if(videoDiff[i][j][k-1]==1 && videoDiff[i][j][k]==1 && videoDiff[i][j][k+1]==1)
                    videoDiffErod[i][j][k]=1;
                else
                    videoDiffErod[i][j][k]=0;

            }}}

    //etape 4 silhouette horizontale
    #pragma omp parallel for num_threads(6) private(i,j,k,zoneDetect)
    for(i=0;i<NBIM;i++){
            for(j=0;j<NBLIGNE;j++){
                zoneDetect =0;
                for(k=0;k<NBCOL;k++){
                    if(!zoneDetect)
                        if(videoDiffErod[i][j][k]==1)  {
                            videoSil[i][j][k] =1;
                            zoneDetect = 1;}
                        else{
                            videoSil[i][j][k] =0;
                        }
                    else
                        {
                        if(videoDiffErod[i][j][k]==0)  {
                            videoSil[i][j][k] =1;
                            zoneDetect = 0;}
                        else{
                            videoSil[i][j][k] =0;
                        }
                    }
                }}}
    end=omp_get_wtime();

    //utilisation des resultats
    for(i=0;i<NBIM;i++){
            for(j=0;j<NBLIGNE;j++){
                for(k=0;k<NBCOL;k++){
                        compteur+= videoSil[i][j][k];}}}
    timep=(end-start);
    printf("Version parallele 6 threads\n");
    printf("res : %d \n",compteur);
    printf ("temps d'execution par: %g \n",timep);
    printf ("Acceleration : %g\n",times/timep);
    printf ("Efficactite : %g\n",(times/timep)/6);
    printf("*******************************************\n");
    return 0;
}
