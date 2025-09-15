#pragma once
#include "Scene.h"
#include "Object.h"

// Stage1Scenクラス
class Stage1Scene : public Scene
{
private:
	std::vector<Object*> m_MySceneObjects; // このシーンのオブジェクト

	int m_State = 0;

	void Init(); // 初期化
	void Uninit(); // 終了処理

	int m_Par;
	int m_StrokeCount;

public:
	Stage1Scene(); // コンストラクタ
	~Stage1Scene(); // デストラクタ

	void Update(); // 更新

	int GetScore();
};

