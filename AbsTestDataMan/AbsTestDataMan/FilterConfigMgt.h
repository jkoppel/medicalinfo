#pragma once

#define MAX_PRODUCTORNO_LEN	32
#define MAX_OPERATORNO_LEN	32
#define FILTER_CONFIG_FILE	"Filter.dat"

struct FilterConfig
{
	BOOL bCheckTime;
	SYSTEMTIME StartTime;
	SYSTEMTIME EndTime;
	char sProductNo[MAX_PRODUCTORNO_LEN+1];
	char sOperatorNo[MAX_PRODUCTORNO_LEN+1];
};

class CFilterConfigMgt
{
public:
	CFilterConfigMgt(void);
	~CFilterConfigMgt(void);
	void GetFilterConfig(struct FilterConfig &config);
	void SetFilterConfig(struct FilterConfig config);
};
