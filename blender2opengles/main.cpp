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
        } else if (type == "vt") {
            model.texels++;
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
        } else if (type == "vt") {
            char* l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            strtok(l, " ");
            for(int i=0; i<2; i++)
                texels[t][i] = atof(strtok(NULL, " "));
            
            delete[] l;
            t++;
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

void writeH(string hp, string name, Model model) {
    ofstream outH;
    outH.open(hp);
    
    if (!outH.good()) {
        cout << "ERROR CREATING H FILE" << endl;
        exit(1);
    }
    
    outH << "// This is a .h file for the model: " << name << endl;
    outH << endl;
    
    // Write statistics
    outH << "// Positions: " << model.positions << endl;
    outH << "// Texels: " << model.texels << endl;
    outH << "// Normals: " << model.normals << endl;
    outH << "// Faces: " << model.faces << endl;
    outH << "// Vertices: " << model.vertices << endl;
    outH << endl;
    
    // Write declarations
    outH << "const int " << name << "Vertices;" << endl;
    outH << "const float " << name << "Positions[" << model.vertices*3 << "];" << endl;
    outH << "const float " << name << "Texels[" << model.vertices*2 << "];" << endl;
    outH << "const float " << name << "Normals[" << model.vertices*3 << "];" << endl;
    outH << endl;
    
    outH.close();
}

void writeCvertices(string fp, string name, Model model)
{
    // Create C file
    ofstream outC;
    outC.open(fp);
    if(!outC.good())
    {
        cout << "ERROR CREATING C FILE" << endl;
        exit(1);
    }
    
    // Write to C file
    outC << "// This is a .c file for the model: " << name << endl;
    outC << endl;
    
    // Header
    outC << "#include " << "\"" << name << ".h" << "\"" << endl;
    outC << endl;
    
    // Vertices
    outC << "const int " << name << "Vertices = " << model.vertices << ";" << endl;
    outC << endl;
    
    // Close C file
    outC.close();
}

void writeCpositions(string fp, string name, Model model, int faces[][9], float positions[][3])
{
    // 2
    // Append C file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // Positions
    outC << "const float " << name << "Positions[" << model.vertices*3 << "] = " << endl;
    outC << "{" << endl;
    for(int i=0; i<model.faces; i++)
    {
        // 3
        int vA = faces[i][0] - 1;
        int vB = faces[i][3] - 1;
        int vC = faces[i][6] - 1;
        
        // 4
        outC << positions[vA][0] << ", " << positions[vA][1] << ", " << positions[vA][2] << ", " << endl;
        outC << positions[vB][0] << ", " << positions[vB][1] << ", " << positions[vB][2] << ", " << endl;
        outC << positions[vC][0] << ", " << positions[vC][1] << ", " << positions[vC][2] << ", " << endl;
    }
    outC << "};" << endl;
    outC << endl;
    
    // Close C file
    outC.close();
}

void writeCtexels(string fp, string name, Model model, int faces[][9], float texels[][2])
{
    // Append C file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // Texels
    outC << "const float " << name << "Texels[" << model.vertices*2 << "] = " << endl;
    outC << "{" << endl;
    for(int i=0; i<model.faces; i++)
    {
        int vtA = faces[i][1] - 1;
        int vtB = faces[i][4] - 1;
        int vtC = faces[i][7] - 1;
        
        outC << texels[vtA][0] << ", " << texels[vtA][1] << ", " << endl;
        outC << texels[vtB][0] << ", " << texels[vtB][1] << ", " << endl;
        outC << texels[vtC][0] << ", " << texels[vtC][1] << ", " << endl;
    }
    outC << "};" << endl;
    outC << endl;
    
    // Close C file
    outC.close();
}

void writeCnormals(string fp, string name, Model model, int faces[][9], float normals[][3])
{
    // Append C file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // Normals
    outC << "const float " << name << "Normals[" << model.vertices*3 << "] = " << endl;
    outC << "{" << endl;
    for(int i=0; i<model.faces; i++)
    {
        int vnA = faces[i][2] - 1;
        int vnB = faces[i][5] - 1;
        int vnC = faces[i][8] - 1;
        
        outC << normals[vnA][0] << ", " << normals[vnA][1] << ", " << normals[vnA][2] << ", " << endl;
        outC << normals[vnB][0] << ", " << normals[vnB][1] << ", " << normals[vnB][2] << ", " << endl;
        outC << normals[vnC][0] << ", " << normals[vnC][1] << ", " << normals[vnC][2] << ", " << endl;
    }
    outC << "};" << endl;
    outC << endl;
    
    // Close C file
    outC.close();
}

int main(int argc, const char * argv[]) {
    
    string nameOBJ = argv[1];
    string filepathOBJ = "source/" + nameOBJ + ".obj";
    string filepathH = "product/" + nameOBJ + ".h";
    string filepathC = "product/" + nameOBJ + ".c";
    cout << filepathOBJ << endl;
    
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
    writeH(filepathH, nameOBJ, model);
    
    // Write C file
    writeCvertices(filepathC, nameOBJ, model);
    
    //append positions
    writeCpositions(filepathC, nameOBJ, model, faces, positions);
    //texels
    writeCtexels(filepathC, nameOBJ, model, faces, texels);
    //normals
    writeCnormals(filepathC, nameOBJ, model, faces, normals);
    return 0;
}
