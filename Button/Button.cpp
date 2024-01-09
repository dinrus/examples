#include <CtrlLib/CtrlLib.h>

// http://java.sun.com/docs/books/tutorial/uiswing/start/swingTour.html

using namespace Upp;

struct ButtonApp : TopWindow {
	int    count;
	Button button;
	Label  label;

	void RefreshLabel()
	{
		label = Format("Число нажатий кнопки %d", count);
	}
	void Click()
	{
		++count;
		RefreshLabel();
	}

	typedef ButtonApp CLASSNAME;

	ButtonApp()
	{
		count = 0;
		button <<= THISBACK(Click);
		button.SetLabel("&Я кнопалка Ultimate++!");
		Add(button.VCenterPos(20).HCenterPos(200));
		Add(label.BottomPos(0, 20).HCenterPos(200));
		label.SetAlign(ALIGN_CENTER);
		Sizeable().Zoomable();
		RefreshLabel();
	}
};

GUI_APP_MAIN
{
	ButtonApp().Run();
}
