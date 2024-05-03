#include <iostream>
#include <thread>
#include <Windows.h>
#include <unistd.h>
#include <filesystem>
#define NEW_FILE 1
#define OPEN_FILE 2
#define OPEN_RECENT_FILE 3
#define SAVE_FILE 4
#define SAVE_AS_FILE 5
#define EXIT_APP 6

struct WindowStuff
{
	bool running = true;
};

WindowStuff windowStuff;
HWND hTextEdit;
char RecentFiles[3][255] = {"", "", ""};
bool NotifyUserOfPath = false;


LRESULT windProc(HWND wind, UINT msg, WPARAM wp, LPARAM lp);
void AddGUI(HWND);
void AddMenus(HWND);
void SaveTextFile();
void SaveTextFileNoPrompt();
void OpenTextFile();

int main()
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);
	WNDCLASS wc = {sizeof(WNDCLASS)};

    wc.hIcon = LoadIcon(NULL, IDI_WARNING);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hInstance = GetModuleHandle(0);
	wc.lpszClassName = "EpicWindowClass";
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.style = 0;
	wc.lpfnWndProc = windProc;

	if (!RegisterClass(&wc)) { return 0; };


	HWND wind = CreateWindowA(
		wc.lpszClassName,
		"WINDOW ! ! !",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		0, 0, GetModuleHandle(0), 0);

	while (windowStuff.running)
	{

		MSG msg = {};
		while (PeekMessage(&msg, wind, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
        //kod dla okna
	}
	return 0;
}

LRESULT windProc(HWND wind, UINT msg, WPARAM wp, LPARAM lp)
{
	LRESULT rez = 0;

	switch (msg)
	{
    case WM_CREATE:
        AddMenus(wind);
        AddGUI(wind);
        break;
    case WM_COMMAND:
        switch(wp)
        {
        case NEW_FILE:
            SetWindowText(hTextEdit, "");
            break;
        case OPEN_FILE:
            OpenTextFile();
            break;
        case SAVE_FILE:
            SaveTextFileNoPrompt();
            if(!NotifyUserOfPath)
            {
                char path[256];
                getcwd(path, 255);

                MessageBox(wind, path, "Where is my file?", NULL);
                NotifyUserOfPath = true;
            }
            break;
        case SAVE_AS_FILE:
            SaveTextFile();
            break;
        case EXIT_APP:
            DestroyWindow(GetConsoleWindow());
            DestroyWindow(wind);
            break;
        }
        break;
	case WM_CLOSE:
	    windowStuff.running = false;
		PostQuitMessage(0);
	break;

	default:
	    std::cout << msg << std::endl;
		rez = DefWindowProc(wind, msg, wp, lp);
	break;
	}

	return rez;
}

void AddGUI(HWND wind)
{
    hTextEdit = CreateWindowW(L"Edit", NULL, WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL, 5, 5, 1280, 800, wind, NULL, NULL, NULL);
}

void AddMenus(HWND wind)
{
    HMENU hRecentFiles = CreateMenu();
    HMENU hFileSubmenu = CreateMenu();
    HMENU hFileMenu = CreateMenu();

    AppendMenu(hRecentFiles, MF_STRING, OPEN_RECENT_FILE, "Recent File Here");
    AppendMenu(hFileSubmenu, MF_STRING, NEW_FILE, "New");
    AppendMenu(hFileSubmenu, MF_STRING, OPEN_FILE, "Open");
    AppendMenu(hFileSubmenu, MF_POPUP, (UINT_PTR)hRecentFiles, "Open Recent");
    AppendMenu(hFileSubmenu, MF_STRING, SAVE_FILE, "Save");
    AppendMenu(hFileSubmenu, MF_STRING, SAVE_AS_FILE, "Save as..");

    AppendMenu(hFileSubmenu, MF_SEPARATOR, NULL, NULL);

    AppendMenu(hFileSubmenu, MF_STRING, EXIT_APP, "Exit");
    AppendMenu(hFileMenu, MF_POPUP, (UINT_PTR)hFileSubmenu, "File");

    SetMenu(wind, hFileMenu);
}

void OpenTextFile()
{
    SetWindowText(hTextEdit, "");
    OPENFILENAME ofn;

    char file_name[255];

    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFilter = "\0";
    ofn.lpstrTitle = "Open a file";
    ofn.lpstrFile = file_name;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 100;
    ofn.nFilterIndex = 1;
    if(GetOpenFileName(&ofn))
    {
        //RecentFiles[0] = ofn.lpstrFile;
        FILE *file;
        file = fopen(ofn.lpstrFile, "r");
        fseek(file, 0, SEEK_END);
        int _size = ftell(file);
        rewind(file);
        char *data = (char *)malloc(_size+1);
        fread(data, _size, 1, file);
        data[_size] = '\0';

        SetWindowText(hTextEdit, data);
        free(data);
        fclose(file);
    }
}
void SaveTextFile()
{
    OPENFILENAME ofn;

    char file_name[255];

    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFilter = "\0";
    ofn.lpstrTitle = "Open a file";
    ofn.lpstrFile = file_name;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 100;
    ofn.nFilterIndex = 1;
    GetSaveFileName(&ofn);

    FILE *file;
    file = fopen(ofn.lpstrFile, "w");

    int _size = GetWindowTextLength(hTextEdit);
    char *data = new char[_size+1];
    GetWindowText(hTextEdit, data, _size+1);

    fwrite(data, _size+1, 1, file);
    fclose(file);

}

void SaveTextFileNoPrompt()
{

    FILE *file;
    file = fopen("file.txt", "w");

    int _size = GetWindowTextLength(hTextEdit);
    char *data = new char[_size+1];
    GetWindowText(hTextEdit, data, _size+1);

    fwrite(data, _size+1, 1, file);
    fclose(file);

}
