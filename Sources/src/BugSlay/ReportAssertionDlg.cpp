#include "StdAfx.h"

#include "CallStack.h"
#include "Resource.h"
#include "DialogFunctions.h"
#include "DialogMemory.h"
#include "WndUtils.h"

// //////////////////////////////////////////////////////////// 
static int nSplitter = -1;
static bool isMore = false;
static bool needSaveLog = true;

// CRAP
static HINSTANCE g_hInstance = nullptr;

// //////////////////////////////////////////////////////////// 
BOOL CALLBACK ReportAssertionDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
bool ReportAssertionDlgProcCommand(HWND hwndDlg, WORD wCtrlID, WORD wNotifCode, LPARAM lParam);

// //////////////////////////////////////////////////////////// 
struct SAssertionDlgParams
{
  const char *pszFileName;
  int nLineNumber;
  const char *pszCondition;
  const char *pszDescription;
  const CCallStackEntryList &entries;
  SIgnoresList &ignores;
  const char *pszExtInfo;
  //
  SAssertionDlgParams(const char *pszNewFileName, int nNewLineNumber,
                      const char *pszNewCondition, const char *pszNewDescription,
                      CCallStackEntryList &newEntries, SIgnoresList &newIgnores, const char *szExtInfo)
    : pszFileName(pszNewFileName), nLineNumber(nNewLineNumber),
      pszCondition(pszNewCondition), pszDescription(pszNewDescription),
      entries(newEntries), ignores(newIgnores), pszExtInfo(szExtInfo) {}
};

// //////////////////////////////////////////////////////////// 
EBSUReport __cdecl ShowAssertionDlg(HINSTANCE hInstance, HWND hWnd,
                                    const char *pszFileName, int nLineNumber,
                                    const char *pszCondition, const char *pszDescription,
                                    CCallStackEntryList &entries, SIgnoresList &ignores,
                                    const char *pszExtInfo)
{
  WriteReportToFile("error.txt", pszCondition, pszDescription, entries);
  g_hInstance = hInstance;
  // remember old cursor before dialog box call
  HCURSOR hCursor = GetCursor();
  // fill start parameters and execute dialog
  SAssertionDlgParams params(pszFileName, nLineNumber, pszCondition, pszDescription, entries, ignores, pszExtInfo);
  int nRetVal = DialogBoxParam(hInstance,
                               "IDD_ASSERTION_REPORT",
                               hWnd,
                               ReportAssertionDlgProc,
                               reinterpret_cast<LPARAM>(&params));
  // restore old cursor after message box usage
  SetCursor(hCursor);

  switch (nRetVal)
  {
    case IDC_ASSERTION_DEBUG:
      return BSU_DEBUG;
    case IDC_ASSERTION_ABORT:
      return BSU_ABORT;
    case IDC_ASSERTION_CONTINUE:
      return BSU_CONTINUE;
    case IDC_ASSERTION_IGNORE:
      return BSU_IGNORE;
    default:
      return BSU_DEBUG;
  }
}

// //////////////////////////////////////////////////////////// 
inline SAssertionDlgParams *GetParams(LPARAM lParam) { return reinterpret_cast<SAssertionDlgParams *>(lParam); }
// //////////////////////////////////////////////////////////// 
BOOL CALLBACK ReportAssertionDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_INITDIALOG:
    {
      // Calculate splitter
      RECT rcDlg, rc;
      GetWindowRect(hwndDlg, &rcDlg);
      GetWindowRect(GetDlgItem(hwndDlg, IDC_STATIC_SPLITTER), &rc);
      nSplitter = rcDlg.bottom - rcDlg.top - rc.top;
      if (!isMore) rcDlg.bottom -= nSplitter;
      MoveWindow(hwndDlg, (GetSystemMetrics(SM_CXFULLSCREEN) - rcDlg.right + rcDlg.left) / 2,
                 (GetSystemMetrics(SM_CYFULLSCREEN) - rcDlg.bottom + rcDlg.top) / 2,
                 rcDlg.right - rcDlg.left, rcDlg.bottom - rcDlg.top, true);

      SAssertionDlgParams *pParams = GetParams(lParam);
      SetDlgUserData(hwndDlg, pParams);

      SetWindowText(hwndDlg, IDC_ASSERTION_CONDITION, pParams->pszCondition);
      SetWindowText(hwndDlg, IDC_ASSERTION_DESCRIPTION, pParams->pszDescription);
      CheckDlgButton(hwndDlg, IDC_ASSERTION_SAVE_LOG, needSaveLog);

      HWND hwndCallStack = GetDlgItem(hwndDlg, IDC_ASSERTION_CALLSTACK);
      FillStackList(hwndCallStack, pParams->entries);

      // Fill ingnore list
      SetWindowUserData(hwndDlg, IDC_ASSERTION_IGNORES_LIST, &(pParams->ignores));
      HWND hwndList = GetDlgItem(hwndDlg, IDC_ASSERTION_IGNORES_LIST);
      SendMessage(hwndList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
      GetClientRect(hwndList, &rc);
      int nPart10 = (rc.right - rc.left) / 10;
      ListView_AddColumn(hwndList, "File", nPart10 * 3);
      ListView_AddColumn(hwndList, "Method", nPart10 * 3);
      ListView_AddColumn(hwndList, "Line", nPart10);
      ListView_AddColumn(hwndList, "Condition", nPart10 * 3);
      for (auto it = pParams->ignores.begin(); it != pParams->ignores.end(); ++it)
      {
        int nNewLine = ListView_AddItem(hwndList, GetFileName(it->szFileName), LPARAM(&(*it)));
        ListView_SetItemText(hwndList, nNewLine, 3, const_cast<char*>(it->szCondition.c_str()));
        ListView_SetItemText(hwndList, nNewLine, 1, const_cast<char*>(it->szFunctionName.c_str()));
        char buf[100];
        itoa(it->nLineNumber, buf, 10);
        ListView_SetItemText(hwndList, nNewLine, 2, buf);
      }
    }
    break;

    case WM_NOTIFY:
    {
      int idCtrl = static_cast<int>(wParam);
      if (IDC_ASSERTION_IGNORES_LIST == idCtrl)
      {
        auto lpnmitem = (LPNMITEMACTIVATE) lParam;
        HWND hwndList = GetDlgItem(hwndDlg, IDC_ASSERTION_IGNORES_LIST);
        switch (lpnmitem->hdr.code)
        {
          case NM_DBLCLK:
          {
            LVITEM Item;
            memset(&Item, 0, sizeof(Item));
            Item.mask = LVIF_PARAM | LVIF_TEXT;
            Item.iItem = lpnmitem->iItem;
            auto pIgnores = reinterpret_cast<SIgnoresList *>(GetWindowUserData(hwndDlg, IDC_ASSERTION_IGNORES_LIST));
            ListView_GetItem(hwndList, &Item);
            auto pIEnt = reinterpret_cast<SIgnoresEntry *>(Item.lParam);
            pIgnores->remove(*pIEnt);
            ListView_DeleteItem(hwndList, lpnmitem->iItem);
          }
          break;
        }
      }
    }
    break;

    case WM_COMMAND:
      return ReportAssertionDlgProcCommand(hwndDlg, LOWORD(wParam), HIWORD(wParam), lParam);
  }
  return false;
}

// //////////////////////////////////////////////////////////// 
bool ReportAssertionDlgProcCommand(HWND hwndDlg, WORD wCtrlID, WORD wNotifCode, LPARAM lParam)
{
  switch (wCtrlID)
  {
    case IDC_ASSERTION_DETAILS:
    {
      auto pParams = reinterpret_cast<SAssertionDlgParams *>(GetDlgUserData(hwndDlg));
      if (pParams->pszExtInfo)
        MessageBox(hwndDlg, pParams->pszExtInfo, "Details", MB_OK | MB_ICONINFORMATION);
    }
    break;
    case IDC_BUTTON_MORE:
    {
      RECT rc;
      GetWindowRect(hwndDlg, &rc);
      rc.bottom += (isMore = !isMore) ? nSplitter : -nSplitter;
      MoveWindow(hwndDlg, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, true);
      SetWindowText(hwndDlg, IDC_BUTTON_MORE, isMore ? "&More <<" : "&More >>");
    }
    break;
    case IDC_ASSERTION_SAVE_LOG:
      needSaveLog = IsDlgButtonChecked(hwndDlg, IDC_ASSERTION_SAVE_LOG) == 1;
      break;

    /* case IDC_ASSERTION_DETAILS:
         */
    case IDC_ASSERTION_DEBUG:
    case IDC_ASSERTION_ABORT:
    case IDC_ASSERTION_CONTINUE:
      EndDialog(hwndDlg, wCtrlID);
      return true;

    case IDC_ASSERTION_IGNORE: // add current assertion to the ignores list
    {
      auto pIgnores = reinterpret_cast<SIgnoresList *>(GetWindowUserData(hwndDlg, IDC_ASSERTION_IGNORES_LIST));
      if (pIgnores)
      {
        SIgnoresEntry ignore;
        // GetCheckButtonState( hwndDlg, IDC_ASSERTION_IGNORE_THIS );
        auto pParams = reinterpret_cast<SAssertionDlgParams *>(GetDlgUserData(hwndDlg));
        if (pParams)
        {
          if (!pParams->entries.empty()) ignore.szFunctionName = pParams->entries.front().szFunctionName;
          else ignore.szFunctionName = _T("<UNKNOWN>");
          ignore.szCondition = pParams->pszCondition;
          ignore.szFileName = pParams->pszFileName;
          ignore.nLineNumber = pParams->nLineNumber;
          ignore.dwFlags = IGNORE_THIS;
          /* ignore.dwFlags = ( GetCheckButtonState( hwndDlg, IDC_ASSERTION_IGNORE_THIS ) == BST_CHECKED ? IGNORE_THIS : 0 ) |
                                        */
          pIgnores->push_back(ignore);
        }
      }
    }
      EndDialog(hwndDlg, IDC_ASSERTION_CONTINUE);
      return true;
    case IDC_ASSERTION_EMERGENCY:
      NBugSlayer::ExecuteEmergencyCommands();
      EndDialog(hwndDlg, IDC_ASSERTION_ABORT);
      return true;
  }
  return false;
}

// //////////////////////////////////////////////////////////// 