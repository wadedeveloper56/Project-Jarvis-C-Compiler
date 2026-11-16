//////////////////////////////////////////////
// Files.cpp - Definitions for CViewFiles, CContainFiles
//             and DockFiles classes

#include "pch.h"
#include "Files.h"
#include "resource.h"

constexpr COLORREF magenta = RGB(255, 0, 255);
constexpr COLORREF red = RGB(255, 0, 0);

/////////////////////////////////
// CViewTree function definitions
//

// Destructor.
CViewTree::~CViewTree()
{
    if (IsWindow()) DeleteAllItems();
}

// Called when a window handle (HWND) is attached to CViewTree.
void CViewTree::OnAttach()
{
    // Set the image lists.
    CBitmap bmImage(IDB_CLASSVIEW);
    bmImage = DpiScaleUpBitmap(bmImage);
    int scale = bmImage.GetSize().cy / 15;
    CImageList normalImages;
    normalImages.Create(scale * 16, scale * 15, ILC_COLOR32 | ILC_MASK, 1, 0);
    normalImages.Add(bmImage, red);
    SetImageList(normalImages, TVSIL_NORMAL);

    // Adjust style to show lines and [+] button.
    DWORD style = GetStyle();
    style |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
    SetStyle(style);

    DeleteAllItems();

    // Add some tree-view items.
    HTREEITEM htiRoot = InsertItem(L"TreeView", 0, 0);
    HTREEITEM htiCTreeViewApp = InsertItem(L"CTreeViewApp", 1, 1, htiRoot);
    InsertItem(L"CTreeViewApp()", 3, 3, htiCTreeViewApp);
    InsertItem(L"GetMainFrame()", 3, 3, htiCTreeViewApp);
    InsertItem(L"InitInstance()", 3, 3, htiCTreeViewApp);
    HTREEITEM htiMainFrame = InsertItem(L"CMainFrame", 1, 1, htiRoot);
    InsertItem(L"CMainFrame()", 3, 3, htiMainFrame);
    InsertItem(L"OnCommand()", 4, 4, htiMainFrame);
    InsertItem(L"OnInitialUpdate()", 4, 4, htiMainFrame);
    HTREEITEM htiView = InsertItem(L"CView", 1, 1, htiRoot);
    InsertItem(L"CView()", 3, 3, htiView);
    InsertItem(L"OnInitialUpdate()", 4, 4, htiView);
    InsertItem(L"WndProc()", 4, 4, htiView);

    // Expand some tree-view items.
    Expand(htiRoot, TVE_EXPAND);
    Expand(htiCTreeViewApp, TVE_EXPAND);
}

// Handle the window's messages.
LRESULT CViewTree::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        // Pass unhandled messages on for default processing.
        return WndProcDefault(msg, wparam, lparam);
    }

    // Catch all unhandled CException types.
    catch (const CException& e)
    {
        // Display the exception and continue.
        CString str1;
        str1 << e.GetText() << L'\n' << e.GetErrorString();

        CString str2;
        str2 << "Error: " << e.what();
        ::MessageBox(nullptr, str1, str2, MB_ICONERROR);
    }

    // Catch all unhandled std::exception types.
    catch (const std::exception& e)
    {
        // Display the exception and continue.
        CString str1 = e.what();
        ::MessageBox(nullptr, str1, L"Error: std::exception", MB_ICONERROR);
    }

    return 0;
}

/////////////////////////////////////
// CContainFiles function definitions
//

// Constructor.
CContainFiles::CContainFiles()
{
    SetTabText(L"FileView");
    SetTabIcon(IDI_FILEVIEW);
    SetDockCaption (L"File View - Docking container");
    SetView(m_viewTree);
}

// Handle the window's messages.
LRESULT CContainFiles::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        // Pass unhandled messages on for default processing.
        return WndProcDefault(msg, wparam, lparam);
    }

    // Catch all unhandled CException types.
    catch (const CException& e)
    {
        // Display the exception and continue.
        CString str1;
        str1 << e.GetText() << L'\n' << e.GetErrorString();

        CString str2;
        str2 << "Error: " << e.what();
        ::MessageBox(nullptr, str1, str2, MB_ICONERROR);
    }

    // Catch all unhandled std::exception types.
    catch (const std::exception& e)
    {
        // Display the exception and continue.
        CString str1 = e.what();
        ::MessageBox(nullptr, str1, L"Error: std::exception", MB_ICONERROR);
    }

    return 0;
}


///////////////////////////////////
//  CDockFiles function definitions
//

// Constructor.
CDockFiles::CDockFiles()
{
    SetView(m_files);
    SetBarWidth(8);
}

// Handle the window's messages.
LRESULT CDockFiles::WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
    try
    {
        // Pass unhandled messages on for default processing.
        return WndProcDefault(msg, wparam, lparam);
    }

    // Catch all unhandled CException types.
    catch (const CException& e)
    {
        // Display the exception and continue.
        CString str1;
        str1 << e.GetText() << L'\n' << e.GetErrorString();

        CString str2;
        str2 << "Error: " << e.what();
        ::MessageBox(nullptr, str1, str2, MB_ICONERROR);
    }

    // Catch all unhandled std::exception types.
    catch (const std::exception& e)
    {
        // Display the exception and continue.
        CString str1 = e.what();
        ::MessageBox(nullptr, str1, L"Error: std::exception", MB_ICONERROR);
    }

    return 0;
}
