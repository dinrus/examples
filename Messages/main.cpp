#include <CtrlLib/MessageCtrl/MessageCtrl.h>

using namespace Upp;

class Messages : public TopWindow {
	MessageCtrl msg;
	DocEdit editor;
//	Button  button1,  button2;
	
public:
	Messages()
	{
		Title("Боксы Сообщений U++ (Пассивные Уведомления)");
		SetRect(0,0, 640, 480);
		Sizeable().Zoomable().CenterScreen();
		SetMinSize({100, 100});

		auto action = [=](int id) {
			switch(id) {
			case IDYES: PromptOK("Вы выбрали 'да'"); break;
			case IDNO:  PromptOK("Вы выбрали 'нет'"); break;
			}
		};
		
		Add(editor.HSizePosZ().VSizePos(0, 24));
	//	Add(button1.SetLabel("Тест").RightPos(4).BottomPos(4));
	//	Add(button2.SetLabel("Очистить").LeftPos(4).BottomPos(4));

	//	button2 << [=, this] { msg.Clear(this); }; // Selective clearing.
	//	button1 << [=, this] {
			msg.Animation()
			  .Top()
			  .Information(*this, "Это ограниченное временем сообщение, которое исчезнет через 5 секунд.", Null, 5)
			  .OK(*this, "Это сообщение об успехе.")
			  .Warning(*this, "Это предупреждающее сообщение.")
			  .Error(*this, "Это сообщение об ошибке.")
			  .Bottom()
			  .AskYesNo(editor, "This is a question box 'in' the text editor with "
			                   "[^https:www`.ultimatepp`.org^ l`i`n`k]"
			                   " support. Would you like to continue?",
			                   action,
			                   callback(LaunchWebBrowser)
			);
			
	//	};
	}
};

GUI_APP_MAIN
{
	Messages().Run();
}