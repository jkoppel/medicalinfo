#include "StdAfx.h"
#include "FilterConfigMgt.h"

CFilterConfigMgt::CFilterConfigMgt(void)
{
}

CFilterConfigMgt::~CFilterConfigMgt(void)
{
}

void CFilterConfigMgt::GetFilterConfig(struct FilterConfig &config)
{
	CFile file;
	memset(&config, 0, sizeof(struct FilterConfig));
	if(!file.Open(_T(FILTER_CONFIG_FILE), CFile::modeNoTruncate | CFile::modeCreate)){
		return;
	}
	file.Read(&config, sizeof(struct FilterConfig));
	file.Close();
}

void CFilterConfigMgt::SetFilterConfig(struct FilterConfig config)
{
	CFile file;

	if(!file.Open(_T(FILTER_CONFIG_FILE), CFile::modeCreate | CFile::modeWrite)){
		return;
	}
	file.Write(&config, sizeof(struct FilterConfig));
	file.Close();
}
