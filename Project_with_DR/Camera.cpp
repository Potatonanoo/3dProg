#include "Camera.h"
#include <Windows.h>
#include <DirectXMath.h>

Camera::Camera(XMFLOAT4 position, XMFLOAT4 forward, XMFLOAT4 up)
{
	this->position = position;
	this->forward = forward;
	this->up = up;
	rotationX = 0;
	rotationY = 0;
	speed = 25.f;
	sens = 0.001f;
	mouseX = 640.f;
	mouseY = 360.f;

	ViewMatrix = XMMatrixLookAtLH({ position.x, position.y, position.z},
		{ forward.x, forward.y, forward.z },
		{up.x, up.y, up.z});

	XMVECTOR xmLookAt = XMLoadFloat4(&forward);
	XMVECTOR xmUp = XMLoadFloat4(&up);
	XMVECTOR xmRight = XMVector3Cross(xmUp, xmLookAt);
	XMStoreFloat4(&right, xmRight);
}

Camera::~Camera()
{
}

void Camera::setPosition(XMFLOAT3 pos)
{
	this->position = { pos.x, pos.y, pos.z, 1.f };
}

void Camera::update(float dt)
{
	XMVECTOR xmForward = XMLoadFloat4(&forward);
	XMVECTOR xmUp = XMLoadFloat4(&up);
	XMVECTOR xmRight =  XMVector3Normalize(XMVector3Cross(xmUp, xmForward));
	XMStoreFloat4(&right, xmRight);

	if (GetAsyncKeyState('W') < 0)
	{
		position = { 
			position.x + (forward.x * speed * dt),
			position.y + (forward.y * speed * dt),
			position.z + (forward.z * speed * dt),
			1.f };
	}

	if (GetAsyncKeyState('S') < 0)
	{
		position = {
			position.x - (forward.x * speed * dt),
			position.y - (forward.y * speed * dt),
			position.z - (forward.z * speed * dt),
			1.f };
	}

	if (GetAsyncKeyState('A') < 0)
	{
		position = {
			position.x - (right.x * speed * dt),
			position.y - (right.y * speed * dt),
			position.z - (right.z * speed * dt),
			1.f };
	}

	if (GetAsyncKeyState('D') < 0)
	{
		position = {
			position.x + (right.x * speed * dt),
			position.y + (right.y * speed * dt),
			position.z + (right.z * speed * dt),
			1.f };
	}

	if (GetAsyncKeyState(VK_SPACE) < 0)
	{
		position = {
			position.x + (up.x * speed * dt),
			position.y + (up.y * speed * dt),
			position.z + (up.z * speed * dt),
			1.f };
	}

	if (GetAsyncKeyState(VK_LCONTROL) < 0)
	{
		position = {
			position.x - (up.x * speed * dt),
			position.y - (up.y * speed * dt),
			position.z - (up.z * speed * dt),
			1.f };
	}

	POINT mousePosition;
	GetCursorPos(&mousePosition);

	if (mousePosition.x != mouseX)
	{
		float difX = mouseX - mousePosition.x;
		rotationX = difX * sens;
		
		XMMATRIX rotation = XMMatrixRotationY(-rotationX);
		xmForward = XMVector3TransformCoord(xmForward, rotation);
		XMStoreFloat4(&forward, xmForward);
	}

	if (mousePosition.y != mouseY)
	{
		float difY = mouseY - mousePosition.y;
		rotationY += difY * sens;

		if (rotationY > 1.f && difY * sens < 0)
			rotationY = 80.f;
		else if (rotationY < -80.f)
			rotationY = -80.f;

		XMMATRIX rotation = XMMatrixRotationAxis(xmRight, -difY * sens);
		xmForward = XMVector3TransformCoord(xmForward, rotation);
		XMStoreFloat4(&forward, xmForward);
	}
	SetCursorPos(mouseX, mouseY);
}

XMVECTOR Camera::getPosition()
{
	return XMLoadFloat4(&position);
}
XMVECTOR Camera::getLookAt()
{
	return XMLoadFloat4(&forward);
}
XMVECTOR Camera::getUpDir()
{
	return XMLoadFloat4(&up);
}

XMMATRIX Camera::getViewMatrix()
{
	ViewMatrix = XMMatrixLookAtLH({ position.x , position.y, position.z },
		{ forward.x + position.x, forward.y + position.y, forward.z + position.z },
		{ up.x, up.y, up.z });
	return ViewMatrix;
}