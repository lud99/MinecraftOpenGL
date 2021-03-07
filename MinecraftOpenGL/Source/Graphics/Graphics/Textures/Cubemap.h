
#include "Texture2D.h"

#include <vector>
#include <string>

class Cubemap : public Texture2D
{
public:
	void Load(std::vector<std::string>& faceFilepaths);

	void Bind();
	void Unbind();
};