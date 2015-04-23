#include <stdlib.h>
#include <time.h>
#ifndef __APPLE__
    #include <GL/glew.h>
    #include <GL/glut.h>
#else
    #include <GLUT/glut.h>
#endif
#include "mountain.h"
Mountain::Mountain(float size, float slice, qglviewer::Vec pos)
{
    this->size = size;
    this->slice = slice;
    this->pos = pos;
    ground = (qglviewer::Vec**)malloc(slice*sizeof(qglviewer::Vec*));
    for(int i = 0 ; i < slice ; i++)
        ground[i] = (qglviewer::Vec*)malloc(slice*sizeof(qglviewer::Vec));
    for(int i = 0 ;  i < slice ; i++){
        for(int j = 0 ; j < slice ; j++){
            (ground[i][j])[0] = (this->pos)[0] + float(i)*size/slice;
            (ground[i][j])[1] = (this->pos)[1] + float(j)*size/slice;
            (ground[i][j])[2] = (this->pos)[2];
        }
    }
}

void Mountain::build(){
    int size = pow(2,ceil(log2(slice)));
    float map[size][size];
    for(int i = 0 ; i < size ; i++)
        for(int j = 0 ; j < size ; j++)
            map[i][j] = 0;
    for(int n = ceil(log2(slice)) ; n > 1 ; n--){
        int stp = pow(2,n);
        for(int i = 0 ; i < size ; i+= stp){
            for(int j = 0 ; j < size ; j+= stp){
                map[i][j] += (2*(float)rand()/(float)RAND_MAX -1)*stp;
            }
        }
        for(int i = 0 ; i < size ; i++){
            for(int j = 0 ; j < size ; j++){
                int index1 = 1;
                if(i != 0){
                    while((i/index1) %(stp) >= 1 ){
                        index1 += stp;
                        if((i/index1) %(stp) == 0){
                            index1 -= stp;
                            break;
                        }
                    }
                }
                /*if(j != 0){
                    while((j/index2) %(stp) >= 1 ){
                        index2 += stp;
                        if((j/index2) %(stp) == 0){
                            index2 -= stp;
                            break;
                        }
                    }
                }*/
                if(i%stp !=0 || j%stp != 0)
                    map[i][j] = interpol(interpol((map[index1-1][0]),(map[index1 + stp -1][0]),index1 - i,stp),
                        interpol((map[index1-1][size - 1]),(map[index1 + stp -1][size - 1]),index1 - i,stp),j,size);
            }
        }
    }
    for(int i = 0 ; i < slice ; i++)
        for(int j = 0 ; j < slice ; j++)
            (ground[i][j])[2] =  map[i][j];
}

float Mountain::interpol(float beg, float end, float diff, int step){
    std::cout << end*diff/step << std::endl;
    return beg*(1 - (float)diff/(float)step) + end*(float)diff/(float)step;
}


void Mountain::initNoise(int size, int step, int oct){
     this->oct2D= oct;
     this->slice = size;
     this->step2D = step;
     this->max_size = (int)pow(ceil(this->slice * pow(2, nombre_octaves2D  - 1)  / pas2D),2);
     ground = (qglviewer::Vec*)malloc(sizeof(qglviewer::Vec)*max_size);
     for(int i = 0; i < max_size; i++)
         (ground[i])[2] = (float)rand()/ RAND_MAX;
}

float Mountain::noise2D(int i, int j){

}

float Mountain::interpol_cos2D(float a, float b, float c, float d, float x, float y){

}

float Mountain::noise2D_function(float x, float y){

}

float Mountain::perlin(float x, float y, float rough){

}



void Mountain::draw(){
    glBegin(GL_QUADS);
    for(int i = 0 ; i < slice - 1 ; i++){
        for(int j = 0 ; j < slice - 1 ; j++){
            glColor3f((ground[i][j])[2],255/((ground[i][j])[2]),(ground[i][j])[2]);
            glVertex3f((ground[i][j])[0],(ground[i][j])[1],(ground[i][j])[2]);
            glVertex3f((ground[i][j+1])[0],(ground[i][j+1])[1],(ground[i][j+1])[2]);
            glVertex3f((ground[i+1][j+1])[0],(ground[i+1][j+1])[1],(ground[i+1][j+1])[2]);
            glVertex3f((ground[i+1][j])[0],(ground[i+1][j])[1],(ground[i+1][j])[2]);
        }
    }
    glEnd();
}
