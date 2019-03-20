//
//  Utilities.h
//  HW3_Draw_a_line
//
//  Created by hansen on 2019/3/20.
//  Copyright © 2019 hansen. All rights reserved.
//

#ifndef Utilities_h
#define Utilities_h

#include <vector>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
using namespace std;

struct Point {
    float x, y;

    bool operator == (const Point& rhs) const {
        return (this->x == rhs.x) && (this->y == rhs.y);
    }
    void operator = (const Point& p) {
        x = p.x;
        y = p.y;
    }
    Point(float _x, float _y) : x(_x), y(_y) {}
};

void PointsBindVAO(GLuint& VAO,GLuint& VBO,vector<float>& data){
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(float), data.data(), GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

}

vector<float> points2floats3d(const vector<Point> &pv){
    vector<float> data;
    data.resize(3 * pv.size());
    // unsigned int count = pv.size();
    unsigned int index = 0;
    for(int i=0; i<pv.size(); i++){
        data[index] = pv[i].x;
        data[index+1] = pv[i].y;
        data[index+2] = 0.0f;
        index += 3;
    }
    return data;
}

void swap(Point &v0,Point &v1){
    Point temp(v0.x,v0.y);
    v0 = v1;
    v1 = temp;
}

void flipX(const Point& base, Point& p) {
    float dy = base.y - p.y;
    p.y += 2 * dy;
}

void flipXY(Point& p) {
    float temp = p.x;
    p.x = p.y;
    p.y = temp;
}

vector<float> genLinePositions(Point v0,Point v1){
    if (v0.x > v1.x) {
        swap(v0, v1);
    }
    bool isFlipXY = false;
    bool isFlipX = false;

    // slope greater than 1 or less than -1
    if (abs(v0.x - v1.x) < abs(v0.y - v1.y)) {
        flipXY(v0);
        flipXY(v1);
        isFlipXY = true;
    }

    if (v0.x > v1.x) {
        swap(v0, v1);
    }

    // slope between -1 and 0
    if (v0.y > v1.y) {
        flipX(v0, v1);
        isFlipX = true;
    }

    // update the delta
    int delta_x = v1.x - v0.x;
    int delta_y = v1.y - v0.y;
    int p = 2 * delta_y - delta_x;

    // store the points
    vector<Point> pv;
    pv.clear();
    pv.push_back(v0);

    for(int i=0; i<delta_x; i++){
        Point lastP = pv.back();
        if (p <= 0) {
            pv.push_back(Point(lastP.x + 1, lastP.y));
            p += 2 * delta_y;
        }
        else {
            pv.push_back(Point(lastP.x + 1, lastP.y + 1));
            p = p + 2 * delta_y - 2 * delta_x;
        }
    }

    // Reverse the transformation at before
    if(isFlipX){
        Point base = pv.front();
        for (auto& p : pv) {
            flipX(base, p);
        }
    }
    if(isFlipXY){
        for (auto& p : pv) {
            flipXY(p);
        }
    }
    
    // convert Point Vector to 3D float Vector 
    auto data = points2floats3d(pv);
    return data;
}

vector<float> genTrianglePositions(Point v0,Point v1,Point v2){
    auto data1 = genLinePositions(v0,v1);
    auto data2 = genLinePositions(v1,v2);
    auto data3 = genLinePositions(v0,v2);

    data1.insert(data1.end(),data2.begin(),data2.end());
    data1.insert(data1.end(),data3.begin(),data3.end());
    return data1;
}

void addCirclePoints(vector<Point>& pv,const Point& centre,const int& x,const int& y){
    vector<Point> eightPoints = {
        Point(centre.x + x, centre.y + y), 
        Point(centre.x - x, centre.y + y),
        Point(centre.x + x, centre.y - y), 
        Point(centre.x - x, centre.y - y),
        Point(centre.x + y, centre.y + x), 
        Point(centre.x - y, centre.y + x),
        Point(centre.x + y, centre.y - x), 
        Point(centre.x - y, centre.y - x)
    };
    pv.insert(pv.end(), eightPoints.begin(), eightPoints.end());
}

vector<float> genCirclePositions(Point centre,int radius){
    vector<Point> pv;
    int x = 0,y = radius,d = 3 - (2*radius);
    addCirclePoints(pv,centre,x,y);

    while(x < y){
        if(d < 0){
            d = d + 4 * x + 6;
        }
        else{
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
        addCirclePoints(pv,centre,x,y);
    }
    auto data = points2floats3d(pv);
    return data;
}

#endif /* Utilities_h */
