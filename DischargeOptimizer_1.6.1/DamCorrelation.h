#pragma once
#include <cmath>
#include "DataArchive.h"


namespace Seomjin
{
	inline double GetVolume(double& h)
	{
		if (h > g_dataarchive->sj_min_level_limit && h <= 167.00)
		{
			return 1e6 * (0.0004541881 * pow(h, 3) - 0.08933406 * pow(h, 2) - 4.306272 * h + 1126.814);
		}
		else if (h > 167.00 && h <= 183.30)
		{
			return 1e6* (0.0009584451 * pow(h, 3) - 0.2401497 * pow(h, 2) + 4.696622 * h + 1480.881);
		}
		else if (h > 183.30 && h <= 186.85)
		{
			return 1e6 * (0.0002639076 * pow(h, 3) - 0.07490688 * pow(h, 2) + 15.17943 * h - 1715.17);
		}
		else if (h > 186.85 && h <= 189.68)
		{
			return 1e6 * (0.000154666 * pow(h, 3) + 0.005972898 * pow(h, 2) - 1.620837 * h - 687.15);
		}
		else if (h > 189.68 && h <= 194.48)
		{
			return 1e6 * (0.0002238944 * pow(h, 3) - 0.003059854 * pow(h, 2) - 3.877388 * h - 406.5751);
		}
		else if (h > 194.48 && h <= g_dataarchive->sj_max_level_limit)
		{
			return 1e6 * (0.0002509427 * pow(h, 3) - 0.007055291 * pow(h, 2) - 1.173385 * h - 980.2872);
		}
		else if (h > g_dataarchive->sj_max_level_limit)
		{
			return g_dataarchive->sj_max_volume_limit;
			//throw std::runtime_error("[Seomjin::GetVolume] Wrong input");
		}
		else if (h <= g_dataarchive->sj_min_level_limit)
		{
			return g_dataarchive->sj_min_volume_limit;
		}

	}

}


namespace Juam
{
	inline double GetVolume(double& h)
	{
		if (h > g_dataarchive->ja_min_level_limit && h < 70.00)
		{
			return 1e6 * (- 0.000181624 * pow(h, 3) + 0.084298368 * pow(h, 2) - 8.428402292 * h + 241.8057597);
		}
		else if (h >= 70.00 && h < 80.00)
		{
			return 1e6 * (0.003262 * pow(h, 3) - 0.5473632 * pow(h, 2) + 29.3930596 * h - 491.7813485);
		}
		else if (h >= 80.00 && h < 90.00)
		{
			return 1e6 * (0.007127817 * pow(h, 3) - 1.551158508 * pow(h, 2) + 115.8231822 * h - 2961.240545);
		}
		else if (h >= 90.00 && h <= 100.00)
		{
			return 1e6 * (0.011192696 * pow(h, 3) - 2.795831002 * pow(h, 2) + 241.1440929 * h - 7121.619434);
		}
		else if (h >= 100.00 && h < g_dataarchive->ja_max_level_limit)
		{
			return 1e6 * (0.021010978 * pow(h, 3) - 5.93671483 * pow(h, 2) + 574.8306826 * h - 18899.7559);
		}
		else if (h > g_dataarchive->ja_max_level_limit)
		{
			return g_dataarchive->ja_max_volume_limit;
			//throw std::runtime_error("[Juam::GetVolume] Wrong input");
		}
		else if (h <= g_dataarchive->ja_min_level_limit)
		{
			return g_dataarchive->ja_min_volume_limit;
		}
	}

}