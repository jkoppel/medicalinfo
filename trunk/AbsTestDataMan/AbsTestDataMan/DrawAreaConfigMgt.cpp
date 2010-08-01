#include "StdAfx.h"
#include "DrawAreaConfigMgt.h"

CDrawAreaConfigMgt::CDrawAreaConfigMgt(void)
{
	CFile file;
	struct DrawAreaConfig config;

	memset(&config, 0, sizeof(struct DrawAreaConfig));
	memcpy(config.crSpeedCurve, DEFAULT_SPEED_COLOR, sizeof(DEFAULT_SPEED_COLOR));

	CFileFind ff;
	if(!ff.FindFile(_T(DRAWAREA_CONFIG_FILE))){
		if(!file.Open(_T(DRAWAREA_CONFIG_FILE), CFile::modeNoTruncate | CFile::modeCreate | CFile::modeWrite)){
			return;
		}
		file.Write(&config, sizeof(struct DrawAreaConfig));
		file.Close();
	}
	ff.Close();
}

CDrawAreaConfigMgt::~CDrawAreaConfigMgt(void)
{
}

void CDrawAreaConfigMgt::GetDrawAreaConfig(struct DrawAreaConfig &config)
{
	CFile file;
	memset(&config, 0, sizeof(struct DrawAreaConfig));
	if(!file.Open(_T(DRAWAREA_CONFIG_FILE), CFile::modeNoTruncate | CFile::modeCreate)){
		return;
	}
	int ret = file.Read(&config, sizeof(struct DrawAreaConfig));
	file.Close();
}

void CDrawAreaConfigMgt::SetDrawAreaConfig(struct DrawAreaConfig config)
{
	CFile file;

	if(!file.Open(_T(DRAWAREA_CONFIG_FILE), CFile::modeCreate | CFile::modeWrite)){
		return;
	}
	file.Write(&config, sizeof(struct DrawAreaConfig));
	file.Close();
}
