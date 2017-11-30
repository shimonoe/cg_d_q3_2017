#include "camera.h"

Camera::Camera()
{
    projectionMatrix.setToIdentity();
    computeViewMatrix();
}

void Camera::computeViewMatrix()
{
    viewMatrix.setToIdentity();
    viewMatrix.lookAt(eye, center, up);
    //projectionMatrix.setToIdentity();
    //projectionMatrix.ortho(-1, 1, -1, 1, 0, 2);
    //viewMatrix.translate(0, 0, -1);

}

void Camera::resizeViewport(int width, int height)
{
    projectionMatrix.setToIdentity();
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    projectionMatrix.perspective(60.0, aspectRatio, 0.1, 20.0);
}


