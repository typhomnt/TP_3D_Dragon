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
    this->slice = pow(2,ceil(log2(slice)));;
    this->pos = pos;
    ground = (qglviewer::Vec**)malloc(this->slice*sizeof(qglviewer::Vec*));
    for(int i = 0 ; i < this->slice ; i++)
        ground[i] = (qglviewer::Vec*)malloc(this->slice*sizeof(qglviewer::Vec));
    for(int i = 0 ;  i < this->slice ; i++){
        for(int j = 0 ; j < this->slice ; j++){
            (ground[i][j])[0] = (this->pos)[0] + float(i)*size/this->slice;
            (ground[i][j])[1] = (this->pos)[1] + float(j)*size/this->slice;
            (ground[i][j])[2] = (this->pos)[2];
        }
    }


    colArr = (float**)malloc(this->slice*sizeof(float*));
    for(int i = 0 ; i < this->slice ; i++)
        colArr[i] = (float*)malloc(this->slice*sizeof(float));
    for(int i = 0 ;  i < this->slice ; i++){
        for(int j = 0 ; j < this->slice ; j++){
            colArr[i][j] = 0;
        }
    }
}

void Mountain::build(){
    int size = pow(2,ceil(log2(slice)));
    float map[size][size];
    for(int i = 0 ; i < size ; i++)
        for(int j = 0 ; j < size ; j++)
            map[i][j] = 0;
    for(int n = ceil(log2(slice)) - 1 ; n >  1 ; n--){
        int stp = pow(2,n);
        for(int i = 0 ; i < size ; i+= stp){
            for(int j = 0 ; j < size ; j+= stp){
                map[i][j] += ((float)rand()/(float)RAND_MAX)*stp/6;
            }
        }
        for(int i = 0 ; i < size ; i++){
            for(int j = 0 ; j < size ; j++){
                int index1 = floor(i/stp)*stp;
                int index2 = floor(j/stp)*stp;
                map[i][j] = interpol(interpol((map[index1][index2]),(map[index1 + stp ][index2]),i - index1,stp),
                        interpol((map[index1][index2 + stp]),(map[index1 + stp][index2 + stp]),i - index1,stp),j - index2,stp);
            }
        }
    }
    for(int i = 0 ; i < slice ; i++)
        for(int j = 0 ; j < slice ; j++){
            (ground[i][j])[2] =  map[i][j];
            colArr[i][j] = 255 - 255*4*float(((ground[i][j])[2]))/float(slice);
        }
}

float Mountain::interpol(float beg, float end, float diff, int step){
    return beg + (float)diff*(end -beg)/(float)step;
}


void Mountain::draw(){
    glBegin(GL_QUADS);
    float max = 0;
    for(int i = 0 ; i < slice - 1 ; i++){
        for(int j = 0 ; j < slice - 1 ; j++){
            if((ground[i][j])[2] > max)
                max = (ground[i][j])[2];
            glColor3f(0,colArr[i][j],0);
            glVertex3f((ground[i][j])[0],(ground[i][j])[1],(ground[i][j])[2]);
            glVertex3f((ground[i][j+1])[0],(ground[i][j+1])[1],(ground[i][j+1])[2]);
            glVertex3f((ground[i+1][j+1])[0],(ground[i+1][j+1])[1],(ground[i+1][j+1])[2]);
            glVertex3f((ground[i+1][j])[0],(ground[i+1][j])[1],(ground[i+1][j])[2]);
        }
    }
    glEnd();
    /*std::cout << max << std::endl;
    std::cout << slice << std::endl;*/
}
