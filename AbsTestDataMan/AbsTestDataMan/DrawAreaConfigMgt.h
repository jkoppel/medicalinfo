#pragma once

#define MAX_COLOR_NUM	32
#define DRAWAREA_CONFIG_FILE	"DrawArea.dat"

struct DrawAreaConfig
{
	COLORREF crSpeedCurve[MAX_COLOR_NUM];
};

const COLORREF DEFAULT_SPEED_COLOR[] = {
		RGB(255,0,0),RGB(0,255,0),RGB(0,0,255),
		RGB(255,255,0),RGB(255,0,255), RGB(0,255,255),
		0x8B0000, 0xFF4500, 0xFF8C00,
		0x008000, 0x7CFC00, 0x00008B,
		0x9400D3, 0xDC143C, 0x1E90FF,
		0x32CD32, 0xADFF2F,
};

class CDrawAreaConfigMgt
{
public:
	CDrawAreaConfigMgt(void);
	~CDrawAreaConfigMgt(void);
	void GetDrawAreaConfig(struct DrawAreaConfig &config);
	void SetDrawAreaConfig(struct DrawAreaConfig config);
};
