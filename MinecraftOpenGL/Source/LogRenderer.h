#pragma once

#include <Graphics/Font.h>

class LogRenderer
{
public:
	static LogRenderer& Get();

	void Init();

	void Render();

	void AddEntry(std::string text);

	~LogRenderer();

private:
	LogRenderer() {};

public:
	int m_Width = 500;
	int m_Height = 720;

	float m_FontSizeScale = 0.35f;
	float m_TextVerticalSpacing = 15.0f;
	float m_TopPadding = 25.0f;
	float m_LeftPadding = 25.0f;

	float m_CurrentYPosition = -m_TopPadding;

	glm::vec2 m_PositionOnScreen = glm::vec2(m_LeftPadding, 0.0f);

	Font m_Font;

	std::vector<std::string> m_Entries;
};