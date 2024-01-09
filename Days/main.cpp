#include "Days.h"

Days::Days()
{
	CtrlLayout(*this, "Дни");
	date1 ^= date2 ^= [=, this] {
		result = IsNull(date1) || IsNull(date2) ? "" :
		         Format("%d дня(-ей) между %` и %`", abs(Date(~date1) - Date(~date2)), ~date1, ~date2);
	};
}

GUI_APP_MAIN
{
	Days().Run();
}
