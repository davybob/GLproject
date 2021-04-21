#ifndef CAMERA_H
#define CAMERA_H
#define FPSVIEW
//#define QUAD

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    SPACE
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;


#ifdef QUAD
    glm::quat Orientation;
    float RightAngle=0.0f;
    float UpAngle=0.0f;
#endif // QUAD


    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    float gravity=9.8f;
    float sign = 1.0f;
    bool stop = true;
    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {

        Position = position;
#ifndef QUAD
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
#endif
#ifdef QUAD
        Orientation = glm::quat(0, 0, 0, -1);
#endif //QUAD
        updateCameraVectors();

    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
#ifndef QUAD
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
#endif 
#ifdef QUAD
        Orientation = glm::quat(0, 0, 0, -1);
#endif //QUAD
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
#ifdef QUAD
        glm::quat reverseOrient = glm::conjugate(Orientation);
        glm::mat4 rot = glm::mat4_cast(reverseOrient);
        glm::mat4 translation = glm::translate(glm::mat4(1.0), -Position);

        return rot * translation;
#endif //QUAD
#ifndef QUAD
        return glm::lookAt(Position, Position + Front, Up);
#endif 
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
#ifndef QUAD

#ifdef FREEVIEW
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

#endif // FREEVIEW


#ifdef FPSVIEW

    void Gravity(float i) {
        if (!stop) {
            if (Position.y < 1.0f)
                Position.y += sign*gravity * i*0.5f;
            if (Position.y >= 1.0f)
                sign = -sign;
                Position.y += sign * gravity * i * 0.5f;
        }
        if (Position.y <= 0.0f) {
            Position.y = 0.0f;
            stop = true;
            sign = -sign;
        }
    };

    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        float tempY = Front.y;
        Front.y = 0.0f;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if ((direction == SPACE && stop)) {
            stop = false;
        }
        Front.y = tempY;
        //Position.y = 0.0f; // <-- this one-liner keeps the user at the ground level (xz plane)
    }

#endif // FPSVIEW

#endif 

#ifdef QUAD
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;

        glm::quat qF = Orientation * glm::quat(0, 0, 0, -1) * glm::conjugate(Orientation);
        glm::vec3 Front = { qF.x, qF.y, qF.z };
        glm::vec3 Right = glm::normalize(glm::cross(Front, glm::vec3(0, 1, 0)));
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        Position.y = 1;

        //Position.y = 0.0f; // <-- this one-liner keeps the user at the ground level (xz plane)
    }
#endif //QUAD

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;
#ifdef QUAD

        RightAngle += xoffset;
        UpAngle += yoffset;
#endif //QUAD
#ifndef QUAD

        Yaw += std::fmod(xoffset,360.0f);
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }
#endif
        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
#ifdef QUAD
        // Yaw
        glm::quat aroundY = glm::angleAxis(glm::radians(-RightAngle), glm::vec3(0, 1, 0));

        // Pitch
        glm::quat aroundX = glm::angleAxis(glm::radians(UpAngle), glm::vec3(1, 0, 0));

        Orientation = aroundY * aroundX;
#endif //QUAD

#ifndef QUAD
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
#endif 
    }
};
#endif