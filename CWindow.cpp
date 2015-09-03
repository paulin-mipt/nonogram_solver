#include "CWindow.h"
#include <string>

static TCHAR szWindowClass[] = _T( "CWindow" );

HWND CWindow::GetHandle() {
    return handle;
}

HWND CWindow::GetHandleDialog() {
    return handleDlg;
}

bool CWindow::Create() {
    handle = ::CreateWindow( szWindowClass, L"Японские кроссворды", WS_OVERLAPPEDWINDOW  | WS_CLIPCHILDREN | WS_EX_LAYERED,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 300, NULL, NULL, ::GetModuleHandle( 0 ), this );
	::UpdateWindow( handle );
    return ( handle != 0 );
}

void CWindow::initialize() {
    numbers = std::vector< std::vector<int> >(sizeY + blocksNum);
	edits = std::vector< std::vector<HWND> >(sizeY + blocksNum);
    for( int i = 0; i < (sizeY + blocksNum); i++ ) {
        numbers[i].resize(sizeX + blocksNum);
		edits[i].resize(sizeX + blocksNum);
    }
    for( int i = 0; i < (sizeY + blocksNum); i++ ){
        for( int j = 0; j < (sizeX + blocksNum); j++ ) {
            numbers[i][j] = 0;
        }
    }

	RECT rect;
	::GetClientRect( handle, &rect ); 
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

    qWidth = ( width - 5 ) / ( sizeX + blocksNum ) + 1;
    qHeight = ( height - 5 ) / ( sizeY + blocksNum ) + 1;

	for( int i = 0; i < sizeY + blocksNum; i++ ) {
        for( int j = 0; j < sizeX + blocksNum; j++ ) {
            RECT rect;
            rect.left = j * qWidth;
            rect.top = i * qHeight;
            rect.right = (j + 1) * qWidth;
            rect.bottom = (i + 1) * qHeight;
            if( (i < blocksNum) ^ (j < blocksNum) ) {
				HFONT font = CreateFont(qWidth / (1.1) , 0, 0, 0, 0, 0u, 0U, 0U, ANSI_CHARSET, 0U, 0U, 0U, 0U, TEXT("Arial"));
				edits[i][j] = CreateWindowEx(NULL, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_GROUP | WS_BORDER | ES_CENTER | ES_NUMBER, 
        rect.left, rect.top, qWidth - 0.1, qHeight - 0.1, handle, NULL, GetModuleHandle(NULL), NULL);
				if (i != blocksNum - 1 && j != blocksNum - 1) {
					Edit_Enable(edits[i][j], false);
				}
				iEdit[edits[i][j]] = i;
				jEdit[edits[i][j]] = j;
				SendMessage(edits[i][j], WM_SETFONT, (WPARAM) font, MAKELPARAM(TRUE, 0));
			}          
        }
    }
}

CWindow::CWindow() {
	whiteBrush = ::CreateSolidBrush( RGB( 0xFF, 0xFF, 0xFF) );
	greyBrush = ::CreateSolidBrush( RGB( 0xD0, 0xD0, 0xD0) );
	blackBrush = ::CreateSolidBrush( RGB( 0x0, 0x0, 0x0) );
    sizeX = 8;
	sizeY = 6;
	blocksNum = 3;
    startNewGame();
}
CWindow::~CWindow() {}

bool CWindow::RegisterClass() {
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = 0;
    wcex.lpfnWndProc = windowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    HINSTANCE hInstance = ::GetModuleHandle( 0 );
    wcex.hInstance = hInstance;
    wcex.hIcon = reinterpret_cast<HICON>( ::LoadImage( hInstance, MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 32, 32, 0 ) );
    wcex.hCursor = ::LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = reinterpret_cast<HBRUSH>( COLOR_WINDOW + 1);
    wcex.lpszMenuName = reinterpret_cast<LPCWSTR>( MAKEINTRESOURCE( IDR_MENU1 ) );
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = reinterpret_cast<HICON>( ::LoadImage( hInstance, MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 16, 16, 0 ) );;

    return ::RegisterClassEx( &wcex );
}

void CWindow::Show( int cmdShow ) {
    ShowWindow( handle, cmdShow );
}

void CWindow::OnDestroy() {
    ::PostQuitMessage( 0 );
}

void CWindow::OnSize( LPARAM lParam ) {
    RECT rect;
    RECT windowRect;
    ::GetClientRect( handle, &rect );
    ::GetWindowRect( handle, &windowRect );
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;
    int diff = (windowHeight - height);
    ::SetWindowPos( handle, NULL, windowRect.left, windowRect.top, windowWidth, diff + windowWidth * (1.0 * sizeY + blocksNum) / (sizeX + blocksNum), 0 );
    ::GetClientRect( handle, &rect );
    ::InvalidateRect( handle, &rect, TRUE );
}

void CWindow::OnPaint() {
	PAINTSTRUCT ps;
	HDC hdc = ::BeginPaint( handle, &ps );

	RECT rect;
	::GetClientRect( handle, &rect );

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	HDC backbuffDC = ::CreateCompatibleDC( hdc );
	HBITMAP backbuffer = ::CreateCompatibleBitmap( hdc, width, height );
	::SelectObject( backbuffDC, backbuffer );

    COLORREF back = RGB( 0, 0, 0 );
	HBRUSH backgroundHBrush = ::CreateSolidBrush( back );
	::FillRect( backbuffDC, &rect, backgroundHBrush );

    HPEN hpen = ::CreatePen(PS_SOLID, 0.1, back); 
    HGDIOBJ hpenOld = SelectObject( backbuffDC, hpen ); 

	::SelectObject( backbuffDC, whiteBrush );
	::SetPolyFillMode( backbuffDC, WINDING );
    qWidth = ( width - 5 ) / ( sizeX + blocksNum ) + 1;
    qHeight = ( height - 5 ) / ( sizeY + blocksNum ) + 1;

    ::SetBkMode( backbuffDC, TRANSPARENT );
    ::SetTextColor( backbuffDC, RGB( 255, 255, 255 ) );

    for( int i = 0; i < sizeY + blocksNum; i++ ) {
        for( int j = 0; j < sizeX + blocksNum; j++ ) {
			RECT rect;
			rect.left = j * qWidth;
			rect.top = i * qHeight;
			rect.right = (j + 1) * qWidth;
			rect.bottom = (i + 1) * qHeight;
            if( i >= blocksNum && j >= blocksNum ) {
				switch (numbers[i][j]) {
				case 0:
					::SelectObject( backbuffDC, greyBrush );
					::Rectangle( backbuffDC, rect.left, rect.top, rect.right, rect.bottom );
					break;
				case 1:
					::SelectObject( backbuffDC, blackBrush );
					::Rectangle( backbuffDC, rect.left, rect.top, rect.right, rect.bottom );
					break;
				case 2:
					::SelectObject( backbuffDC, whiteBrush );
					::Rectangle( backbuffDC, rect.left, rect.top, rect.right, rect.bottom );
				}
			} else if( (i < blocksNum) ^ (j < blocksNum) ) {
				HFONT font = CreateFont(qWidth / (1.1) , 0, 0, 0, 0, 0u, 0U, 0U, ANSI_CHARSET, 0U, 0U, 0U, 0U, TEXT("Arial"));
				::MoveWindow(edits[i][j], rect.left, rect.top, qWidth, qHeight, TRUE);
				SendMessage(edits[i][j], WM_SETFONT, (WPARAM) font, MAKELPARAM(TRUE, 0));
			}
			else {
				::SelectObject( backbuffDC, blackBrush );
				::Rectangle( backbuffDC, rect.left, rect.top, rect.right, rect.bottom );
			}
        }
    }
	::BitBlt( hdc, 0, 0, width, height, backbuffDC, 0, 0, SRCCOPY );

    ::SelectObject( backbuffDC, hpenOld );
    ::DeleteObject( hpen );

	::DeleteObject( backbuffer );
	::DeleteDC( backbuffDC );

	::EndPaint( handle, &ps ); 
	::DeleteObject( backgroundHBrush );
}

void CWindow::startNewGame() {
	initialize();
	OnSize(NULL);
    RECT rect;
    ::GetClientRect( handle, &rect );
    ::InvalidateRect( handle, &rect, TRUE );
}

void CWindow::loadFile() {
    std::ifstream fin;
    fin.open("game.save");
    if( fin.is_open() ) {
        fin >> sizeX >> sizeY >> blocksNum;
        numbers.resize(sizeY + blocksNum);
        for( int i = 0; i < sizeY + blocksNum; i++ ) {
            numbers[i].resize(sizeX + blocksNum);
			for( int j = 0; j < (sizeX + blocksNum); j++ ) {
                fin >> numbers[i][j];
            }
        }
       
        loadedFromFile = true;
        RECT rect;
        ::GetClientRect( handle, &rect );
        ::InvalidateRect( handle, &rect, TRUE );
    }
    fin.close();
}

void CWindow::SaveFile() {
    std::ofstream fout;
    fout.open("game.save");
    fout << sizeX << " " << sizeY << " " << blocksNum << std::endl;
    for( int i = 0; i < (sizeY + blocksNum); i++ ) {
        for( int j = 0; j < (sizeX + blocksNum); j++ ) {
            fout << numbers[i][j] << " ";
        }
        fout << std::endl;
    }
    fout.close();
}

void CWindow::OnClose() {
    switch( MessageBox( handle, L"Вы уверены?", L"Выйти из игры", MB_YESNO | MB_ICONQUESTION ) ) {
    case IDNO:
        return;   
    case IDYES:
		switch( MessageBox( handle, L"И это после всего, что между нами было?!", L"Выйти из игры", MB_YESNO | MB_ICONWARNING ) ) {
		case IDNO:
			return;   
		case IDYES:
			SaveFile();
			::PostQuitMessage( 0 );
			break;
		}
    }
}

void CWindow::OnCommand( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) {
    if (HIWORD(wParam) == EN_KILLFOCUS) {
		char edit1[100]; 
		HWND hEdit = (HWND)lParam;
		int i = iEdit[hEdit], j = jEdit[hEdit];
		GetWindowText(hEdit, reinterpret_cast<LPWSTR>(edit1), sizeof(edit1)/sizeof(edit1[0]));
		int result = atoi(edit1);
		numbers[i][j] = result;
		if (result == 0) {
			SetWindowText(hEdit, L"");
			if (i < blocksNum) {
				for (int k = i - 1; k >= 0; k--) {
					numbers[k][j] = 0;
					SetWindowText(edits[k][j], L"");
					Edit_Enable(edits[k][j], false);
				}
			} else {
				for (int k = j - 1; k >= 0; k--) {
					numbers[i][k] = 0;
					SetWindowText(edits[i][k], L"");
					Edit_Enable(edits[i][k], false);
				}
			}
		}
		else {
			if (i < blocksNum) {
				if (i > 0) {
					Edit_Enable(edits[i - 1][j], true);
				}
			} else {
				if (j > 0) {
					Edit_Enable(edits[i][j - 1], true);
				}
			}
		}
		return;
	} else if( HIWORD( wParam ) == 1 ) {
        switch( LOWORD( wParam ) ) {
        case ID_ACC1:
            OnClose();
            break;
        default:
            return;
        }
    } else if( HIWORD( wParam ) == 0 ) {
        switch( LOWORD( wParam ) ) {   
        case ID_EXIT:
            OnClose();
            break;
        case ID_NEWGAME:
            ::CreateDialog( GetModuleHandle( 0 ), MAKEINTRESOURCE( IDD_DIALOG1 ), handle, DialogProc );            
            break;
        case ID_SAVE:
            SaveFile();
            break;
        case ID_LOAD:
            loadFile();
            break;
		case ID_CLEAR:
			for (int i = 0; i < sizeY; i++) {
				for (int j = 0; j < sizeX; j++) {
					numbers[i + blocksNum][j + blocksNum] = 0;
				}
			}
			RECT rect;
			::GetClientRect( handle, &rect );
			::InvalidateRect( handle, &rect, TRUE );
			break;
		case ID_CHEAT:
			if (!solve()) {
				::MessageBox( handle, L"Oops! Так это точно не решается. Попробуйте очистить игровое поле и считерить ещё раз", L"Не вышло...", MB_OK | MB_ICONERROR );
			}
			break;
        default: 
            break;  
        }
	}
}

bool CWindow::solve() {
	JapanSolver solver(numbers, blocksNum);
	bool result = solver.solve(&numbers);
	if (result) {
		RECT rect;
		::GetClientRect( handle, &rect );
		::InvalidateRect( handle, &rect, TRUE );
	}
	return result;
}

INT_PTR __stdcall CWindow::DialogProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam ) { 
    CWindow* that = reinterpret_cast<CWindow*>( ::GetWindowLong( ::GetParent( hwndDlg ), GWL_USERDATA ) );

    switch( uMsg ) {
    case WM_INITDIALOG:
        that->handleDlg = hwndDlg;
        return TRUE;
    case WM_COMMAND:
        switch( LOWORD( wParam ) ) {
        case IDOK:
			int _sizeX, _sizeY, _blocksNum;
			char bufferX[10], bufferY[10], bufferBlock[10];
            GetWindowText( ::GetDlgItem( hwndDlg, IDC_EDIT_HEIGHT ), reinterpret_cast<LPWSTR>( bufferX ), sizeof( bufferX ) );
            _sizeX = atoi( bufferX );
            GetWindowText( ::GetDlgItem( hwndDlg, IDC_EDIT_WIDTH ), reinterpret_cast<LPWSTR>( bufferY ), sizeof( bufferY ) );
            _sizeY = atoi( bufferY );
            GetWindowText( ::GetDlgItem( hwndDlg, IDC_EDIT_BLOCKS ), reinterpret_cast<LPWSTR>( bufferBlock ), sizeof( bufferBlock ) );
            _blocksNum = atoi( bufferBlock );
            switch( ::MessageBox( that->handle, L"Начать новую игру?", L"Новая игра", MB_YESNO | MB_ICONWARNING ) ) {
            case IDYES:
		        that->sizeX = _sizeX;
				that->sizeY = _sizeY;
				that->blocksNum = _blocksNum;
                that->startNewGame();
				break;
			case IDNO:
                break;
            }
			::DestroyWindow( hwndDlg );
            return TRUE;
        case IDCANCEL:
            ::DestroyWindow( hwndDlg );
            return TRUE;
        }
        break;
    default:
        return FALSE;
    }

    return TRUE;
}

void CWindow::checkWin() {
    bool won = true;
    /*for( int i = 0; i < (sizeY + blocksNum); i++ ) {
        for( int j = 0; j < (sizeX + blocksNum); j++ ) {
            if( numbers[i][j] != i*(sizeX + blocksNum) + j + 1) {
                won = false;
                break;
            }
        }
    }*/
    if( won ) {
        RECT rect;
        ::GetClientRect( handle, &rect );
        ::InvalidateRect( handle, &rect, TRUE );
        switch( ::MessageBox( handle, L"Вы выиграли!\r\nХотите еще?", L"Поздравляем!", MB_YESNO | MB_ICONINFORMATION) ) {
        case IDYES:
            startNewGame();
            break;
        case IDNO:
            return;
        }
    }
}

LRESULT CWindow::OnCtlColorEdit(HDC dc, HWND hEdit, CWindow *win) {
	/*int i = iEdit[hEdit];
	int j = jEdit[hEdit];
	int cnt = 0, N = numbers[i][j];
	if (!N) N = 1;
	while (N) {
		N /= 10;
		cnt++;
	}
	RECT rect = rects[i][j];
	HFONT font = CreateFont((rect.top - rect.bottom) / (2 * cnt), 0, 0, 0, 0, 0u, 0U, 0U, ANSI_CHARSET, 0U, 0U, 0U, 0U, TEXT("Arial"));
	
	SendMessage(edits[i][j], WM_SETFONT, (WPARAM)font, MAKELPARAM(TRUE, 0));
	*/
	return (LRESULT)CreateSolidBrush(RGB(138, 255, 92));
}

void CWindow::OnClick( LPARAM lParam ) {
    RECT rect;
	::GetClientRect( handle, &rect ); 

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

    int xPos = GET_X_LPARAM(lParam); 
    int yPos = GET_Y_LPARAM(lParam); 
    int mouseI = yPos / qHeight;
    int mouseJ = xPos / qWidth;
	if (mouseI >= blocksNum && mouseJ >= blocksNum) {
		numbers[mouseI][mouseJ] = (numbers[mouseI][mouseJ] + 1) % 3; 
	}
	
    ::GetClientRect( handle, &rect );
    ::InvalidateRect( handle, &rect, TRUE );  
}

LRESULT __stdcall CWindow::windowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    CWindow* that = reinterpret_cast<CWindow*>( ::GetWindowLong( hWnd, GWL_USERDATA ) );
    switch( message ) {
    case WM_NCCREATE: 
        {
            CREATESTRUCT* str = reinterpret_cast<CREATESTRUCT*>( lParam );
            ::SetWindowLong( hWnd, GWL_USERDATA, LONG( str->lpCreateParams ) );
            return ::DefWindowProc( hWnd, message, wParam, lParam );
        }
    case WM_PAINT:
        that->OnPaint();
        return 0;
    case WM_CLOSE:
        that->OnClose();
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_SIZE:
        that->OnSize( lParam );
        return 0;
    case WM_DESTROY:
        that->OnDestroy();
        return 0;
    case WM_COMMAND:
        that->OnCommand( hWnd, message, wParam, lParam );
        return 0;
    case WM_LBUTTONDOWN:
        that->OnClick( lParam );
        return 0;
	case WM_CTLCOLOREDIT:
		return that->OnCtlColorEdit((HDC)wParam, (HWND)lParam, that);
    default:
        return ::DefWindowProc( hWnd, message, wParam, lParam );
    }
}
