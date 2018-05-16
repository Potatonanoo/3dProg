#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
private:
	XMMATRIX ViewMatrix;
	XMFLOAT4 position;
	XMFLOAT4 forward;
	XMFLOAT4 up;
	XMFLOAT4 right;
	float speed;
	float sens;
	float rotationX;
	float rotationY;
	float mouseX;
	float mouseY;
public:
	Camera(XMFLOAT4 position, XMFLOAT4 forward, XMFLOAT4 up);
	~Camera();
	void setPosition(XMFLOAT3 pos);
	void update(float dt);
	XMVECTOR getPosition();
	XMVECTOR getLookAt();
	XMVECTOR getUpDir();
	XMMATRIX getViewMatrix();

};

#endif
