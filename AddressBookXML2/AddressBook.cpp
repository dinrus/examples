#include <CtrlLib/CtrlLib.h>
#include <Report/Report.h>

using namespace Upp;

#define LAYOUTFILE <AddressBookXML2/AddressBook.lay>
#include <CtrlCore/lay.h>

#define TAG_PHONEBOOK "AddressBook"
#define TAG_PERSON "person"
#define TAG_NAME "name"
#define TAG_SURNAME "surname"
#define TAG_PHONE "phone"
#define TAG_EMAIL "email"

class AddressBook : public WithAddressBookLayout<TopWindow> {
	WithModifyLayout<ParentCtrl> modify;
	WithSearchLayout<ParentCtrl> search;
	FileSel fs;
	String  filename;

	void SetupSearch();
	void Add();
	void Change();
	void Search();
	void Open();
	void Save();
	void SaveAs();
	void Print();
	void Quit();
	void FileMenu(Bar& bar);
	void MainMenu(Bar& bar);

	typedef AddressBook CLASSNAME;

public:
	void Serialize(Stream& s);

	AddressBook();
};

AddressBook::AddressBook()
{
	CtrlLayout(*this, "Контакты");
	CtrlLayout(modify);
	CtrlLayout(search);
	tab.Add(modify, "Изменить");
	tab.Add(search, "Поиск");
	ActiveFocus(search.name);
	search.oname = true;
	search.oname <<= search.osurname <<= search.oaddress
	             <<= search.oemail <<= THISBACK(SetupSearch);
	array.AddColumn(TAG_NAME, "Имя");
	array.AddColumn(TAG_SURNAME, "Фамилия");
	array.AddColumn(TAG_PHONE, "Телефон");
	array.AddColumn(TAG_EMAIL, "Имейл");
	modify.add <<= THISBACK(Add);
	modify.change <<= THISBACK(Change);
	search.search <<= THISBACK(Search);
	SetupSearch();
	fs.AllFilesType();
	menu.Set(THISBACK(MainMenu));
}

void AddressBook::FileMenu(Bar& bar)
{
	bar.Add("Открыть..", CtrlImg::open(), THISBACK(Open));
	bar.Add("Сохранить", CtrlImg::save(), THISBACK(Save));
	bar.Add("Сохранить как...", CtrlImg::save_as(), THISBACK(SaveAs));
	bar.Separator();
	bar.Add("Печать", CtrlImg::print(), THISBACK(Print));
	bar.Separator();
	bar.Add("Выход", THISBACK(Quit));
}

void AddressBook::MainMenu(Bar& bar)
{
	bar.Add("Файл", THISBACK(FileMenu));
}

void AddressBook::SetupSearch()
{
	search.name.Enable(search.oname);
	search.surname.Enable(search.osurname);
	search.address.Enable(search.oaddress);
	search.email.Enable(search.oemail);
}

void AddressBook::Add()
{
	array.Add(~modify.name, ~modify.surname, ~modify.address, ~modify.email);
	array.GoEnd();
	modify.name <<= modify.surname <<= modify.address <<= modify.email <<= Null;
	ActiveFocus(modify.name);
}

void AddressBook::Change()
{
	if(array.IsCursor()) {
		array.Set(0, ~modify.name);
		array.Set(1, ~modify.surname);
		array.Set(2, ~modify.address);
		array.Set(3, ~modify.email);
	}
}

bool Contains(const String& text, const String& substr)
{
	for(const char *s = text; s <= text.End() - substr.GetLength(); s++)
		if(strncmp(s, substr, substr.GetLength()) == 0)
			return true;
	return false;
}

void AddressBook::Search()
{
	if(!array.GetCount()) return;
	bool sc = true;
	array.ClearSelection();
	for(int i = 0; i < array.GetCount(); i++) {
		if((!search.oname || Contains(array.Get(i, 0), ~search.name)) &&
		   (!search.osurname || Contains(array.Get(i, 1), ~search.surname)) &&
		   (!search.oaddress || Contains(array.Get(i, 2), ~search.address)) &&
		   (!search.oemail || Contains(array.Get(i, 3), ~search.email))) {
			array.Select(i);
			if(sc) {
				array.SetCursor(i);
				array.CenterCursor();
				sc = false;
			};
		}
	}
}

void AddressBook::Open()
{
	if(!fs.ExecuteOpen()) return;
	filename = fs;
	array.Clear();
	try {
		String d = LoadFile(filename);
		XmlNode n = ParseXML(d);
		if(n.GetCount()==0 || n[0].GetTag() != TAG_PHONEBOOK)
			throw XmlError("Нет тега AddressBook");
		const XmlNode &ab = n[0];
		for(int i = 0; i < ab.GetCount(); i++) {
			const XmlNode &person = ab[i];
			if(person.IsTag(TAG_PERSON))
				array.Add(person[TAG_NAME].GatherText(),
				          person[TAG_SURNAME].GatherText(),
				          person[TAG_PHONE].GatherText(),
				          person[TAG_EMAIL].GatherText());
		}
	}
	catch(XmlError &e) {
		Exclamation("Ошибка при чтении входного файла:&\1" + e);
	}
}

void AddressBook::Save()
{
	if(IsEmpty(filename)) {
		SaveAs();
		return;
	}
	XmlNode n;
	XmlNode &ab = n.Add(TAG_PHONEBOOK);
	for(int i=0; i < array.GetCount(); i++) {
		XmlNode &person = ab.Add(TAG_PERSON);
		person.Add(TAG_NAME).Add().CreateText(array.Get(i, TAG_NAME));
		person.Add(TAG_SURNAME).Add().CreateText(array.Get(i, TAG_SURNAME));
		person.Add(TAG_PHONE).Add().CreateText(array.Get(i, TAG_PHONE));
		person.Add(TAG_EMAIL).Add().CreateText(array.Get(i, TAG_EMAIL));
	}
	if(!SaveFile(filename, AsXML(n)))
		Exclamation("Ошибка при сохранении файла!");
}

void AddressBook::SaveAs()
{
	if(!fs.ExecuteSaveAs()) return;
	filename = fs;
	Save();
}

void AddressBook::Print()
{
	String qtf;
	qtf = "{{1:1:1:1 Имя:: Фамилия:: Телефон:: Имейл";
	for(int i = 0; i < array.GetCount(); i++)
		for(int q = 0; q < 4; q++)
			qtf << ":: " << DeQtf((String)array.Get(i, q));
	Report report;
	report << qtf;
	Perform(report);
}

void AddressBook::Quit()
{
	Break();
}

void AddressBook::Serialize(Stream& s)
{
	int version = 0;
	s / version;
	s % search.oname % search.osurname % search.oaddress % search.oemail;
	s % fs;
	SetupSearch();
}

GUI_APP_MAIN
{
	AddressBook ab;
	LoadFromFile(ab);
	ab.Run();
	StoreToFile(ab);
}
