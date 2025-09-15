#pragma once
#include "Object.h"

//前方宣言
class Texture;
struct VERTEX_3D;

class Skydome : public Object
{
public:
	// Objectのコンストラクタに合わせてCamera*を引数に取る
	Skydome(Camera* cam);
	~Skydome();

	// Objectの純粋仮想関数をすべてオーバーライド
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