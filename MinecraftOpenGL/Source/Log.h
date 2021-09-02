#pragma once

#include <string>
#include "LogRenderer.h"

static void Log(std::string text)
{
	LogRenderer::Get().AddEntry(text);
}