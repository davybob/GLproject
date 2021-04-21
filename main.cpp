
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <math.h> 
#include "Window.h"
#include "Render.h"
#include "shader.h"
#include "Camera.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


//For simplex method DO NOT update points, rather generate them when needed using model matrix 
glm::vec3 Support(glm::vec3 D, std::vector<glm::vec3> points) {
    /*
    Support function;
        computes furthest point along direction vector;    
        returns that furthest point
    */
    
    float max=0;
    float value=0;
    glm::vec3 point = points[0];

    for (auto it : points){
        //Furthest point will be the largest dot product of the direction vector and vertex.
        value = glm::dot(D, it);
        if (value > max){
            max = value;
            point = it;
        }
    }
    return point;
}

glm::vec3 Support(glm::vec3 D, std::vector<glm::vec3> A, std::vector<glm::vec3> B) {
    //Returns one point in Minkowski difference
    return Support(D, A) - Support(-D, B);
}

bool doSimplex(std::vector<glm::vec3> points, glm::vec3* D) {
    switch (points.size())
    {
    case 2:
        if (glm::dot(points[0]-points[1], glm::vec3(0,0,0)-points[1]) > 0) { // If AB . AO > 0
            D = &(points[1] * dot(points[0], -points[1]) - -points[0] * dot(points[0],points[1])); // a x (b x c) = (a.c)b - (a.b)c
        }
        else {
            D = &(glm::vec3(0, 0, 0) - points[1]); //A->origin!
            points.erase(points.begin()); // Remove point B 
        }
        break;
    case 3:
        //auxiliary variables
        glm::vec3 ABC = glm::cross(points[1] - points[2], points[0] - points[2]); // 2 = A ,,,,,, 1 = B ,,,,,,,,,, 0 = C
        glm::vec3 ABCAC = glm::cross(ABC, points[0] - points[2]);
        glm::vec3 ABABC = glm::cross(points[1] - points[2], ABC);

        if (glm::dot(ABCAC, glm::vec3(0,0,0)-points[2]) > 0) {
            std::cout << "POINT 2 FOUND" << std::endl;
            if (glm::dot(points[0] - points[2], glm::vec3(0, 0, 0) - points[2]) > 0) {
                D = &glm::cross(points[2] - points[0], glm::cross(glm::vec3(0, 0, 0) - points[2], points[0] - points[2])); //Only need to change search direction!
                points.erase(points.begin() + 1); // Erase second point (Point B)
            }
            else {
                if (glm::dot(points[1] - points[2], glm::vec3(0, 0, 0) - points[2]) > 0) {
                    D = &glm::cross(points[1] - points[2], glm::cross(glm::vec3(0, 0, 0) - points[2], points[1] - points[2])); //Only need to change search direction!
                    points.erase(points.begin()); // Erase third point (Point C)
                }
                else {
                    D = &(glm::vec3(0, 0, 0) - points[2]);
                    points.erase(points.begin(), points.begin() + 1); // Erase second and third point (Point B, C)
                }
            }
        }
        else{
            if (glm::dot(ABABC, glm::vec3(0, 0, 0) - points[2]) > 0){
                if (glm::dot(points[1] - points[2], glm::vec3(0, 0, 0) - points[2]) > 0) {
                    D = &glm::cross(points[1] - points[2], glm::cross(glm::vec3(0, 0, 0) - points[2], points[1] - points[2])); //Only need to change search direction!
                    points.erase(points.begin() + 1); // Erase second point (Point B)
                }
                else {
                    D = &(glm::vec3(0, 0, 0) - points[2]);
                    points.erase(points.begin(), points.begin() + 1); // Erase second and third point (Point B, C)
                }
            }
            else {
                if (glm::dot(ABC, glm::vec3(0, 0, 0) - points[2]) > 0) {
                    D = &ABC;
                }
                else {
                    D = &-ABC;
                    glm::vec3 temp = points[1];
                    points[1] = points[2];
                    points[2] = temp;
                }
            }
        }
            break;
    case 4:
            break;
    default:
            break;
    }
    return false;
}

bool collision(std::vector<glm::vec3> A, std::vector<glm::vec3> B) {
    glm::vec3 S = Support(A[0]-B[0],A,B);
    std::vector<glm::vec3> points;
    points.push_back(S);
    glm::vec3 D = -S;

    while (true) {
        glm::vec3 newPoint = Support(D, A, B);
        if (glm::dot(newPoint, D) <= 0) { //newPoint is NOT PAST ORIGIN (I.E NO INTERSECTION!) (WE WENT AS FAR AS WE COULD IN SEARCH AND DID NOT PASS ORIGIN!)
            return false;
        }
        points.push_back(newPoint); //Put new point to simplex (we have line, triangle, tetrahedron)
        if (doSimplex(points, &D)){
            std::cout << "collision!" << std::endl;
            return true;
        }
        if (points.size() > 3) 
            break;
    }
    return false;
}

bool detectCollisions(glm::vec3 posA, glm::vec3 sizeA, glm::vec3 posB, glm::vec3 sizeB) {
    // Euclidean distance between center of A and center of B
    float distanceCenter = sqrt((posA.x-posB.x) * (posA.x-posB.x) + (posA.y - posB.y) * (posA.y - posB.y) + (posA.z - posB.z) * (posA.z - posB.z) );
    if (((distanceCenter < posA.x + sizeA.x ) || (distanceCenter < posB.x + sizeB.x))&&
        ((distanceCenter < posA.y + sizeA.y ) || (distanceCenter < posB.y + sizeB.y))&&
        ((distanceCenter < posA.z + sizeA.z)||(distanceCenter < posB.z + sizeB.z)))
    {
        std::cout << "COLLISION DETECTION!!!!!!!!!!!!!!!!!!" << "\n";

    }
    return true;
}

std::vector<GLfloat> Minkowski(std::vector<glm::vec3> A, std::vector<glm::vec3> B) {
    //Returns outer points of minkowski sum
    std::vector<GLfloat> largest;
    for (auto it : A)
    {
        GLfloat large=0;
        glm::vec3 outer;
        for (auto it2 : B){
            //Furthest point will be the largest dot product of the direction vector and vertex.
            auto _val = it - it2;
            GLfloat dot = glm::dot(_val, _val);
            if (dot > large) {
                large = dot;
                outer = _val;
            }
        }
        largest.push_back(outer.x);
        largest.push_back(outer.y);
        largest.push_back(outer.z);
        largest.push_back(1.0f);
        largest.push_back(0.2f);
        largest.push_back(0.4f);
        largest.push_back(1.0f);
    }
    return largest;
}

void o(Shader& shader) {
    GLint numActiveAttribs = 0;
    GLint numActiveUniforms = 0;
    glGetProgramInterfaceiv(shader.ID, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numActiveAttribs);
    glGetProgramInterfaceiv(shader.ID, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);
    std::vector<GLchar> nameData(256);
    std::vector<GLenum> properties;
    properties.push_back(GL_NAME_LENGTH);
    properties.push_back(GL_TYPE);
    properties.push_back(GL_ARRAY_SIZE);
    std::vector<GLint> values(properties.size());
    for (GLsizei attrib = 0; attrib < numActiveAttribs; ++attrib)
    {
        glGetProgramResourceiv(shader.ID, GL_PROGRAM_INPUT, attrib, properties.size(),
            &properties[0], values.size(), NULL, &values[0]);

        nameData.resize(values[0]); //The length of the name.
        glGetProgramResourceName(shader.ID, GL_PROGRAM_INPUT, attrib, nameData.size(), NULL, &nameData[0]);
        std::string name((char*)&nameData[0], nameData.size() - 1);
    }
}

int main() {
    Window window(SCR_WIDTH, SCR_HEIGHT, "Engine");
    Shader shader("vshader.glsl", "fshader.glsl", "gshader.glsl");
    Render output(&shader);
    glm::mat4 projection = glm::perspective(glm::radians(window.camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    window.setProjection(projection, shader);
    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;
    bool firstMouse = true;

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    o(shader);
    std::vector<GLfloat> points = {
    -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f , // top-left
     0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-right
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // bottom-right
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f  // bottom-left
    };
    output.FromData("points", "", points, std::vector<GLshort>(), Render::type::pc,1, GL_LINES);
    output.Scale("points", 0.1f);
    shader.use();
    GLfloat translations_points[15][3];
    for (GLint i = 0; i < 15; i++){
        translations_points[i][0] = i-8;
        translations_points[i][1] = 0;
        translations_points[i][2] = 0;
        shader.setFloatNd(("offsets[" + std::to_string(i) + "]"), 3, translations_points[i],15);
    }

    /*
    std::vector<GLfloat> vertices = {
        //frontface
        -1.0, -1.0,  1.0,   0.0f, 0.0f,     1.0f, 1.0f, 1.0f, 0.3f ,  //bottomleft (RED) 0
        1.0, -1.0,  1.0,    1.0f, 0.0f,     1.0f, 1.0f, 1.0f, 0.3f, //bottomright (GREEN) 1
        -1.0,  1.0,  1.0,   0.0f, 1.0f,     1.0f, 1.0f, 1.0f, 0.3f, //topleft (BLUE) 2
         1.0,  1.0,  1.0,   1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 0.3f, //topright (WHITE) 3
        //backface
         -1.0, -1.0, -1.0,  0.0f, 0.0f,     1.0f, 1.0f, 1.0f, 0.3f, //bottomleft (GREY) 4
       1.0, -1.0, -1.0,     1.0f, 0.0f,     1.0f, 1.0f, 1.0f, 0.3f,  //bottomright (ORANGE) 5
       -1.0,  1.0, -1.0,    0.0f, 1.0f,     1.0f, 1.0f, 1.0f, 0.3f, //topleft (WHITE) 6
         1.0,  1.0, -1.0,   1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 0.3f //topright (PINK) 7
    };
    std::vector<GLshort> indices = {
        0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1

    };
    std::vector<GLfloat> colour = { 1.0f, 1.0f, 1.0f, 0.3f };
    std::vector<GLfloat> colour2 = { 1.0f, 0.5f, 0.31f, 0.4f };
    o(shader);
    output.polygonGenerator(12,"circle",8.5, colour2);
    output.polygonGenerator(4,"square", 1, colour);

    output.Rotate("circle",90, glm::vec3(1.0f, 0.0f, 0.0f));
    output.Translate("circle", glm::vec3(0.0f, 0.0f, -1.0f));

    output.FromData("cube", "C:/Users/Home/Downloads/wall.jpg", vertices, indices, GL_STATIC_DRAW, Render::type::ptc);
    output.Translate("cube", glm::vec3(5.0f, 0.0f, 2.0f));
    output.Scale("cube", glm::vec3(0.25f, 0.25f, 0.25f));

    output.FromData("cube2", "",vertices, indices, GL_STATIC_DRAW, Render::type::ptc);
    output.Translate("cube2", glm::vec3(2.3f, 0.1f, 2.4f));
    output.Scale("cube2", glm::vec3(0.25f, 0.25f, 0.25f));

    shader.use();
    shader.setFloatNd("lightColour", 4, colour.data());
    //Calculate light intensity
    GLfloat position[] = { output.pub_objects["square"].CoordSystem.translation.x, output.pub_objects["square"].CoordSystem.translation.y, output.pub_objects["square"].CoordSystem.translation.z };
    shader.setFloatNd("lightPosition", 3, position);
*/
do {
        window.tickRate();
        //window.camera.Gravity(window.deltaTime/3.0f);
        //window.setView(window.camera.GetViewMatrix(), shader);
        
        //if (window.mouse_press)
          //  output.Translate("circle", glm::vec3(0.0f, -0.1f, -0.1f));

        output.update();
        output.loop(); //Render scene 
        window.run(); // clear buffer, apply view / projection transform.

 } while (!window.closeWindow);

	return 0;
}