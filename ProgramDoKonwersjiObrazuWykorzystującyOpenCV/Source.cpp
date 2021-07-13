#include <vector>
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <tchar.h>
#include <ShObjIdl.h>
#include <ShObjIdl_core.h>
#include <opencv2/imgcodecs.hpp>
#include <sstream>

using namespace cv;
using namespace std;

static TCHAR szWindowClass[] = _T("DesktopApp");

static TCHAR szTitle[] = _T("Program do konwersji i kompresji obrazów wykorzystuj¹cy bibliotekê OpenCV");

HINSTANCE hInst;

HWND CloseProgram, LoadFile, Help, Konwertuj, NazwaPlikuL, NazwaPliku, Czas, CzasL, BMP, JPEG, JPEG2000, PNG, PBM, SR, TIFFI, JPEG_QUALITY, PNG_QUALITY, PBM_QUALITY, GRAY_SCALE, JPEG2000_QUALITY, NAZWA_PLIKU, ComboRead, ComboPNG, PokazPlik;
int IDBMP = 1, IDJPEG = 2, IDJPEG2000 = 3, IDPNG = 4, IDPBM = 5, IDSR = 6, IDTIFF = 7, IDJPEG_QUALITY = 8, IDPNG_QUALITY = 9, IDPBM_QUALITY = 10, IDGRAY_SCALE = 11, ID_COMBORD = 12, ID_COMBOPNG = 13;
BOOL checked1, checked2, checked3, checked4, checked5, checked6, checked7, checked8;

OPENFILENAME ofn;
char szFile[MAX_PATH];
HANDLE hf;

DWORD dlugosc;
LPSTR Bufor;

vector<HWND>przyciski;

string path, path2, NazwaKoncowa;

Mat mat;

int DesktopWidth, DesktopHeight;
int WindowWidth = 680, WindowHeight = 580;
int JPEG_Q = 0, PNG_Q = 0, PBM_Q = 0, JPEG2000_Q = 0;
int tryb_odczytu, tryb_zapisu;

BOOL WczytanoPlik = FALSE;
BOOL UkonczonoKonwersje = FALSE;

LRESULT CALLBACK WndProc(_In_ HWND   hWnd, _In_ UINT   message, _In_ WPARAM wParam, _In_ LPARAM lParam);

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	setlocale(LC_ALL, "");
	DesktopWidth = GetSystemMetrics(SM_CXSCREEN);
	DesktopHeight = GetSystemMetrics(SM_CYSCREEN);

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Wyst¹pi³ problem z rejestracj¹ okna!"),
			_T("B³¹d"),
			NULL);

		return 1;
	}

	hInst = hInstance;

	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WindowWidth,
		WindowHeight,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Utworzenie okna nie powiod³o siê!"),
			_T("B³¹d"),
			NULL);

		return 1;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		HFONT hFont = CreateFontA(
			0,
			0,
			0,
			0,
			FW_DEMIBOLD,
			FALSE,
			FALSE,
			FALSE,
			ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH,
			FF_DONTCARE
		);

		// Ustawienie pozycji okna programu
		SetWindowPos(hWnd, HWND_TOP, (DesktopWidth / 8) + (int)((double)DesktopWidth / 5), DesktopHeight / 4, WindowWidth, WindowHeight, NULL);

		// Przyciski kontroluj¹ce dzia³anie programu
		przyciski.push_back(Help = CreateWindow(_T("BUTTON"),
			_T("Pomoc"),
			WS_CHILD | WS_VISIBLE | BS_PUSHLIKE,
			450,
			80,
			190,
			30,
			hWnd,
			NULL,
			NULL,
			NULL));
		przyciski.push_back(Konwertuj = CreateWindow(_T("BUTTON"), _T("Konwertuj"), WS_CHILD | WS_VISIBLE | BS_PUSHLIKE, 230, 480, 190, 30, hWnd, NULL, NULL, NULL));
		przyciski.push_back(CloseProgram = CreateWindow(_T("BUTTON"), _T("Zakoñcz"), WS_CHILD | WS_VISIBLE | BS_PUSHLIKE, 450, 480, 190, 30, hWnd, NULL, NULL, NULL));
		przyciski.push_back(LoadFile = CreateWindow(_T("BUTTON"), _T("Wczytaj Obraz"), WS_CHILD | WS_VISIBLE | BS_PUSHLIKE, 230, 80, 190, 30, hWnd, NULL, NULL, NULL));
		przyciski.push_back(PokazPlik = CreateWindow(_T("BUTTON"), _T("Wyœwietl Obraz"), WS_CHILD | WS_VISIBLE | BS_PUSHLIKE, 20, 480, 180, 30, hWnd, NULL, NULL, NULL));

		// Combobox z mo¿liwymi trybami odzczytu koloru i kompresji PNG
		przyciski.push_back(ComboRead = CreateWindow(WC_COMBOBOX, TEXT(""), CBS_DROPDOWN | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 20, 80, 200, 230, hWnd, (HMENU)ID_COMBORD, NULL, NULL));
		przyciski.push_back(ComboPNG = CreateWindow(WC_COMBOBOX, TEXT(""), CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 450, 280, 190, 230, hWnd, (HMENU)ID_COMBOPNG, NULL, NULL));

		// Przyciski typu checkbox do ustawiania jaki format pliku chcemy otrzymaæ
		przyciski.push_back(BMP = CreateWindow(_T("BUTTON"), _T("BMP"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 20, 130, 180, 30, hWnd, (HMENU)IDBMP, NULL, NULL));
		przyciski.push_back(JPEG = CreateWindow(_T("BUTTON"), _T("JPEG"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 20, 180, 180, 30, hWnd, (HMENU)IDJPEG, NULL, NULL));
		przyciski.push_back(JPEG2000 = CreateWindow(_T("BUTTON"), _T("JPEG2000"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 20, 230, 180, 30, hWnd, (HMENU)IDJPEG2000, NULL, NULL));
		przyciski.push_back(PNG = CreateWindow(_T("BUTTON"), _T("PNG"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 20, 280, 180, 30, hWnd, (HMENU)IDPNG, NULL, NULL));
		przyciski.push_back(TIFFI = CreateWindow(_T("BUTTON"), _T("TIFF"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 20, 330, 180, 30, hWnd, (HMENU)IDTIFF, NULL, NULL));

		// Kontrolki niedytowalne, tylko do odczytu
		przyciski.push_back(NazwaPlikuL = CreateWindowEx(WS_EX_TRANSPARENT, _T("STATIC"), _T("Podaj nazwê pliku"), WS_CHILD | WS_VISIBLE | SS_CENTER | BS_CENTER | WS_SYSMENU, 450, 380, 190, 30, hWnd, NULL, NULL, NULL));
		przyciski.push_back(CzasL = CreateWindowEx(WS_EX_TRANSPARENT, _T("STATIC"), _T("Czas ukoñczenia konwersji"), WS_CHILD | WS_VISIBLE | SS_CENTER | BS_CENTER | WS_SYSMENU, 230, 380, 190, 30, hWnd, NULL, NULL, NULL));
		przyciski.push_back(Czas = CreateWindowEx(WS_EX_CLIENTEDGE, _T("STATIC"), NULL, WS_CHILD | WS_VISIBLE | SS_CENTER | BS_CENTER | WS_BORDER, 230, 430, 190, 30, hWnd, NULL, NULL, NULL));

		// Pola tekstowe
		przyciski.push_back(JPEG_QUALITY = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 230, 180, 190, 30, hWnd, (HMENU)IDJPEG_QUALITY, NULL, NULL));
		przyciski.push_back(PNG_QUALITY = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 230, 280, 190, 30, hWnd, (HMENU)IDPNG_QUALITY, NULL, NULL));
		przyciski.push_back(JPEG2000_QUALITY = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 230, 230, 190, 30, hWnd, (HMENU)IDPBM_QUALITY, NULL, NULL));
		przyciski.push_back(NazwaPliku = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 450, 430, 190, 30, hWnd, (HMENU)NAZWA_PLIKU, NULL, NULL));

		// Stany pocz¹tkowe kontrolek
		for (int i = 0; i < przyciski.size(); i++)
		{
			EnableWindow(przyciski[i], FALSE);
		}
		EnableWindow(ComboRead, TRUE);
		EnableWindow(LoadFile, TRUE);
		EnableWindow(CloseProgram, TRUE);
		EnableWindow(Help, TRUE);

		// Ustawianie w³asnej czcionki do wyœwietlania tekstu w przyciskach
		for (int i = 0; i < przyciski.size(); i++)
		{
			SendMessage(przyciski[i], WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
		}

		SendMessage(ComboRead, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"UNCHANGED");
		SendMessage(ComboRead, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"GRAYSCALE");
		SendMessage(ComboRead, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"COLOR");
		SendMessage(ComboRead, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"ANYDEPTH");
		SendMessage(ComboRead, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"ANYCOLOR");
		SendMessage(ComboRead, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"REDUCED_GRAYSCALE_2");
		SendMessage(ComboRead, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"REDUCED_COLOR_2");
		SendMessage(ComboRead, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"REDUCED_GRAYSCALE_4");
		SendMessage(ComboRead, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"REDUCED_COLOR_4");
		SendMessage(ComboRead, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"REDUCED_GRAYSCALE_8");
		SendMessage(ComboRead, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"REDUCED_COLOR_8");
		SendMessage(ComboRead, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);

		SendMessage(ComboPNG, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"DEFAULT");
		SendMessage(ComboPNG, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"FILTERED");
		SendMessage(ComboPNG, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"HUFFMAN");
		SendMessage(ComboPNG, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"RLE");
		SendMessage(ComboPNG, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"FIXED");
		SendMessage(ComboPNG, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);

		MessageBox(NULL,
			_T("Przed rozpoczêciem pracy z programem zapoznaj siê z pomoc¹\ndla u¿ytkownika pod przyciskiem Pomoc."),
			_T("Informacja"),
			MB_OK);
	}
	break;
	case WM_COMMAND:
	{
		if ((HWND)lParam == NazwaPlikuL)
		{
			break;
		}
		if ((HWND)lParam == CzasL)
		{
			break;
		}
		if ((HWND)lParam == Czas)
		{
			break;
		}
		// Obs³uga przycisku do wyœwietlenia wczytanego pliku
		if ((HWND)lParam == PokazPlik)
		{
			if (WczytanoPlik)
			{
				namedWindow("Wybrany obraz", WINDOW_NORMAL);
				imshow("Wybrany obraz", mat);
				resizeWindow("Wybrany obraz", 1024, 880);
			}
			else MessageBox(NULL, _T("Brak wczytanego pliku."), _T("B³¹d"), MB_OK);
			break;
		}
		// Wyœwietl okno z zakoñczeniem pracy z programem
		if ((HWND)lParam == CloseProgram)
		{
			int msgboxID = MessageBox(NULL, _T("Czy chcesz zakoñczyæ program?"), _T("Koniec"), MB_YESNO | MB_TOPMOST);

			if (msgboxID == IDYES)
			{
				PostQuitMessage(0);
			}

			if (msgboxID == IDNO)
			{
				WczytanoPlik = false;

				if (path.size() > 0)
				{
					WczytanoPlik = true;
				}
			}
			break;
		}
		// Wyœwietlenie przycisku pomocy z opisem dzia³ania programu
		if ((HWND)lParam == Help)
		{
			int msgboxID = MessageBox(NULL,
				_T("Program do kompresji i konwersji obrazów.\n\nAby zacz¹æ pracê z programem nale¿y wybraæ tryb odczytu obrazu oraz wczytaæ plik, który chcemy przekonwertowaæ wtedy uzyskamy dostêp do wszystkich mo¿liwoœci konwertowania pliku.\n\nW przypadku konwersji do JPEG, JPEG 2000 lub PNG nale¿y wybraæ wartoœæ odpowiedni¹ dla ka¿dego typu konwersji:\n- JPEG \t\t0 - 100 / Im wy¿sza wartoœæ tym lepszy wygl¹d obrazu\n- JPEG 2000 \t0 - 1000\n- PNG \t\t0 - 9 / Im wy¿sza wartoœæ tym lepsza kompresja\n\nW przypadku kompresji PNG nale¿y wybraæ jeszcze typ kompresji jeœli nie mamy czasu na testy najlepszym wyborem jest kompresja RLE.\n\nNastêpnie nale¿y podaæ nazwê pliku jaki zostanie zapisany na dysku w folderze w którym znajduje siê oryginalny plik, rozszerzenie jest dopisywane na podstawie wybranego typu konwersji.\n\nJeœli zosta³y wykonane wszystkie wymagane czynnoœci mo¿na przyst¹piæ do konwersji pliku.\n\nPo przetworzeniu pliku wczytaj nastêpny plik aby kontynuowaæ pracê programu lub zakoñcz go u¿ywaj¹c przycisku zakoñcz."),
				_T("Pomoc"),
				MB_OK);

			if (msgboxID == IDOK)
			{
				WczytanoPlik = false;

				if (path.size() > 0)
				{
					WczytanoPlik = true;
				}
			}
			break;
		}
		// Obs³uga przycisku wczytywania pliku
		if ((HWND)lParam == LoadFile)
		{
			LPCSTR typ_plikow = "Windows Bitmaps\0*.BMP;*.DIB\0JPEG files\0*.JPEG;*.JPG;*.JPE\0JPEG 2000 files\0*.jp2\0Portable Network Graphics\0*.PNG\0TIFF files\0*.TIFF;*.TIF\0\0";
			// Inicjalizacja struktury openfilename
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = szFile;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = typ_plikow;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			if (GetOpenFileName(&ofn) == TRUE)
				hf = CreateFile(ofn.lpstrFile,
					GENERIC_READ,
					0,
					(LPSECURITY_ATTRIBUTES)NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					(HANDLE)NULL);
			else break;
			path = ofn.lpstrFile;
			CloseHandle(hf);
			// Jeœli œcie¿ka pliku zawiera dane mo¿emy przyst¹piæ do dalszego dzia³ania programu
			if (path.size() > 0)
			{
				SetWindowText(Czas, "");
				WczytanoPlik = true;
			}
			// Gdy wczytamy plik i wszystko przebiegnie poprawnie przechodzimy do dalszego etapu dzia³ania programu
			if (WczytanoPlik)
			{
				char msg[1000];
				HWND hGetCombo = GetDlgItem(hWnd, ID_COMBORD);
				int nIndex = ::SendMessage(hGetCombo, CB_GETCURSEL, 0, 0);
				SendMessage(hGetCombo, CB_GETLBTEXT, (WPARAM)nIndex, (LPARAM)msg);

				if (strcmp(msg, "UNCHANGED") == 0) tryb_odczytu = IMREAD_UNCHANGED;
				if (strcmp(msg, "GRAYSCALE") == 0) tryb_odczytu = IMREAD_GRAYSCALE;
				if (strcmp(msg, "COLOR") == 0) tryb_odczytu = IMREAD_COLOR;
				if (strcmp(msg, "ANYDEPTH") == 0) tryb_odczytu = IMREAD_ANYDEPTH;
				if (strcmp(msg, "ANYCOLOR") == 0) tryb_odczytu = IMREAD_ANYCOLOR;
				if (strcmp(msg, "REDUCED_GRAYSCALE_2") == 0) tryb_odczytu = IMREAD_REDUCED_GRAYSCALE_2;
				if (strcmp(msg, "REDUCED_COLOR_2") == 0) tryb_odczytu = IMREAD_REDUCED_COLOR_2;
				if (strcmp(msg, "REDUCED_GRAYSCALE_4") == 0) tryb_odczytu = IMREAD_REDUCED_GRAYSCALE_4;
				if (strcmp(msg, "REDUCED_COLOR_4") == 0) tryb_odczytu = IMREAD_REDUCED_COLOR_4;
				if (strcmp(msg, "REDUCED_GRAYSCALE_8") == 0) tryb_odczytu = IMREAD_REDUCED_GRAYSCALE_8;
				if (strcmp(msg, "REDUCED_COLOR_8") == 0) tryb_odczytu = IMREAD_REDUCED_COLOR_8;

				mat = imread(path, tryb_odczytu);
			}
		}
		// Sprawdzenie stanu kontrolek
		checked1 = IsDlgButtonChecked(hWnd, IDBMP);
		checked2 = IsDlgButtonChecked(hWnd, IDJPEG);
		checked3 = IsDlgButtonChecked(hWnd, IDJPEG2000);
		checked4 = IsDlgButtonChecked(hWnd, IDPNG);
		checked5 = IsDlgButtonChecked(hWnd, IDPBM);
		checked6 = IsDlgButtonChecked(hWnd, IDSR);
		checked7 = IsDlgButtonChecked(hWnd, IDTIFF);

		// Wybór konwersji BMP
		if (checked1 == BST_CHECKED && checked2 == BST_UNCHECKED && checked3 == BST_UNCHECKED && checked4 == BST_UNCHECKED && checked5 == BST_UNCHECKED && checked6 == BST_UNCHECKED && checked7 == BST_UNCHECKED && WczytanoPlik) {
			// Ustawienie przycisków odpowiednich dla wybranego formatu
			EnableWindow(LoadFile, FALSE);
			EnableWindow(ComboRead, FALSE);
			EnableWindow(JPEG, FALSE);
			EnableWindow(JPEG2000, FALSE);
			EnableWindow(TIFFI, FALSE);
			EnableWindow(JPEG_QUALITY, FALSE);
			EnableWindow(JPEG2000_QUALITY, FALSE);
			EnableWindow(PNG, FALSE);
			EnableWindow(PNG_QUALITY, FALSE);
			EnableWindow(ComboPNG, FALSE);

			EnableWindow(BMP, TRUE);
			EnableWindow(NazwaPliku, TRUE);
			EnableWindow(Konwertuj, TRUE);
			EnableWindow(Help, TRUE);
			EnableWindow(CloseProgram, TRUE);
			EnableWindow(NazwaPlikuL, TRUE);
			EnableWindow(CzasL, TRUE);

			if ((HWND)lParam == Konwertuj)
			{
				clock_t start = clock();

				if (mat.size == NULL)
				{
					MessageBox(NULL, "Nie wczytano pliku, nie mo¿na przeprowadziæ konwersji.", "B³¹d", MB_OK);
					break;
				}

				Mat image_bmp;
				mat.convertTo(image_bmp, CV_8UC3);

				dlugosc = GetWindowTextLength(NazwaPliku);
				Bufor = (LPSTR)GlobalAlloc(GPTR, dlugosc + 1);
				GetWindowText(NazwaPliku, Bufor, dlugosc + 1);
				NazwaKoncowa = Bufor;
				if (NazwaKoncowa.size() <= 0)
				{
					MessageBox(NULL, "Nie podano nazwy pliku!", "B³¹d", MB_OK);
					break;
				}
				GlobalFree(Bufor);

				imwrite(NazwaKoncowa + ".bmp", image_bmp);

				clock_t end = clock();
				float seconds = (float)(end - start) / CLOCKS_PER_SEC;
				ostringstream ss;
				ss << seconds;
				string s(ss.str());
				SetWindowTextA(Czas, s.c_str());
				SetWindowText(NazwaPliku, "");
				EnableWindow(BMP, FALSE);
				SendMessage(BMP, BM_SETCHECK, BST_UNCHECKED, 0);
				EnableWindow(NazwaPliku, FALSE);
				image_bmp = imread(NazwaKoncowa + ".bmp", tryb_odczytu);
				namedWindow("Wynik konwersji", WINDOW_NORMAL);
				imshow("Wynik konwersji", image_bmp);
				resizeWindow("Wynik konwersji", 1024, 880);
				EnableWindow(Konwertuj, FALSE);
				MessageBox(NULL, "Wybierz nowy plik lub zakoñcz program.", "Zakoñczono konwersjê", MB_OK);
				ofn.lpstrFile = NULL;
				WczytanoPlik = FALSE;
				UkonczonoKonwersje = TRUE;
				mat.release();
				image_bmp.release();
				EnableWindow(LoadFile, TRUE);
				EnableWindow(ComboRead, TRUE);
			}
		}

		// Wybór konwersji JPEG
		if (checked1 == BST_UNCHECKED && checked2 == BST_CHECKED && checked3 == BST_UNCHECKED && checked4 == BST_UNCHECKED && checked5 == BST_UNCHECKED && checked6 == BST_UNCHECKED && checked7 == BST_UNCHECKED && WczytanoPlik) {
			// Ustawienie przycisków odpowiednich dla wybranego formatu
			EnableWindow(LoadFile, FALSE);
			EnableWindow(ComboRead, FALSE);
			EnableWindow(BMP, FALSE);
			EnableWindow(JPEG2000, FALSE);
			EnableWindow(TIFFI, FALSE);
			EnableWindow(JPEG2000_QUALITY, FALSE);
			EnableWindow(PNG, FALSE);
			EnableWindow(PNG_QUALITY, FALSE);
			EnableWindow(ComboPNG, FALSE);

			EnableWindow(JPEG, TRUE);
			EnableWindow(NazwaPliku, TRUE);
			EnableWindow(Konwertuj, TRUE);
			EnableWindow(JPEG_QUALITY, TRUE);
			EnableWindow(Help, TRUE);
			EnableWindow(CloseProgram, TRUE);
			EnableWindow(NazwaPlikuL, TRUE);
			EnableWindow(CzasL, TRUE);

			if ((HWND)lParam == Konwertuj)
			{
				clock_t start = clock();

				if (mat.size == NULL)
				{
					MessageBox(NULL, "Nie wczytano pliku, nie mo¿na przeprowadziæ konwersji.", "B³¹d", MB_OK);
					break;
				}

				Mat image_jpg;
				mat.convertTo(image_jpg, CV_8UC4);

				dlugosc = GetWindowTextLength(JPEG_QUALITY);
				Bufor = (LPSTR)GlobalAlloc(GPTR, dlugosc + 1);
				GetWindowText(JPEG_QUALITY, Bufor, dlugosc + 1);

				bool isNumberNotGood = false;
				for (int i = 0; i < strlen(Bufor); i++)
				{
					if (isdigit(Bufor[i]) == false)
						isNumberNotGood = true;
				}
				if (isNumberNotGood)
				{
					MessageBox(NULL, "Wyst¹pi³ b³¹d podano niew³aœciwy typ danych, wpisz liczbê ca³kowit¹", "B³¹d", MB_OK);
					SetWindowText(JPEG_QUALITY, "");
					SetWindowText(NazwaPliku, "");
					break;
				}

				JPEG_Q = _ttoi(Bufor);
				GlobalFree(Bufor);

				if (checked2 == BST_CHECKED && JPEG_Q < 0 || JPEG_Q > 100)
				{
					MessageBox(NULL, "Podaj liczbê ca³kowit¹ z zakresu od 0 do 100", "B³¹d", MB_OK);
					SetWindowText(JPEG_QUALITY, "");
					SetWindowText(NazwaPliku, "");
					break;
				}

				vector<int> compression_params;
				compression_params.push_back(IMWRITE_JPEG_QUALITY);
				compression_params.push_back(JPEG_Q);

				dlugosc = GetWindowTextLength(NazwaPliku);
				Bufor = (LPSTR)GlobalAlloc(GPTR, dlugosc + 1);
				GetWindowText(NazwaPliku, Bufor, dlugosc + 1);
				NazwaKoncowa = Bufor;
				if (NazwaKoncowa.size() <= 0)
				{
					MessageBox(NULL, "Nie podano nazwy pliku!", "B³¹d", MB_OK);
					break;
				}
				GlobalFree(Bufor);

				imwrite(NazwaKoncowa + ".jpeg", image_jpg, compression_params);
				clock_t end = clock();
				float seconds = (float)(end - start) / CLOCKS_PER_SEC;
				ostringstream ss;
				ss << seconds;
				string s(ss.str());
				SetWindowTextA(Czas, s.c_str());
				SetWindowText(JPEG_QUALITY, "");
				SetWindowText(NazwaPliku, "");
				EnableWindow(JPEG, FALSE);
				EnableWindow(JPEG_QUALITY, FALSE);
				SendMessage(JPEG, BM_SETCHECK, BST_UNCHECKED, 0);
				EnableWindow(NazwaPliku, FALSE);
				EnableWindow(Konwertuj, FALSE);
				image_jpg = imread(NazwaKoncowa + ".jpeg", tryb_odczytu);
				namedWindow("Wynik konwersji", WINDOW_NORMAL);
				imshow("Wynik konwersji", image_jpg);
				resizeWindow("Wynik konwersji", 1024, 880);
				MessageBox(NULL, "Wybierz nowy plik lub zakoñcz program.", "Zakoñczono konwersjê", MB_OK);
				ofn.lpstrFile = NULL;
				WczytanoPlik = FALSE;
				UkonczonoKonwersje = TRUE;
				compression_params.clear();
				mat.release();
				image_jpg.release();
				EnableWindow(LoadFile, TRUE);
				EnableWindow(ComboRead, TRUE);
			}
		}

		// Wybór konwersji JPEG2000
		if (checked1 == BST_UNCHECKED && checked2 == BST_UNCHECKED && checked3 == BST_CHECKED && checked4 == BST_UNCHECKED && checked5 == BST_UNCHECKED && checked6 == BST_UNCHECKED && checked7 == BST_UNCHECKED && WczytanoPlik) {
			// Ustawienie przycisków odpowiednich dla wybranego formatu
			EnableWindow(LoadFile, FALSE);
			EnableWindow(ComboRead, FALSE);
			EnableWindow(BMP, FALSE);
			EnableWindow(JPEG, FALSE);
			EnableWindow(TIFFI, FALSE);
			EnableWindow(JPEG_QUALITY, FALSE);
			EnableWindow(PNG, FALSE);
			EnableWindow(PNG_QUALITY, FALSE);
			EnableWindow(ComboPNG, FALSE);

			EnableWindow(JPEG2000, TRUE);
			EnableWindow(NazwaPliku, TRUE);
			EnableWindow(Konwertuj, TRUE);
			EnableWindow(JPEG2000_QUALITY, TRUE);
			EnableWindow(Help, TRUE);
			EnableWindow(CloseProgram, TRUE);
			EnableWindow(NazwaPlikuL, TRUE);
			EnableWindow(CzasL, TRUE);

			if ((HWND)lParam == Konwertuj)
			{
				clock_t start = clock();

				if (mat.size == NULL)
				{
					MessageBox(NULL, "Nie wczytano pliku, nie mo¿na przeprowadziæ konwersji.", "B³¹d", MB_OK);
					break;
				}

				Mat image_jpg2;
				mat.convertTo(image_jpg2, CV_8UC4);

				dlugosc = GetWindowTextLength(JPEG2000_QUALITY);
				Bufor = (LPSTR)GlobalAlloc(GPTR, dlugosc + 1);
				GetWindowText(JPEG2000_QUALITY, Bufor, dlugosc + 1);

				bool isNumberNotGood = false;
				for (int i = 0; i < strlen(Bufor); i++)
				{
					if (isdigit(Bufor[i]) == false)
						isNumberNotGood = true;
				}
				if (isNumberNotGood)
				{
					MessageBox(NULL, "Wyst¹pi³ b³¹d podano niew³aœciwy typ danych, wpisz liczbê ca³kowit¹", "B³¹d", MB_OK);
					SetWindowText(JPEG2000_QUALITY, "");
					SetWindowText(NazwaPliku, "");
					break;
				}

				JPEG2000_Q = _ttoi(Bufor);
				GlobalFree(Bufor);

				if (checked2 == BST_CHECKED && JPEG2000_Q < 0 || JPEG2000_Q > 1000)
				{
					MessageBox(NULL, "Podaj liczbê ca³kowit¹ z zakresu od 0 do 1000", "B³¹d", MB_OK);
					SetWindowText(JPEG2000_QUALITY, "");
					SetWindowText(NazwaPliku, "");
					break;
				}

				vector<int> compression_params;
				compression_params.push_back(IMWRITE_JPEG2000_COMPRESSION_X1000);
				compression_params.push_back(JPEG2000_Q);

				dlugosc = GetWindowTextLength(NazwaPliku);
				Bufor = (LPSTR)GlobalAlloc(GPTR, dlugosc + 1);
				GetWindowText(NazwaPliku, Bufor, dlugosc + 1);
				NazwaKoncowa = Bufor;
				if (NazwaKoncowa.size() <= 0)
				{
					MessageBox(NULL, "Nie podano nazwy pliku!", "B³¹d", MB_OK);
					break;
				}
				GlobalFree(Bufor);

				imwrite(NazwaKoncowa + ".jp2", image_jpg2, compression_params);
				clock_t end = clock();
				float seconds = (float)(end - start) / CLOCKS_PER_SEC;
				ostringstream ss;
				ss << seconds;
				string s(ss.str());
				SetWindowTextA(Czas, s.c_str());
				SetWindowText(JPEG2000_QUALITY, "");
				SetWindowText(NazwaPliku, "");
				EnableWindow(JPEG2000, FALSE);
				EnableWindow(JPEG2000_QUALITY, FALSE);
				SendMessage(JPEG2000, BM_SETCHECK, BST_UNCHECKED, 0);
				EnableWindow(NazwaPliku, FALSE);
				EnableWindow(Konwertuj, FALSE);
				image_jpg2 = imread(NazwaKoncowa + ".jp2", tryb_odczytu);
				namedWindow("Wynik konwersji", WINDOW_NORMAL);
				imshow("Wynik konwersji", image_jpg2);
				resizeWindow("Wynik konwersji", 1024, 880);
				MessageBox(NULL, "Wybierz nowy plik lub zakoñcz program.", "Zakoñczono konwersjê", MB_OK);
				ofn.lpstrFile = NULL;
				WczytanoPlik = FALSE;
				UkonczonoKonwersje = TRUE;
				compression_params.clear();
				mat.release();
				image_jpg2.release();
				EnableWindow(LoadFile, TRUE);
				EnableWindow(ComboRead, TRUE);
			}
		}

		// Wybór konwersji PNG
		if (checked1 == BST_UNCHECKED && checked2 == BST_UNCHECKED && checked3 == BST_UNCHECKED && checked4 == BST_CHECKED && checked5 == BST_UNCHECKED && checked6 == BST_UNCHECKED && checked7 == BST_UNCHECKED && WczytanoPlik) {
			// Ustawienie przycisków odpowiednich dla wybranego formatu
			EnableWindow(LoadFile, FALSE);
			EnableWindow(ComboRead, FALSE);
			EnableWindow(BMP, FALSE);
			EnableWindow(JPEG, FALSE);
			EnableWindow(JPEG2000, FALSE);
			EnableWindow(TIFFI, FALSE);
			EnableWindow(JPEG_QUALITY, FALSE);
			EnableWindow(JPEG2000_QUALITY, FALSE);

			EnableWindow(PNG, TRUE);
			EnableWindow(NazwaPliku, TRUE);
			EnableWindow(Konwertuj, TRUE);
			EnableWindow(PNG_QUALITY, TRUE);
			EnableWindow(ComboPNG, TRUE);
			EnableWindow(Help, TRUE);
			EnableWindow(CloseProgram, TRUE);
			EnableWindow(NazwaPlikuL, TRUE);
			EnableWindow(CzasL, TRUE);

			if ((HWND)lParam == Konwertuj)
			{
				clock_t start = clock();

				if (mat.size == NULL)
				{
					MessageBox(NULL, "Nie wczytano pliku, nie mo¿na przeprowadziæ konwersji.", "B³¹d", MB_OK);
					break;
				}

				Mat image_png;
				mat.convertTo(image_png, CV_8UC4);

				dlugosc = GetWindowTextLength(PNG_QUALITY);
				Bufor = (LPSTR)GlobalAlloc(GPTR, dlugosc + 1);
				GetWindowText(PNG_QUALITY, Bufor, dlugosc + 1);

				bool isNumberNotGood = false;
				for (int i = 0; i < strlen(Bufor); i++)
				{
					if (isdigit(Bufor[i]) == false)
						isNumberNotGood = true;
				}
				if (isNumberNotGood)
				{
					MessageBox(NULL, "Wyst¹pi³ b³¹d podano niew³aœciwy typ danych, wpisz liczbê ca³kowit¹", "B³¹d", MB_OK);
					SetWindowText(PNG_QUALITY, "");
					SetWindowText(NazwaPliku, "");
					break;
				}

				PNG_Q = _ttoi(Bufor);
				GlobalFree(Bufor);

				if (checked4 == BST_CHECKED && PNG_Q < 0 || PNG_Q > 9)
				{
					MessageBox(NULL, "Podaj liczbê ca³kowit¹ z zakresu od 0 do 9", "B³¹d", MB_OK);
					SetWindowText(PNG_QUALITY, "");
					SetWindowText(NazwaPliku, "");
					break;
				}

				vector<int> compression_params;
				compression_params.push_back(IMWRITE_PNG_COMPRESSION);
				compression_params.push_back(PNG_Q);
				compression_params.push_back(IMWRITE_PNG_STRATEGY);
				if (WczytanoPlik)
				{
					char msg[1000];
					HWND hGetCombo = GetDlgItem(hWnd, ID_COMBOPNG);
					int nIndex = ::SendMessage(hGetCombo, CB_GETCURSEL, 0, 0);
					SendMessage(hGetCombo, CB_GETLBTEXT, (WPARAM)nIndex, (LPARAM)msg);

					if (strcmp(msg, "DEFAULT") == 0) compression_params.push_back(IMWRITE_PNG_STRATEGY_DEFAULT);
					if (strcmp(msg, "FILTERED)") == 0) compression_params.push_back(IMWRITE_PNG_STRATEGY_FILTERED);
					if (strcmp(msg, "HUFFMAN") == 0) compression_params.push_back(IMWRITE_PNG_STRATEGY_HUFFMAN_ONLY);
					if (strcmp(msg, "RLE") == 0) compression_params.push_back(IMWRITE_PNG_STRATEGY_RLE);
					if (strcmp(msg, "FIXED") == 0) compression_params.push_back(IMWRITE_PNG_STRATEGY_FIXED);
				}

				dlugosc = GetWindowTextLength(NazwaPliku);
				Bufor = (LPSTR)GlobalAlloc(GPTR, dlugosc + 1);
				GetWindowText(NazwaPliku, Bufor, dlugosc + 1);
				NazwaKoncowa = Bufor;
				if (NazwaKoncowa.size() <= 0)
				{
					MessageBox(NULL, "Nie podano nazwy pliku!", "B³¹d", MB_OK);
					break;
				}
				GlobalFree(Bufor);

				imwrite(NazwaKoncowa + ".png", image_png, compression_params);
				clock_t end = clock();
				float seconds = (float)(end - start) / CLOCKS_PER_SEC;
				ostringstream ss;
				ss << seconds;
				string s(ss.str());
				SetWindowTextA(Czas, s.c_str());
				SetWindowText(PNG_QUALITY, "");
				SetWindowText(NazwaPliku, "");
				EnableWindow(PNG, FALSE);
				EnableWindow(PNG_QUALITY, FALSE);
				SendMessage(PNG, BM_SETCHECK, BST_UNCHECKED, 0);
				EnableWindow(NazwaPliku, FALSE);
				EnableWindow(ComboPNG, FALSE);
				EnableWindow(Konwertuj, FALSE);
				image_png = imread(NazwaKoncowa + ".png", tryb_odczytu);
				namedWindow("Wynik konwersji", WINDOW_NORMAL);
				imshow("Wynik konwersji", image_png);
				resizeWindow("Wynik konwersji", 1024, 880);
				MessageBox(NULL, "Wybierz nowy plik lub zakoñcz program.", "Zakoñczono konwersjê", MB_OK);
				ofn.lpstrFile = NULL;
				WczytanoPlik = FALSE;
				UkonczonoKonwersje = TRUE;
				compression_params.clear();
				mat.release();
				image_png.release();
				EnableWindow(LoadFile, TRUE);
				EnableWindow(ComboRead, TRUE);
			}
		}

		// Wybór konwersji TIFF
		if (checked1 == BST_UNCHECKED && checked2 == BST_UNCHECKED && checked3 == BST_UNCHECKED && checked4 == BST_UNCHECKED && checked5 == BST_UNCHECKED && checked6 == BST_UNCHECKED && checked7 == BST_CHECKED && WczytanoPlik) {
			// Ustawienie przycisków odpowiednich dla wybranego formatu
			EnableWindow(LoadFile, FALSE);
			EnableWindow(ComboRead, FALSE);
			EnableWindow(BMP, FALSE);
			EnableWindow(JPEG, FALSE);
			EnableWindow(JPEG2000, FALSE);
			EnableWindow(JPEG_QUALITY, FALSE);
			EnableWindow(JPEG2000_QUALITY, FALSE);
			EnableWindow(PNG, FALSE);
			EnableWindow(PNG_QUALITY, FALSE);
			EnableWindow(ComboPNG, FALSE);

			EnableWindow(TIFFI, TRUE);
			EnableWindow(NazwaPliku, TRUE);
			EnableWindow(Konwertuj, TRUE);
			EnableWindow(Help, TRUE);
			EnableWindow(CloseProgram, TRUE);
			EnableWindow(NazwaPlikuL, TRUE);
			EnableWindow(CzasL, TRUE);

			if ((HWND)lParam == Konwertuj)
			{
				clock_t start = clock();

				if (mat.size == NULL)
				{
					MessageBox(NULL, "Nie wczytano pliku, nie mo¿na przeprowadziæ konwersji.", "B³¹d", MB_OK);
					break;
				}

				Mat image_tiff;
				mat.convertTo(image_tiff, CV_8UC4);

				dlugosc = GetWindowTextLength(NazwaPliku);
				Bufor = (LPSTR)GlobalAlloc(GPTR, dlugosc + 1);
				GetWindowText(NazwaPliku, Bufor, dlugosc + 1);
				NazwaKoncowa = Bufor;
				if (NazwaKoncowa.size() <= 0)
				{
					MessageBox(NULL, "Nie podano nazwy pliku!", "B³¹d", MB_OK);
					break;
				}
				GlobalFree(Bufor);

				imwrite(NazwaKoncowa + ".tiff", image_tiff);
				clock_t end = clock();
				float seconds = (float)(end - start) / CLOCKS_PER_SEC;
				ostringstream ss;
				ss << seconds;
				string s(ss.str());
				SetWindowTextA(Czas, s.c_str());
				SetWindowText(NazwaPliku, "");
				EnableWindow(TIFFI, FALSE);
				SendMessage(TIFFI, BM_SETCHECK, BST_UNCHECKED, 0);
				EnableWindow(NazwaPliku, FALSE);
				EnableWindow(Konwertuj, FALSE);
				image_tiff = imread(NazwaKoncowa + ".tiff", tryb_odczytu);
				namedWindow("Wynik konwersji", WINDOW_NORMAL);
				imshow("Wynik konwersji", image_tiff);
				resizeWindow("Wynik konwersji", 1024, 880);
				MessageBox(NULL, "Wybierz nowy plik lub zakoñcz program.", "Zakoñczono konwersjê", MB_OK);
				ofn.lpstrFile = NULL;
				WczytanoPlik = FALSE;
				UkonczonoKonwersje = TRUE;
				mat.release();
				image_tiff.release();
				EnableWindow(LoadFile, TRUE);
				EnableWindow(ComboRead, TRUE);
			}
		}

		// Resetowanie pól wyboru
		if (checked1 == BST_UNCHECKED && checked2 == BST_UNCHECKED && checked3 == BST_UNCHECKED && checked4 == BST_UNCHECKED && checked5 == BST_UNCHECKED && checked6 == BST_UNCHECKED && checked7 == BST_UNCHECKED && WczytanoPlik) {
			for (int i = 0; i < przyciski.size(); i++)
			{
				EnableWindow(przyciski[i], TRUE);
			}
			EnableWindow(Konwertuj, FALSE);
			EnableWindow(ComboPNG, FALSE);
			for (int i = 13; i < przyciski.size(); i++)
			{
				EnableWindow(przyciski[i], FALSE);
			}
			EnableWindow(ComboRead, TRUE);
			EnableWindow(LoadFile, TRUE);
		}
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;
	default:
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	break;
	}
	return 0;
}
