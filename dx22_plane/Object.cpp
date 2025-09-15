#include "Object.h"
 
using namespace DirectX::SimpleMath;

//�R���X�g���N�^
Object::Object(Camera* cam) :m_Camera(cam)
{

}
//�f�X�g���N�^
Object:: ~Object()
{

}

void Object::SetPosition(const Vector3& pos)
{
	m_Position.x = pos.x;
	m_Position.y = pos.y;
	m_Position.z = pos.z;
}

Vector3 Object::GetPosition() const
{
	return m_Position;
}