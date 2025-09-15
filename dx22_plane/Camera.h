#pragma once

#include	<SimpleMath.h>

//-----------------------------------------------------------------------------
//CameraƒNƒ‰ƒX
//-----------------------------------------------------------------------------
class Camera {
private:
	DirectX::SimpleMath::Vector3	m_Position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	DirectX::SimpleMath::Vector3	m_Rotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	DirectX::SimpleMath::Vector3	m_Scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);


	DirectX::SimpleMath::Vector3	m_Target{};
	DirectX::SimpleMath::Matrix		m_ViewMatrix{};

	float m_CameraDirection = 0;

public:


	DirectX::SimpleMath::Vector3 GetPosition() const { return m_Position; }

	void Init();
	void Update();
	void Draw();
	void Uninit();

	void SetCamera(int mode);
};