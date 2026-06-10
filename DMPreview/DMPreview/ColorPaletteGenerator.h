#pragma once

class ColorPaletteGenerator{
	ColorPaletteGenerator();

	static float maxHueAngle ;
	static int defaultAlpha;
public:
	//Generate a color mapping palette RGBA
	//static void generatePalette(BYTE * palette, int paletteSize, bool reverseRedToBlue = true);

	//controlPoint1Value = 0.1f , controlPoint2Value = 0.9f
	//static void generatePalette(BYTE * palette, int paletteSize, int controlPoint1, int controlPoint2, bool reverseRedToBlue = true);

	static void generatePalette(BYTE * palette, int paletteSize, int controlPoint1, float controlPoint1Value, int controlPoint2, float controlPoint2Value, bool reverseRedToBlue = true);

	//Generate a gray mapping palette RGBA
	//static void generatePaletteGray(BYTE * palette, int paletteSize, bool reverseGraylevel = true);

	//controlPoint1Value = 0.1f , controlPoint2Value = 0.9f
	//static void generatePaletteGray(BYTE * palette, int paletteSize, int controlPoint1, int controlPoint2, bool reverseGraylevel = true);

	static void generatePaletteGray(BYTE * palette, int paletteSize, int controlPoint1, float controlPoint1Value, int controlPoint2, float controlPoint2Value, bool reverseGraylevel = true);
private:
	//value range: 0.0f ~ 1.0f as hue angle
	static void getRGB(float value, float &R, float &G, float &B, bool reverseRedToBlue = true);
	static void HSV_to_RGB(float H, float S, float V, float &R, float &G, float &B);
};