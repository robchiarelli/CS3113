class SheetSprite{
public:
	SheetSprite();
	SheetSprite(unsigned int textureID, float u, float v, float width, float height);

	void Draw(float scale);

	float scale;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
};