#include "LogRenderer.h"

#include "World/Player/ClientPlayer.h"

LogRenderer& LogRenderer::Get()
{
	static LogRenderer instance;
	return instance;
}

void LogRenderer::Init()
{
	m_Font.Load("Resources/Fonts/arial.ttf");
}

void LogRenderer::Render()
{
	Camera& camera = ClientPlayer::GetCamera();

	m_Font.RenderBatch(camera.m_UiProjectionMatrix);
}

void LogRenderer::AddEntry(std::string text)
{
	glm::vec2 position = m_PositionOnScreen;
	position.y += m_CurrentYPosition;
	glm::vec2 textSize = m_Font.AddTextToBatch(text, position, m_FontSizeScale, glm::vec3(1.0f), TEXT_ALIGN_LEFT, m_Width);

	m_CurrentYPosition -= textSize.y + m_TextVerticalSpacing; // -Y is downwards, X+ is to the right
}

LogRenderer::~LogRenderer()
{

}