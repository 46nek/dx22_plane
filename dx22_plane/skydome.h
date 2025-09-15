#pragma once
#include "Object.h"

//�O���錾
class Texture;
struct VERTEX_3D;

class Skydome : public Object
{
public:
	// Object�̃R���X�g���N�^�ɍ��킹��Camera*�������Ɏ��
	Skydome(Camera* cam);
	~Skydome();

	// Object�̏������z�֐������ׂăI�[�o�[���C�h
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

private:
	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11Buffer* m_IndexBuffer = nullptr;
	Texture* m_Texture = nullptr;
	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;
	ID3D11InputLayout* m_VertexLayout = nullptr;

	unsigned int m_IndexCount = 0;
};