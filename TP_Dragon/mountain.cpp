#include <stdlib.h>
#include <time.h>
#ifndef __APPLE__
    #include <GL/glew.h>
    #include <GL/glut.h>
#else
    #include <GLUT/glut.h>
#endif
#include "mountain.h"
static float amp = 0.4;
Mountain::Mountain(float size, float slice, qglviewer::Vec pos,bool ice)
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
    this->ice=ice;
}

void Mountain::build(){
    int size = pow(2,ceil(log2(slice)));
    float map[size][size];
    for(int i = 0 ; i < size ; i++)
        for(int j = 0 ; j < size ; j++)
            map[i][j] = 0;
    for(int n = ceil(log2(slice)) - 1; n >  1 ; n--){
        int stp = pow(2,n);
        for(int i = 0 ; i < size ; i+= stp){
            for(int j = 0 ; j < size ; j+= stp){
                if(i== 0 || j==0 || i == size -1 || j == size - 1)
                    map[i][j] = 0 ;
                else
                    map[i][j] += ((float)rand()/(float)RAND_MAX)*stp*amp;
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
    /*for(int i = 0 ; i < size ; i++)
        for(int j = 0 ; j < size ; j++)
            if(i== 0 || j==0 || i == size -1 || j == size - 1)
                map[i][j] = 0 ;*/

    for(int i = 0 ; i < slice ; i++)
        for(int j = 0 ; j < slice ; j++){
            (ground[i][j])[2] =  map[i][j];
            colArr[i][j] = float(10*((ground[i][j])[2]))/float(slice);
        }
}

float Mountain::interpol(float beg, float end, float diff, int step){
    //return beg + (float)diff*(end -beg)/(float)step;
    return beg + (end - beg)*sin(float(M_PI*diff)/float(step*2));
}


void Mountain::draw(){
     glTranslatef(-size/2, -size/2, 0.0);
    glBegin(GL_QUADS);
    for(int i = 0 ; i < slice - 1 ; i++){
        for(int j = 0 ; j < slice - 1 ; j++){
            if(!ice)
                glColor3f(20*colArr[i][j],colArr[i][j],colArr[i][j]);
            else
                 glColor3f(colArr[i][j],colArr[i][j],20*colArr[i][j]);
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

void Mountain::animate(){
    if(ice){
         amp -= 0.01;
         build();
        }
    if(amp < 0.001)
        for(int i = 0 ; i < slice ; i++)
            for(int j = 0 ; j < slice ; j++){
                (ground[i][j])[2] = 0;
            }

}
