//
//  main.cpp
//  blender2opengles
//
//  Created by Strelchenko Vadym on 6/4/16.
//  Copyright © 2016 Strelchenko Vadym. All rights reserved.
//

#include <iostream>
#include <string>
#include <fstream>
#include <typeinfo>

using namespace std;

//structure, which holds the number of ...
typedef struct Model {
    int vertices;
    int positions;
    int texels;
    int normals;
    int faces;
} Model;

//extract info about the model (num of vertices, faces, texels, etc.)
Model getObjInfo(string fp) {
    Model model = {0}; // init with 0s
    
    //open OBJ file
    ifstream inOBJ;
    inOBJ.open(fp);
    
    if (!inOBJ.good()) {
        cout << "ERROR OPENING OBJ FILE" << endl;
        exit(1);
    }
    
    //read OBJ file
    string line;
    string type;
    while (!inOBJ.eof()) {
        getline(inOBJ, line);
        type = line.substr(0, 2); // first 2 char-s
        
        if (type == "v ") {
            model.positions++;
//        } else if (type == "vt") {
  //          model.texels++;
        } else if (type == "vn") {
            model.normals++;
        } else if (type == "f ") {
            model.faces++;
        }
    }
    //TODO: delete comment
    //Even though a 3D model has shared vertex data, in this tutorial OpenGL ES will process all vertices individually instead of as indexed arrays. You already know that OpenGL ES draws triangles, so the total number of vertices will be the total number of faces times their three defining points
    model.vertices = model.faces * 3;
    
    inOBJ.close();
    
    return model;
}

float* findLowestVertex(float positions[][3], int positionsNum) {
    float* lowest = positions[0];
    
    for (int i = 1; i < positionsNum; i++) {
        if (positions[i][2] < lowest[2]) {
            lowest = positions[i];
        }
    }
    
    return lowest;
}

float* findHighestVertex(float positions[][3], int positionsNum) {
    float* highest = positions[0];
    
    for (int i = 1; i < positionsNum; i++) {
        if (positions[i][2] > highest[2]) {
            highest = positions[i];
        }
    }
    
    return highest;
}

//extract model data
void extractOBJData(string fp, float positions[][3], float texels[][2], float normals[][3], int faces[][9]) {
    //open OBJ file
    ifstream inOBJ;
    inOBJ.open(fp);
    
    if (!inOBJ.good()) {
        cout << "ERROR OPENING OBJ FILE" << endl;
        exit(1);
    }
    
    // Counters
    int p = 0;
    int t = 0;
    int n = 0;
    int f = 0;
    
    //read OBJ file
    string line;
    string type;
    while (!inOBJ.eof()) {
        getline(inOBJ, line);
        type = line.substr(0, 2); // first 2 char-s
        
        if (type == "v ") {
            // 1
            // Copy line for parsing
            char* l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            // 2
            // Extract tokens
            strtok(l, " ");
            for(int i=0; i<3; i++)
                positions[p][i] = atof(strtok(NULL, " "));
            
            // 3
            // Wrap up
            delete[] l;
            p++;
       /* } else if (type == "vt") {
            char* l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            strtok(l, " ");
            for(int i=0; i<2; i++)
                texels[t][i] = atof(strtok(NULL, " "));
            
            delete[] l;
            t++;*/
        } else if (type == "vn") {
            char* l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            strtok(l, " ");
            for(int i=0; i<3; i++)
                normals[n][i] = atof(strtok(NULL, " "));
            
            delete[] l;
            n++;
        } else if (type == "f ") {
            char* l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            strtok(l, " ");
            for(int i=0; i<9; i++)
                faces[f][i] = atof(strtok(NULL, " /"));
            
            delete[] l;
            f++;
        }
    }
    
    inOBJ.close();
}

/**
 highestLowest - wheather to write the highest and the lowest vertex to the output file.
 */
void writeJava(string classFilePath, string name, Model model, int faces[][9], float positions[][3], float texels[][2], float normals[][3], bool highestLowest) {
    ofstream outJava;
    outJava.open(classFilePath);
    
    if (!outJava.good()) {
        cout << "ERROR CREATING .java FILE" << endl;
        exit(1);
    }
    
    outJava << "// This is a .java file for the model: " << name << endl;
    outJava << endl;
    
    outJava << "public class " << name << " {" << endl;
    
    // Write statistics
    outJava << "// Positions: " << model.positions << endl;
    //outJava << "// Texels: " << model.texels << endl;
    outJava << "// Normals: " << model.normals << endl;
    outJava << "// Faces: " << model.faces << endl;
    outJava << "// Vertices: " << model.vertices << endl;
    outJava << endl;
    
    // Write declarations
    outJava << "public static final int VERTICES;" << endl;
    outJava << "public static final float[] POSITIONS;" << endl;
    //outJava << "public static final float[] TEXELS;" << endl;
    outJava << "public static final float[] NORMALS;" << endl;
    outJava << endl;
    
    // Write the highest and the lowest values.
    if (highestLowest) {
        float* highest = findHighestVertex(positions, model.vertices);
        float* lowest = findLowestVertex(positions, model.vertices);
        outJava << "// The highest vertex of the model." << endl;
        outJava << "public static final float[] HIGHEST = {" << highest[0] << "f," << highest[1] << "f," << highest[2] << "f};" << endl;
        outJava << "// The lowest vertex of the model." << endl;
        outJava << "public static final float[] LOWEST = {" << lowest[0] << "f," << lowest[1] << "f," << lowest[2] << "f};" << endl;
    }
    outJava << endl;
    
    // Write static initializer definitions
    outJava << "static {" << endl;
    
    // Vertices
    outJava << "VERTICES = " << model.vertices << ";" << endl;
    outJava << endl;
    
    // write positions
    outJava << "POSITIONS = new float[] {" << endl;
    for(int i=0; i<model.faces; i++)
    {
        int vA = faces[i][0] - 1;
        int vB = faces[i][3] - 1;
        int vC = faces[i][6] - 1;
        
        outJava << positions[vA][0] << "f, " << positions[vA][1] << "f, " << positions[vA][2] << "f, " << endl;
        outJava << positions[vB][0] << "f, " << positions[vB][1] << "f, " << positions[vB][2] << "f, " << endl;
        outJava << positions[vC][0] << "f, " << positions[vC][1] << "f, " << positions[vC][2] << "f, " << endl;
    }
    outJava << "};" << endl;
    outJava << endl;
    
    // write texels
    /*outJava << "TEXELS = new float[] {" << endl;
    for(int i=0; i<model.faces; i++)
    {
        int vtA = faces[i][1] - 1;
        int vtB = faces[i][4] - 1;
        int vtC = faces[i][7] - 1;
        
        outJava << texels[vtA][0] << "f, " << texels[vtA][1] << "f, " << endl;
        outJava << texels[vtB][0] << "f, " << texels[vtB][1] << "f, " << endl;
        outJava << texels[vtC][0] << "f, " << texels[vtC][1] << "f, " << endl;
    }
    outJava << "};" << endl;
    outJava << endl;*/
    
    // write normals
    outJava << "NORMALS = new float[] {" << endl;
    for(int i=0; i<model.faces; i++)
    {
        int vnA = faces[i][2] - 1;
        int vnB = faces[i][5] - 1;
        int vnC = faces[i][8] - 1;
        
        outJava << normals[vnA][0] << "f, " << normals[vnA][1] << "f, " << normals[vnA][2] << "f, " << endl;
        outJava << normals[vnB][0] << "f, " << normals[vnB][1] << "f, " << normals[vnB][2] << "f, " << endl;
        outJava << normals[vnC][0] << "f, " << normals[vnC][1] << "f, " << normals[vnC][2] << "f, " << endl;
    }
    outJava << "};" << endl;
    
    // Close the static initializer
    outJava << "}" << endl;
    
    // Close class
    outJava << "}";
    outJava << endl;

    // Close the file
    outJava.close();
}

int main(int argc, const char * argv[]) {
    
    string nameOBJ = argv[1];
    string filepathOBJ = "source/" + nameOBJ + ".obj";
    string filepathJava = "product/" + nameOBJ + ".java";
    
    Model model = getObjInfo(filepathOBJ);
    cout << "Model info" << endl;
    cout << "model.faces: " << model.faces << endl;
    cout << "model.normals: " << model.normals << endl;
    cout << "model.positions: " << model.positions << endl;
    cout << "model.texels: " << model.texels << endl;
    cout << "model.vertices: " << model.vertices << endl;
    
    float positions[model.positions][3]; // XYZ
    float texels[model.texels][2]; // u, v
    float normals[model.normals][3]; // XYZ
    int faces [model.faces][9]; // an entry of pointers (indices). nine integers, to describe the three vertices of a triangular face, where each vertex gets three indexes, one for its position (P), one for its texel (T) and one for its normal (N)
    
    extractOBJData(filepathOBJ, positions, texels, normals, faces);
    cout << "Model Data" << endl;
    cout << "P1: " << positions[0][0] << "x " << positions[0][1] << "y " << positions[0][2] << "z" << endl;
    cout << "T1: " << texels[0][0] << "u " << texels[0][1] << "v " << endl;
    cout << "N1: " << normals[0][0] << "x " << normals[0][1] << "y " << normals[0][2] << "z" << endl;
    cout << "F1v1: " << faces[0][0] << "p " << faces[0][1] << "t " << faces[0][2] << "n" << endl;
    
    // Write H file
    writeJava(filepathJava, nameOBJ, model, faces, positions, texels, normals, true);
    
    return 0;
}
