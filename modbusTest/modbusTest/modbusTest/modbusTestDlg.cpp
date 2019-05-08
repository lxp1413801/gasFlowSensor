
// modbusTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "modbusTest.h"
#include "modbusTestDlg.h"
#include "afxdialogex.h"
#include "stdint.h"

#include "m_string.h"
#include "modbus.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
	volatile uint8_t recBuf[RECEVIED_BUFFER_LEN];
	volatile uint16_t recLen;
//HANDLE __hCom;
HANDLE __hCommThread;
HWND __hwndMain = NULL;

uint32_t FlowClibValue=0;
int FlowClibPoint=0;

int modbusAddrCount;
uint8_t modbusAddrBuf[32];
bool Cover=false;
//
uint8_t readStardMeterModeBusCmd[16];
bool StantardMeterOnline=false;
//
static uint8_t bShowData[RECEVIED_BUFFER_LEN];//= new byte[dwRead];

HANDLE hThreadModbusRead=NULL;
HANDLE hThreadModebusPoll=NULL;
HANDLE hThreadModebusWriteClib=NULL;
HANDLE hCommReceivedThread=NULL;
bool hThreadModbusReadExitIrq=false;
bool modbusPollingFlg=false;
const uint32_t BaudRateTable[14]=
{
	CBR_110, 
	CBR_300, 
	CBR_600, 
	CBR_1200, 
	CBR_2400, 
	CBR_4800, 
	CBR_9600, 
	CBR_14400, 
	CBR_19200, 
	CBR_38400, 
	CBR_57600, 
	CBR_115200, 
	CBR_128000, 
	CBR_256000, 
};
#define BAUD_RATE_DEFAULT_IND 6
typedef struct __DCB_BITES_INF
{
	TCHAR  Name[10];
	BYTE bitsNum;
}DCB_BITES_INF,*pDCB_BITES_INF;
const DCB_BITES_INF ParityTable[5]=
{
	{_T("EVEN"),EVENPARITY},
	{_T("MARK"), MARKPARITY},
	{_T("NONE"), NOPARITY},
	{_T("ODDP"),ODDPARITY}, 
	{_T("SPACE"),SPACEPARITY},
};
#define PARITY_DEFAULT_IND 2
const DCB_BITES_INF  StopBitsTable[3]=
{
	{_T("1"),ONESTOPBIT },
	{_T("1.5") ,ONE5STOPBITS},
	{_T("2") ,TWOSTOPBITS},
};
#define STOP_DEFAULT_IND 0
const BYTE  ByteSizeTable[4]=
{
	5,6,7,8,	
};
#define BYTE_WIDTH_DEFAULT_IND 3
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CmodbusTestDlg 对话框




CmodbusTestDlg::CmodbusTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CmodbusTestDlg::IDD, pParent)
	, uartRreceivedFlg(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CmodbusTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_calibration_point, m_calibration_point);
	DDX_Control(pDX, IDC_LIST2, m_list);
	DDX_Control(pDX, IDC_EDIT_REFERENCE_DEVICE_ID, m_referenceDeviceId);
	DDX_Control(pDX, IDC_LIST_IMD_MSG, m_listImdMsg);
	DDX_Control(pDX, IDC_EDIT_MODBUS_ADDR, m_manulInputAddr);
	DDX_Control(pDX, IDC_CHECK_MAX_ENABLE, m_check_cover);
	DDX_Control(pDX, IDC_EDIT_PRIO_SETED, m_proSetedPoint);
	DDX_Control(pDX, IDC_ID_SET_VALUE, m_idSetValue);
	DDX_Control(pDX, IDC_CHECK_MODIFY_SET_EN,m_modSetEn);
	DDX_Control(pDX, IDC_EDIT_INPUT_FULL,m_editFullFlow);

	DDX_Control(pDX, IDC_EDIT_OUT_ZERO,m_editOutZero);
	DDX_Control(pDX, IDC_EDIT_OUT_FULL,m_editOutFull);

	DDX_Control(pDX,IDC_CHECK_PID_OP_EN,m_checkPidOpEn);
	DDX_Control(pDX,IDC_EDIT_PID_SET_ID,m_pidSetID);//CEdit m_pidSetID;//IDC_EDIT_PID_SET_ID
	DDX_Control(pDX,IDC_EDIT_PID_SET_P,m_pidSetP);//CEdit m_pidSetID;//IDC_EDIT_PID_SET_ID
	DDX_Control(pDX,IDC_EDIT_PID_SET_I,m_pidSetI);//CEdit m_pidSetID;//IDC_EDIT_PID_SET_ID
	DDX_Control(pDX,IDC_EDIT_PID_SET_D,m_pidSetD);//CEdit m_pidSetID;//IDC_EDIT_PID_SET_ID
}

BEGIN_MESSAGE_MAP(CmodbusTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CmodbusTestDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_COM, &CmodbusTestDlg::OnBnClickedButtonOpenCom)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_COM, &CmodbusTestDlg::OnBnClickedButtonCloseCom)
	ON_BN_CLICKED(IDC_BUTTON_COM_SEND_TEST, &CmodbusTestDlg::OnBnClickedButtonComSendTest)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_previous_point, &CmodbusTestDlg::OnBnClickedButtonpreviouspoint)
	ON_BN_CLICKED(IDC_BUTTON_last_point, &CmodbusTestDlg::OnBnClickedButtonlastpoint)
	ON_EN_CHANGE(IDC_EDIT_REFERENCE_DEVICE_ID, &CmodbusTestDlg::OnEnChangeEditReferenceDeviceId)
	//ON_BN_CLICKED(IDC_BUTTON_SET_ZERO2, &CmodbusTestDlg::OnBnClickedButtonSetZero2)
	ON_BN_CLICKED(IDC_BUTTON_POOL_SALAVE, &CmodbusTestDlg::OnBnClickedButtonPoolSalave)
	ON_BN_CLICKED(IDC_BUTTON_SET_ZERO, &CmodbusTestDlg::OnBnClickedButtonSetZero)
	ON_CBN_SELCHANGE(IDC_COMBO_COM_PORT, &CmodbusTestDlg::OnCbnSelchangeComboComPort)
	ON_CBN_DROPDOWN(IDC_COMBO_COM_PORT, &CmodbusTestDlg::OnDropdownComboComPort)
	//ON_BN_CLICKED(IDC_BUTTON_START_POLL, &CmodbusTestDlg::OnBnClickedButtonStartPoll)
	//ON_BN_CLICKED(IDC_BUTTON_STOP_POLL, &CmodbusTestDlg::OnBnClickedButtonStopPoll)
	ON_BN_CLICKED(IDC_BUTTON_SET_POINT, &CmodbusTestDlg::OnBnClickedButtonSetPoint)
	ON_EN_CHANGE(IDC_EDIT_calibration_point, &CmodbusTestDlg::OnEnChangeEditcalibrationpoint)
	ON_BN_CLICKED(IDC_BUTTON_START_READ, &CmodbusTestDlg::OnBnClickedButtonStartRead)
	ON_BN_CLICKED(IDC_BTN_SET_ID, &CmodbusTestDlg::OnBnClickedBtnSetId)
	//ON_EN_CHANGE(IDC_EDIT_INPUT_MV, &CmodbusTestDlg::OnEnChangeEditInputMv)
	ON_BN_CLICKED(IDC_CHECK_MODIFY_SET_EN, &CmodbusTestDlg::OnBnClickedCheckModifySetEn)
	ON_BN_CLICKED(IDC_CHECK_PID_OP_EN, &CmodbusTestDlg::OnBnClickedCheckPidOpEn)
	ON_BN_CLICKED(IDC_BTN_PID_RD, &CmodbusTestDlg::OnBnClickedBtnPidRd)
	ON_BN_CLICKED(IDC_BTN_PID_WR, &CmodbusTestDlg::OnBnClickedBtnPidWr)
END_MESSAGE_MAP()


// CmodbusTestDlg 消息处理程序
CRITICAL_SECTION cs;
BOOL CmodbusTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	InitializeCriticalSection(&cs);
	m_dlg_init_call_back();
	__hwndMain=this->m_hWnd ;
	receivedBufCount=0;
	modBusAddrCount=0;
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CmodbusTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CmodbusTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CmodbusTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




int CmodbusTestDlg::enum_com_port(void)
{
	int i=0;
	CString str;
	CComboBox* pComboBox;
	pComboBox=((CComboBox*)GetDlgItem(IDC_COMBO_COM_PORT));
	pComboBox->ResetContent();
	HKEY hKey;    
	if(ERROR_SUCCESS ==::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Hardware\\DeviceMap\\SerialComm"),NULL, KEY_READ, &hKey ))//打开串口注册表对应的键值   
	{
		i=0;
		TCHAR  RegKeyName[128],SerialPortName[128];
		DWORD  dwLong,dwSize; 
		while(1){  
			dwLong = dwSize=sizeof(RegKeyName); 
			long ret;
			ret=::RegEnumValue(hKey,i,RegKeyName,&dwLong,NULL,NULL,(PUCHAR)SerialPortName, &dwSize);
			if(ret==ERROR_NO_MORE_ITEMS)break;
			memcmp(RegKeyName,"\\Device\\",8);
			pComboBox->AddString(SerialPortName);
			i++;
		}
		RegCloseKey(hKey); 
	}
	else{
		RegCloseKey(hKey);
	}
	if((pComboBox->GetCount())>0){
		pComboBox->SetCurSel(0);
	}
	return 0;
}


int CmodbusTestDlg::m_comb_init(void)
{
	int i=0;
	CString str;
	CComboBox* pComboBox;

	enum_com_port();
	//
	pComboBox=((CComboBox*)GetDlgItem(IDC_COMBO_UART_DATA_WIDTH));
	pComboBox->ResetContent();
	for(i=0;i<sizeof(ByteSizeTable)/sizeof(BYTE);i++){
		str.Format(_T("%d"),ByteSizeTable[i]); 
		pComboBox->AddString(str) ;
	}
	if((pComboBox->GetCount())>0){
		pComboBox->SetCurSel(BYTE_WIDTH_DEFAULT_IND);
	}
	//
	pComboBox=((CComboBox*)GetDlgItem(IDC_COMBO_UART_BAUD_RATE));
	pComboBox->ResetContent();
	for(i=0;i<sizeof(BaudRateTable)/sizeof(BaudRateTable[0]);i++){
		str.Format(_T("%d"),BaudRateTable[i]); 
		pComboBox->AddString(str) ;
	}
	if((pComboBox->GetCount() )>0){
		pComboBox->SetCurSel(BAUD_RATE_DEFAULT_IND);
	}
	//
	pComboBox=((CComboBox*)GetDlgItem(IDC_COMBO_UART_STOP_BITS));
	pComboBox->ResetContent();
	for(i=0;i<sizeof(StopBitsTable)/sizeof(StopBitsTable[0]);i++){
		pComboBox->AddString(StopBitsTable[i].Name) ;
	}
	if((pComboBox->GetCount())>0){
		pComboBox->SetCurSel(STOP_DEFAULT_IND);
	}
	//
	pComboBox=((CComboBox*)GetDlgItem(IDC_COMBO_UART_PARITY_BIT));
	pComboBox->ResetContent();
	for(i=0;i<sizeof(ParityTable)/sizeof(ParityTable[0]);i++){
		pComboBox->AddString(ParityTable[i].Name) ;
	}
	if((pComboBox->GetCount() )>0){
		pComboBox->SetCurSel(PARITY_DEFAULT_IND);
	}
	//IDC_COMBO_UART_FLOW_CTRL
	pComboBox=((CComboBox*)GetDlgItem(IDC_COMBO_UART_FLOW_CTRL));
	pComboBox->AddString(__T("NONE"));
	pComboBox->SetCurSel(0);
	return 0;
}


int CmodbusTestDlg::m_button_init(void)
{
	/*
CFont * f; 
     f = new CFont; 
     f->CreateFont(48, // nHeight 
     0, // nWidth 
     0, // nEscapement 
     0, // nOrientation 
     FW_BOLD, // nWeight 
     false, // bItalic 
     false, // bUnderline 
     0, // cStrikeOut 
     ANSI_CHARSET, // nCharSet 
     OUT_DEFAULT_PRECIS, // nOutPrecision 
     CLIP_DEFAULT_PRECIS, // nClipPrecision 
     DEFAULT_QUALITY, // nQuality 
     DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
     _T("宋体")); // lpszFac 
   
     GetDlgItem(IDC_BUTTON_SET_ZERO)->SetFont(f);
	 GetDlgItem(IDC_BUTTON_SET_POINT)->SetFont(f);
	 //
	 GetDlgItem(IDC_BUTTON_previous_point)->SetFont(f);
	 GetDlgItem(IDC_BUTTON_last_point)->SetFont(f);
	 GetDlgItem(IDC_EDIT_calibration_point)->SetFont(f);
	 //GetDlgItem(IDC_EDIT_calibration_point)->set
	CFont * ff; 
		 ff = new CFont; 
		 ff->CreateFont(96, // nHeight 
		 0, // nWidth 
		 0, // nEscapement 
		 0, // nOrientation 
		 FW_BOLD, // nWeight 
		 false, // bItalic 
		 false, // bUnderline 
		 0, // cStrikeOut 
		 ANSI_CHARSET, // nCharSet 
		 OUT_DEFAULT_PRECIS, // nOutPrecision 
		 CLIP_DEFAULT_PRECIS, // nClipPrecision 
		 DEFAULT_QUALITY, // nQuality 
		 DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		 _T("黑体")); // lpszFac 
		 m_calibration_point.SetFont(ff);
		 */
		m_calibration_point.SetWindowTextW(__T("-1"));
		//list//http://blog.csdn.net/tanghaili/article/details/8162269
		//http://www.jizhuomi.com/software/197.html
		m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);   
		CRect rect;   
  
		// 获取编程语言列表视图控件的位置和大小   
		m_list.GetClientRect(&rect);   

		// 为列表视图控件添加三列   
		m_list.InsertColumn(0, _T("说明"), LVCFMT_CENTER,			rect.Width()*1/8,	0);
		m_list.InsertColumn(1, _T("modBUS地址"), LVCFMT_CENTER,		rect.Width()*2/8,	1);  
		m_list.InsertColumn(2, _T("通讯状态"), LVCFMT_CENTER,		rect.Width()*1/8,	2);
		m_list.InsertColumn(3, _T("电压(mv)"), LVCFMT_CENTER,		rect.Width()*2/8,	3);   
		m_list.InsertColumn(4, _T("流量(l/min)"), LVCFMT_CENTER,	rect.Width()*2/8,	4);   

	//
   // CRect rect;   
  
    // 获取编程语言列表视图控件的位置和大小   
    //m_programLangList.GetClientRect(&rect);   
		m_proSetedPoint.SetWindowTextW(__T("-1"));
		m_referenceDeviceId.SetWindowTextW(__T("253"));
	return 0;
}


int CmodbusTestDlg::m_dlg_init_call_back(void)
{
	hCom=NULL;
	//hCommThread=NULL;
	m_ComPortOpened=false;
	m_button_init();
	m_comb_init();
	m_editFullFlow.SetWindowTextW(__T("300.0"));
	m_editOutZero.SetWindowTextW(__T("500.0"));
	m_editOutFull.SetWindowTextW(__T("5500.0"));
	OnBnClickedCheckModifySetEn();
	get_full();
	OnBnClickedCheckPidOpEn();
	return 0;
}




bool CmodbusTestDlg::m_com_open(void)
{
	//http://blog.csdn.net/l_andy/article/details/51131232
	int sel=0;
	int q31;
	//int tmp;
	CComboBox* pComboBox;
	CString str;
	pComboBox=((CComboBox*)GetDlgItem(IDC_COMBO_COM_PORT));
	sel=pComboBox->GetCurSel();
	//pComboBox->
	pComboBox->GetLBText(sel,str); 
	CString ss=__T("\\\\.\\");
	str=ss+str;
	hCom = CreateFile( str,
					GENERIC_READ|GENERIC_WRITE, 
					0,
					NULL ,
					OPEN_EXISTING,
					//FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
					0,
					NULL);

	if ( hCom== INVALID_HANDLE_VALUE )
	{
		::AfxMessageBox(__T("打开串口失败!!!"));
		return false;
		
	}
	m_ComPortOpened=true;
	SetupComm(hCom, 8192, 8192);
	DCB wdcb;
	GetCommState( hCom, &wdcb ) ;

	TRACE(_T("DCBlength :%d\n"), wdcb.DCBlength );
	TRACE(_T("BaudRate :%d\n"), wdcb.BaudRate );
	TRACE(_T("fBinary :%d\n"), wdcb.fBinary );
	TRACE(_T("fParity :%d\n"), wdcb.fParity );
	TRACE(_T("fOutxCtsFlow :%d\n"), wdcb.fOutxCtsFlow );
	TRACE(_T("fOutxDsrFlow :%d\n"), wdcb.fOutxDsrFlow );
	TRACE(_T("fDtrControl :%d\n"), wdcb.fDtrControl );
	TRACE(_T("fDsrSensitivity :%d\n"), wdcb.fDsrSensitivity );	
	TRACE(_T("fTXContinueOnXoff :%d\n"), wdcb.fTXContinueOnXoff );
	TRACE(_T("fOutX :%d\n"), wdcb.fOutX );
	TRACE(_T("fInX :%d\n"), wdcb.fInX );
	TRACE(_T("fErrorChar :%d\n"), wdcb.fErrorChar );
	TRACE(_T("fNull :%d\n"), wdcb.fNull );
	TRACE(_T("fRtsControl :%d\n"), wdcb.fRtsControl );
	TRACE(_T("fAbortOnError :%d\n"), wdcb.fAbortOnError );
	TRACE(_T("fDummy2 :%d\n"), wdcb.fDummy2 );
//	TRACE(_T("wReserved :%d\n"), wdcb.wReserved );
	TRACE(_T("XonLim :%d\n"), wdcb.XonLim );
	TRACE(_T("XoffLim :%d\n"), wdcb.XoffLim );;        
	TRACE(_T("ByteSize :%d\n"), wdcb.ByteSize );
	TRACE(_T("Parity :%d\n"), wdcb.Parity );
	TRACE(_T("StopBits :%d\n"), wdcb.StopBits );
	TRACE(_T("XonChar :%d\n"), wdcb.XonChar );
	TRACE(_T("XoffChar :%d\n"), wdcb.XoffChar );
	TRACE(_T("ErrorChar :%d\n"), wdcb.ErrorChar );
	TRACE(_T("EofChar :%d\n"), wdcb.EofChar );
	TRACE(_T("EvtChar :%d\n"), wdcb.EvtChar );
	TRACE(_T("wReserved1 :%d\n"), wdcb.wReserved1 );


	pComboBox=((CComboBox*)GetDlgItem(IDC_COMBO_UART_BAUD_RATE));
	sel=pComboBox->GetCurSel();
	q31=BaudRateTable[sel];
	wdcb.BaudRate = q31 ;
	//
	pComboBox=((CComboBox*)GetDlgItem(IDC_COMBO_UART_DATA_WIDTH));
	sel=pComboBox->GetCurSel();
	q31= ByteSizeTable[sel] ;
 	wdcb.ByteSize = q31;
	//
	pComboBox=((CComboBox*)GetDlgItem(IDC_COMBO_UART_PARITY_BIT));
	sel=pComboBox->GetCurSel();
	q31=ParityTable[sel].bitsNum ;
	wdcb.Parity= NOPARITY;
	//wdcb.Parity=q31;
	//
	pComboBox=((CComboBox*)GetDlgItem(IDC_COMBO_UART_STOP_BITS));
	sel=pComboBox->GetCurSel();
	q31=StopBitsTable[sel].bitsNum ;
 	wdcb.StopBits = ONESTOPBIT;
	//wdcb.StopBits =q31;

	SetCommState( hCom, &wdcb ) ;
	
	COMMTIMEOUTS _CO;
	if(!::GetCommTimeouts(hCom, &_CO))
	return false;
	_CO.ReadIntervalTimeout = 10;
	_CO.ReadTotalTimeoutMultiplier =256;
	_CO.ReadTotalTimeoutConstant = 5000;
	_CO.WriteTotalTimeoutMultiplier = 100;
	_CO.WriteTotalTimeoutConstant = 5000;
	if(!::SetCommTimeouts(hCom, &_CO))
	return false; 
	PurgeComm( hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR );
	return true;
}


void CmodbusTestDlg::m_com_close(void)
{
	if(hCom){
		CloseHandle(hCom);
		hCom=NULL;
	}
	m_ComPortOpened=false;
}


void CmodbusTestDlg::OnBnClickedButtonCloseCom()
{
	// TODO: 在此添加控件通知处理程序代码

	//
	/*
	HANDLE hThreadModbusRead=NULL;
	HANDLE hThreadModebusPoll=NULL;
	HANDLE hThreadModebusWriteClib=NULL;
	HANDLE hCommReceivedThread=NULL;

	*/
	DWORD dw=0;
	if((void*)hThreadModbusRead){
		TerminateThread(hThreadModbusRead,dw);
		CloseHandle(hThreadModbusRead);		
	}
	if((void*)hThreadModebusPoll){
		TerminateThread(hThreadModebusPoll,dw);
		CloseHandle(hThreadModebusPoll);	
	}
	if((void*)hThreadModebusWriteClib){
		TerminateThread(hThreadModebusWriteClib,dw);
		CloseHandle(hThreadModebusWriteClib);
	}
	if((void*)hCommReceivedThread){
		TerminateThread(hCommReceivedThread,dw);
		CloseHandle(hCommReceivedThread);
	}
	if((void*)hModbusXprocessThread){
		TerminateThread(hModbusXprocessThread,dw);
		CloseHandle(hModbusXprocessThread);	
		modbusMachineStatus=MODBUS_MS_NONE;
	}

	CComboBox* pComboBox;
	pComboBox=((CComboBox*)GetDlgItem(IDC_COMBO_COM_PORT));
	m_com_close();
	if(m_ComPortOpened){
		pComboBox->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_OPEN_COM)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_CLOSE_COM)->EnableWindow(true);
	}else{
		pComboBox->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_OPEN_COM)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_CLOSE_COM)->EnableWindow(false);
	}

}


//======================================================================
//稳定规范的代码
//======================================================================
DWORD WINAPI ThreadProcReceived(LPVOID lpParameter )
{	
	OVERLAPPED m_osRead;
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	
	DWORD dwBytesRead = 0 ;
	bool bReadStat;
	byte bGet[RECEVIED_BUFFER_LEN] = {0};
	unsigned short i=0;
	while ( 1 )
	{
		Sleep(50);
		memset(bGet, 0, RECEVIED_BUFFER_LEN );
		dwBytesRead = 0;
		memset( &m_osRead, 0, sizeof(OVERLAPPED) );
		m_osRead.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);	
		ClearCommError(hCom, &dwErrorFlags, &ComStat);

		bReadStat=ReadFile(hCom, bGet,ComStat.cbInQue, &dwBytesRead, &m_osRead);
		if( !bReadStat ){
			if(GetLastError()==ERROR_IO_PENDING){
				dwBytesRead=0;
				DWORD dwdResult = WaitForSingleObject(m_osRead.hEvent,2000);
				if ( dwdResult == WAIT_TIMEOUT ){
					PurgeComm(hCom,  PURGE_RXABORT|PURGE_RXCLEAR);
					TRACE(_T("Read Failed!\n"));
				}else{	
					GetOverlappedResult(hCom,&m_osRead,	&dwBytesRead,TRUE ) ;	
				}
				PurgeComm(hCom, PURGE_TXABORT| PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR); 
			}
		}
		if ( dwBytesRead != 0 ){
			memcpy( bShowData, bGet, dwBytesRead );
			::SendMessage(__hwndMain, WM_COM_RECEIVED, dwBytesRead, (LPARAM)bShowData );
			TRACE(_T("Get data Count : %d\n"),dwBytesRead );
			for ( DWORD i=0; i< dwBytesRead; i++ ){
				TRACE(_T("%02x "), bGet[i]);
			}	
		}	
	}	
	return 1;
}
LRESULT CmodbusTestDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	CString str;
	uint8_t buf[RECEVIED_BUFFER_LEN];
	int nCurSel;
	uint16_t count;
	uint16_t i;
	switch(message){
	case WM_COM_RECEIVED:
		if(receivedBufCount<256){
			if((void*)lParam!=NULL && wParam<256){
				memcpy(receivedBuf+receivedBufCount,(uint8_t*)lParam,(uint16_t)wParam);
				receivedBufCount+=wParam;
			}
		}
		KillTimer(1);
		SetTimer(1,300,NULL);
		break;
	case WM_COM_SENDED:
		m_com_imd_msg(__T(" Sended:"),(uint8_t*)lParam,(uint16_t)wParam);
		break;
	default:
		break;
	
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}
int CmodbusTestDlg::m_modebus_received_process(uint8_t* buf, uint16_t len)
{
	int count,i;
	CString str;
	uint16_t t16;
	if(len>256)len=256;
	t16=crc_verify(buf,len);
	if(!t16)return 0;
	//if(buf[1]!=FUNC_READ_HOLDING_REGISTERS)return 0;
	modbusMachineStatus_t ms;
	ms=modbusMachineStatus;
	switch(ms){
		case MODBUS_MS_POLL_ID:
			if(buf[1]!=0x03)break;
			count=m_list.GetItemCount();		
			for(i=0;i<count;i++){
				str=m_list.GetItemText(i,1);
				if(buf[0]==	_tstoi(str.GetBuffer(str.GetLength())))break;
			}
			if(i>=count){
				m_list.InsertItem(count, __T("被标定表"));
				str.Format(__T("%03d"),buf[0]);
				m_list.SetItemText(count,1,str);
				//滚动到最后一行
				//http://bbs.csdn.net/topics/60486645
				m_list.SendMessage(WM_VSCROLL, SB_BOTTOM, NULL);
			}
			break;
		case MODBUS_MS_READ_VO:
			if(buf[1]!=0x03)break;
			count=m_list.GetItemCount();		
			for(i=0;i<count;i++){
				str=m_list.GetItemText(i,1);
				if(buf[0]==	_tstoi(str.GetBuffer(str.GetLength())))break;
			}	
			if(i<count){
				//获取电压值
				int t32;
				t32=(buf[3]<<8 | buf[4]);
				t32>>=1;
				str.Format(__T("%04d"),t32);
				m_list.SetItemText(i,3,str);
				//折算为流量
				float flow=modBusParam.fullFlow*(t32-modBusParam.outMvZero)/(modBusParam.outMvFull-modBusParam.outMvZero);
				//str=__T("mv");
				str.Format(__T("%.1f"),flow);
				m_list.SetItemText(i,4,str);
			}
			break;
		case MODBUS_MS_SET_POINT:
			if(buf[1]!=0x06 && buf[1]!=0x26)break;
			count=m_list.GetItemCount();		
			for(i=0;i<count;i++){
				str=m_list.GetItemText(i,1);
				if(buf[0]==	_tstoi(str.GetBuffer(str.GetLength())))break;
			}	
			if(i<count){
				//设置成功标准为
				str=__T("成功");
				m_list.SetItemText(i,2,str);
			}
			break;
		case MODBUS_MS_GET_PID:
			if(buf[1]!=FUNC_READ_HOLDING_REGISTERS)break;
			if(buf[0]!=modBusParam.pidDeviceId)break;
			uint16_t t16;
			t16=(buf[3]<<8) | buf[4];
			str.Format(__T("%d"),t16);
			m_pidSetP.SetWindowTextW(str);

			t16=(buf[5]<<8) | buf[6];
			str.Format(__T("%d"),t16);
			m_pidSetI.SetWindowTextW(str);

			t16=(buf[7]<<8) | buf[8];
			str.Format(__T("%d"),t16);
			m_pidSetD.SetWindowTextW(str);
	}
	return 0;
}

int CmodbusTestDlg::m_com_imd_msg(CString header , uint8_t* buf, uint16_t len)
{
	CString str;
	if(len>100)len=100;
	buf[len]='\0';
	uint8_t hex[256];
	int tt=m_str_b2h(hex,buf,len);
	hex[tt]='\0';
	str=hex;
	//str.Format(__T("%02x"),buf);
	str=header+str;
	//m_listImdMsg.AddString(str);
	if(m_listImdMsg.GetCount()>256)m_listImdMsg.ResetContent();

	m_listImdMsg.InsertString(m_listImdMsg.GetCount(),str);
	
	int count;
	count=m_listImdMsg.GetCount();
	if(count>1)	m_listImdMsg.SetCurSel(count-1);
	
	return 0;
}
void CmodbusTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString str;

	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent){
	case 1:
		KillTimer(1);
		memcpy((void*)recBuf,receivedBuf,receivedBufCount);
		recLen=receivedBufCount;
		receivedBufCount=0;
		recBuf[recLen]='\0';
		m_com_imd_msg(__T("received:"),(uint8_t* )recBuf,recLen);
		m_modebus_received_process((uint8_t*)recBuf,recLen);
		//m_com_received_echo(recBuf,recLen);
		uartRreceivedFlg=true;
		break;
	case 2:
		KillTimer(2);
		m_referenceDeviceId.GetWindowTextW(str);
		/*
		if(str==__T("")){
			//MessageBox(__T("请输入"));
		}
		*/
		break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CmodbusTestDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
}

void CmodbusTestDlg::OnBnClickedButtonOpenCom()
{
	// TODO: 在此添加控件通知处理程序代码
	
	CString str;
	CComboBox* pComboBox;
	pComboBox=((CComboBox*)GetDlgItem(IDC_COMBO_COM_PORT));
	
	if(!m_com_open()){
		if(m_ComPortOpened){
			m_com_close();
		}
	}
	if(m_ComPortOpened){
		//pComboBox->setw
		pComboBox->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_OPEN_COM)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_CLOSE_COM)->EnableWindow(true);
	}else{
		pComboBox->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_OPEN_COM)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_CLOSE_COM)->EnableWindow(false);
	}
	//__hCom=hCom;
	DWORD threadID;
 	hCommReceivedThread = ::CreateThread((LPSECURITY_ATTRIBUTES)NULL, 0,(LPTHREAD_START_ROUTINE)ThreadProcReceived, __hwndMain, 0, &threadID);
 	hModbusXprocessThread=::CreateThread((LPSECURITY_ATTRIBUTES)NULL, 0,(LPTHREAD_START_ROUTINE)modbus_x_process, __hwndMain, 0, &threadID);
	
	if ( hCommReceivedThread == NULL )
 	{
 		//return FALSE;
		::AfxMessageBox(__T("打开串口失败!!!"));
 	}
	//
	if(m_ComPortOpened){
		//KillTimer(1);
		//SetTimer(2,500,NULL);
	}
}

void CmodbusTestDlg::OnEnChangeEditcalibrationpoint()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
	
	// TODO:  在此添加控件通知处理程序代码

	
}
void CmodbusTestDlg::OnBnClickedButtonpreviouspoint()
{
	// TODO: 在此添加控件通知处理程序代码
	int point;
	CString str;
	m_calibration_point.GetWindowTextW(str);
	point=_tstoi(str.GetBuffer(str.GetLength())); 
	point--;
	if(point<0)point=29;
	str.Format(__T("%d"),point);
	m_calibration_point.SetWindowTextW(str);
	FlowClibPoint=point;

}


void CmodbusTestDlg::OnBnClickedButtonlastpoint()
{
	// TODO: 在此添加控件通知处理程序代码
	int point;
	CString str;
	m_calibration_point.GetWindowTextW(str);
	point=_tstoi(str.GetBuffer(str.GetLength())); 
	point++;
	if(point>29)point=0;
	str.Format(__T("%d"),point);
	m_calibration_point.SetWindowTextW(str);
	//FlowClibPoint=point;
}


void CmodbusTestDlg::OnEnChangeEditReferenceDeviceId()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString str;
	m_referenceDeviceId.GetWindowTextW(str);
	int t=	_tstoi(str.GetBuffer(str.GetLength())); 
	if(t<100)t=100;
	if(t>255)t=255;
	str.Format(__T("%03d"),t);
	if(m_list.GetItemCount()==0){
		m_list.InsertItem(0, __T("标准表"));
	}
	m_list.SetItemText(0,1,str);
	//
	st_modbusReadClib* stp;
	stp=(st_modbusReadClib*)readStardMeterModeBusCmd;
	stp->addr=t;
	stp->addr_hi=0x40;
	stp->addr_lo=0x03;
	stp->len_hi=0;
	stp->len_lo=2;
	stp->func=FUNC_READ_HOLDING_REGISTERS;
	crc_append(readStardMeterModeBusCmd,sizeof(st_modbusReadClib)-2);
}



void CmodbusTestDlg::OnBnClickedButtonComSendTest()
{
	if(!m_ComPortOpened){
		MessageBox(__T("串口未打开！！"));
		return;
	}
	if(hModbusXprocessThread)SuspendThread(hModbusXprocessThread);
	modbusMachineStatus=MODBUS_MS_NONE;
	if(hModbusXprocessThread)ResumeThread(hModbusXprocessThread);
	while(modbusMachineStatus!=MODBUS_MS_NONE){
		Sleep(10);
	}
	modbusMachineStatus=MODBUS_MS_SEND_TEST;
}
void CmodbusTestDlg::OnBnClickedButtonPoolSalave()
{
	if(!m_ComPortOpened){
		MessageBox(__T("串口未打开！！"));
		return;
	}
	if(hModbusXprocessThread)SuspendThread(hModbusXprocessThread);
	modbusMachineStatus=MODBUS_MS_NONE;
	if(hModbusXprocessThread)ResumeThread(hModbusXprocessThread);
	while(modbusMachineStatus!=MODBUS_MS_NONE){
		Sleep(10);
	}
	//EnterCriticalSection(&cs);
	CString str,sl;
	volatile int t=0;
	modBusParam.poolExceptAddrNum=0;
	m_referenceDeviceId.GetWindowTextW(str);
	t=	_tstoi(str.GetBuffer(str.GetLength())); 
	t=t;
	if(t>253)t=253;
	if(t<1)t=1;
	str.Format(__T("%d"),t);
	m_referenceDeviceId.SetWindowTextW(str);
	//standDeviceId=(uint8_t)t;
	modBusParam.poolExceptAddrBuf[0]=(uint8_t)(t&0xffL);
	modBusParam.poolExceptAddrNum++;
	//初始化列表框
	m_list.DeleteAllItems();
	str.Format(__T("%03d"),t);
	if(m_list.GetItemCount()==0){
		m_list.InsertItem(0, __T("标准表"));
	}
	m_list.SetItemText(0,1,str);
	//手动输入地址
	volatile int loc=0,i;
	loc=loc;
	m_manulInputAddr.GetWindowTextW(str);
	while(str.GetLength()>0){
		
		loc=str.Find(__T(' '));
		if(loc<str.GetLength() && loc>=0){
			if(loc==0){	if(str.GetLength()>0)str=str.Right(str.GetLength()-1);	continue;}
			if(loc>3)loc=3;
			sl=str.Left(loc);
			//str=__T("");
		}else{
			sl=str;
			str=__T("");
		}
		t=	_tstoi(sl.GetBuffer(sl.GetLength()));
		str=str.Right(str.GetLength()-loc);
		if(t>255)t=255;
		for(i=0;i<modBusParam.poolExceptAddrNum;i++){if(t==modBusParam.poolExceptAddrBuf[i])break;}
		if(i==modBusParam.poolExceptAddrNum){
			modBusParam.poolExceptAddrBuf[modBusParam.poolExceptAddrNum]=(uint8_t)t;
			modBusParam.poolExceptAddrNum++;
			}
	}
	modbusMachineStatus=MODBUS_MS_POLL_ID;
	//LeaveCriticalSection(&cs);
}

void CmodbusTestDlg::OnBnClickedButtonSetZero()
{
	// TODO: 在此添加控件通知处理程序代码

}

void CmodbusTestDlg::OnCbnSelchangeComboComPort()
{
	// TODO: 在此添加控件通知处理程序代码
	//enum_com_port();
}


void CmodbusTestDlg::OnDropdownComboComPort()
{
	// TODO: 在此添加控件通知处理程序代码
	enum_com_port();
}

//uint8_t setPoint=255;
void CmodbusTestDlg::get_full()
{
	CString str=__T("");
	m_editFullFlow.GetWindowText(str);
	modBusParam.fullFlow=_tstof(str.GetBuffer(str.GetLength()));
	if(modBusParam.fullFlow<10.0)modBusParam.fullFlow=10.0;
	if(modBusParam.fullFlow>500.0)modBusParam.fullFlow=500.0;
	str.Format(__T("%.1f"),modBusParam.fullFlow);
	m_editFullFlow.SetWindowTextW(str);

	m_editOutZero.GetWindowText(str);
	modBusParam.outMvZero=_tstof(str.GetBuffer(str.GetLength()));
	if(modBusParam.outMvZero<50.0)modBusParam.outMvZero=50.0;
	if(modBusParam.outMvZero>1000.0)modBusParam.outMvZero=1000.0;
	str.Format(__T("%.1f"),modBusParam.outMvZero);
	m_editOutZero.SetWindowTextW(str);

	m_editOutFull.GetWindowText(str);
	modBusParam.outMvFull=_tstof(str.GetBuffer(str.GetLength()));
	if(modBusParam.outMvFull<2000.0)modBusParam.outMvFull=2000.0;
	if(modBusParam.outMvFull>6000.0)modBusParam.outMvFull=6000.0;
	str.Format(__T("%.1f"),modBusParam.outMvFull);
	m_editOutFull.SetWindowTextW(str);
}
void CmodbusTestDlg::OnBnClickedButtonSetPoint()
{
	int count,i;
	if(!m_ComPortOpened){
		MessageBox(__T("串口未打开！！"));
		return;
	}	
	if(hModbusXprocessThread)SuspendThread(hModbusXprocessThread);
	modbusMachineStatus=MODBUS_MS_NONE;
	if(hModbusXprocessThread)ResumeThread(hModbusXprocessThread);
	while(modbusMachineStatus!=MODBUS_MS_NONE){
		Sleep(10);
	}
	CString str=__T("");
	//获取量程设置
	get_full();
#if(0)
	m_editFullFlow.GetWindowText(str);
	modBusParam.fullFlow=_tstof(str.GetBuffer(str.GetLength()));
	if(modBusParam.fullFlow<10.0)modBusParam.fullFlow=10.0;
	if(modBusParam.fullFlow>500.0)modBusParam.fullFlow=500.0;
	str.Format(__T("%.1f"),modBusParam.fullFlow);
	m_editFullFlow.SetWindowTextW(str);

	m_editOutZero.GetWindowText(str);
	modBusParam.outMvZero=_tstof(str.GetBuffer(str.GetLength()));
	if(modBusParam.outMvZero<50.0)modBusParam.outMvZero=50.0;
	if(modBusParam.outMvZero>1000.0)modBusParam.outMvZero=1000.0;
	str.Format(__T("%.1f"),modBusParam.outMvZero);
	m_editOutZero.SetWindowTextW(str);

	m_editOutFull.GetWindowText(str);
	modBusParam.outMvFull=_tstof(str.GetBuffer(str.GetLength()));
	if(modBusParam.outMvFull<2000.0)modBusParam.outMvFull=2000.0;
	if(modBusParam.outMvFull>6000.0)modBusParam.outMvFull=6000.0;
	str.Format(__T("%.1f"),modBusParam.outMvFull);
	m_editOutFull.SetWindowTextW(str);
#endif

	//获取标准输出电压，从标准表获取，或者从输入框获取
	LVITEM lvi;
	TCHAR szBuffer[32]=TEXT("");
	//https://zhidao.baidu.com/question/208693978.html
	GetDlgItem(IDC_EDIT_MV_INPUT)->GetWindowText(str);
	if(str!=__T("")){
		modBusParam.curStandardFlow=(_tstof(str.GetBuffer(str.GetLength())));
	}else{
		//从列表框中获取
		lvi.iItem =0;
		lvi.mask = LVIF_TEXT;
		lvi.pszText=(LPWSTR)szBuffer;
		lvi.cchTextMax = 32;
		lvi.iSubItem=4;
		m_list.GetItem(&lvi);
		str.Format(__T("%s"),szBuffer);		
		if(str.GetLength()==0){
			::AfxMessageBox(__T("标准表未读取流量，不能标定！！！"));
			return;			
		}else{
			modBusParam.curStandardFlow=(_tstof(str.GetBuffer(str.GetLength())));		
		}
	}
	if(modBusParam.curStandardFlow<0)modBusParam.curStandardFlow=0;
	if(modBusParam.curStandardFlow>modBusParam.fullFlow)modBusParam.curStandardFlow=modBusParam.fullFlow;
	//计算成电压
	modBusParam.curStandardMv=(modBusParam.outMvFull-modBusParam.outMvZero)*modBusParam.curStandardFlow/modBusParam.fullFlow+modBusParam.outMvZero;
	GetDlgItem(IDC_EDIT_MV_INPUT)->SetWindowText(__T(""));

	if(modBusParam.curStandardMv<0){
		::AfxMessageBox(__T("标定值错误，不能标定！！！"));
		return;			
	}
	//获取点位
	m_calibration_point.GetWindowTextW(str);
	modBusParam.curSetPoint=_tstoi(str.GetBuffer(str.GetLength())); 
	if(modBusParam.curSetPoint<0 || modBusParam.curSetPoint>29){
		::AfxMessageBox(__T("标定点错误！！！"));
		return;		
	}
	//设置前一个标定点
	//m_proSetedPoint.GetWindowTextW(str);
	str.Format(__T("%d,%dmv,%.1fl/min"),modBusParam.curSetPoint,modBusParam.curStandardMv,modBusParam.curStandardFlow);
	m_proSetedPoint.SetWindowTextW(str);
	//获取是否覆盖
	modBusParam.setFiCover=m_check_cover.GetCheck();
	m_check_cover.SetCheck(0);
	//清除通讯状态
	count=m_list.GetItemCount();		
	for(i=0;i<count;i++){
		str=__T("");
		m_list.SetItemText(i,2,str);
	}	
	modbusMachineStatus=MODBUS_MS_SET_POINT;
	OnBnClickedButtonlastpoint();
}

void CmodbusTestDlg::OnBnClickedButtonStopPoll()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD dw=0;
	if((void*)hThreadModbusRead){

		//TerminateThread(hThreadModbusRead,dw);
		SuspendThread(hThreadModbusRead);
	}
}


volatile modbusMachineStatus_t modbusMachineStatus=MODBUS_MS_NONE;
HANDLE hModbusXprocessThread=NULL;
bool m_ComPortOpened=false;
HANDLE hCom=NULL;



volatile stModBusParam_t modBusParam={0};
void CmodbusTestDlg::OnBnClickedButtonStartRead()
{
	int rowCount;
	int i;
	if(!m_ComPortOpened){
		MessageBox(__T("串口未打开！！"));
		return;
	}
	if(hModbusXprocessThread)SuspendThread(hModbusXprocessThread);
	modbusMachineStatus=MODBUS_MS_NONE;
	if(hModbusXprocessThread)ResumeThread(hModbusXprocessThread);
	while(modbusMachineStatus!=MODBUS_MS_NONE){
		Sleep(10);
	}
	//获取列表框内的id列表
	modBusParam.readReadAddrNum=0;
	TCHAR szBuffer[32]=TEXT("");
	CString str=__T("");	
	rowCount=m_list.GetItemCount();
	LVITEM lvi;
	for(i=0;i<rowCount;i++){
		lvi.iItem =i;
		lvi.mask = LVIF_TEXT;
		lvi.pszText=(LPWSTR)szBuffer;
		lvi.cchTextMax = 32;
		lvi.iSubItem=1;
		m_list.GetItem(&lvi);
		str.Format(__T("%s"),szBuffer);

		modBusParam.readReadAddrBuf[modBusParam.readReadAddrNum]=(uint8_t)(_tstoi(str.GetBuffer(str.GetLength()))); 
		modBusParam.readReadAddrNum++;	
	}
	modbusMachineStatus=MODBUS_MS_READ_VO;
}	


//uint8_t idSetValue=1;
void CmodbusTestDlg::OnBnClickedBtnSetId()
{
	if(!m_ComPortOpened){
		MessageBox(__T("串口未打开！！"));
		return;
	}	
	if(hModbusXprocessThread)SuspendThread(hModbusXprocessThread);
	modbusMachineStatus=MODBUS_MS_NONE;
	if(hModbusXprocessThread)ResumeThread(hModbusXprocessThread);
	while(modbusMachineStatus!=MODBUS_MS_NONE){
		Sleep(10);
	}
	//get id
	CString str;
	volatile int t=0;
	//idSetValue=7;
	modBusParam.idSetIdValue=7;
	m_idSetValue.GetWindowTextW(str);
	if(str.GetLength()==0){
		MessageBox(__T("请输入正确的id值！！"));
		return;
	}
	t=	_tstoi(str.GetBuffer(str.GetLength()));
	if(t>255)t=255;
	if(t<1)t=1;
	modBusParam.idSetIdValue=(uint8_t)t;
	m_idSetValue.SetWindowTextW(__T(""));
	modbusMachineStatus=MODBUS_MS_SET_ID;
}

uint16_t __x_com_send_len( uint8_t* buf,uint16_t len )
{
	DWORD wdPacket = (DWORD)len;
	DWORD dwdWriten = 0;

	COMSTAT ComStat;
	DWORD dwErrorFlags;
	ClearCommError(hCom, &dwErrorFlags, &ComStat);

	OVERLAPPED m_osWrite;
	memset(&m_osWrite,0,sizeof(OVERLAPPED));
	m_osWrite.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	//PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);//在读写串口之前清空缓冲区
	BOOL bResult = WriteFile( hCom, buf, wdPacket, &dwdWriten, &m_osWrite );

	if(!bResult)
	{
		if(GetLastError()==ERROR_IO_PENDING)//the com is writing!
		{
		
			DWORD dwdResult = WaitForSingleObject(m_osWrite.hEvent,1000);
			//PurgeComm(hCom, PURGE_TXABORT| PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
			PurgeComm(hCom, PURGE_TXABORT| PURGE_TXCLEAR);
			if ( dwdResult == WAIT_TIMEOUT )
			{
				TRACE(_T("Write Failed!"));
			}
			else
			{
				TRACE(_T("Write Success!\n"));
			}
		}
	}
	return (uint16_t)dwdWriten;
}

void  mb_send_test(void)
{
	uint8_t buf[32];//={0x01, 0x03, 0x40 ,0x01 ,0x00 ,0x01 ,0xC0 ,0x0A };
	if(!m_ComPortOpened){
		AfxMessageBox(__T("串口未打开！！"),MB_OK,0);
		//return 0;
	}
	st_modbusReadClib* stb;
	stb=(st_modbusReadClib*)buf;
	stb->addr=0xff;
	stb->addr_hi=0x00;
	stb->addr_lo=0x01;
	stb->func=0x03;
	stb->len_hi=0x00;
	stb->len_lo=0x01;
	crc_append(buf,sizeof(st_modbusReadClib)-2);
	__x_com_send_len(buf,sizeof(st_modbusReadClib));
	::SendMessage(__hwndMain, WM_COM_SENDED,sizeof(st_modbusReadClib), (LPARAM)buf );

}



void mb_send_poll_id(void)
{
	uint8_t buf[64];
	st_modbusReadClib* stb;
	stb=(st_modbusReadClib*)buf;
	int i;
	for(i=0;i<16;i++){
		modBusParam.poolExceptAddrBuf[i+modBusParam.poolExceptAddrNum]=i+1;
	}
	modBusParam.poolExceptAddrNum+=16;
	for(i=0;i<modBusParam.poolExceptAddrNum;i++){
		if(modbusMachineStatus!=MODBUS_MS_POLL_ID)break;
		stb->addr=modBusParam.poolExceptAddrBuf[i];
		stb->addr_hi=0x00;
		stb->addr_lo=0x01;

		stb->len_hi=0x00;
		stb->len_lo=0x01;

		stb->func=0x03;
		crc_append(buf,sizeof(st_modbusReadClib)-2);
		__x_com_send_len(buf,sizeof(st_modbusReadClib));
		::SendMessage(__hwndMain, WM_COM_SENDED,sizeof(st_modbusReadClib), (LPARAM)buf );
		Sleep(500);
	}
	//modbusMachineStatus=MODBUS_MS_NONE;
}

void mb_send_read_vo(void)
{
	uint8_t buf[64];
	st_modbusReadClib* stb;
	stb=(st_modbusReadClib*)buf;
	int i;
	for(i=0;i<modBusParam.readReadAddrNum;i++){
		if(modbusMachineStatus!=MODBUS_MS_READ_VO)break;
		stb->addr=modBusParam.readReadAddrBuf[i];
		stb->func=0x03;
		stb->addr_hi=0x00;
		stb->addr_lo=0x0d;

		stb->len_hi=0x00;
		stb->len_lo=0x01;
		crc_append(buf,sizeof(st_modbusReadClib)-2);
		__x_com_send_len(buf,sizeof(st_modbusReadClib));
		::SendMessage(__hwndMain, WM_COM_SENDED,sizeof(st_modbusReadClib), (LPARAM)buf );
		Sleep(500);
	}		
	//modbusMachineStatus=MODBUS_MS_NONE;
}

void mb_send_set_id(void)
{
	st_modbusWriteSingleClib* stb;
	uint8_t buf[32];
	stb=(st_modbusWriteSingleClib*)buf;
	stb->addr=0;
	stb->addr_hi=0;
	stb->addr_lo=0x02;
	stb->func=FUNC_WRITE_SINGLE_REGISTER;
	stb->data_hi=0;
	stb->data_lo=modBusParam.idSetIdValue;
	crc_append(buf,sizeof(st_modbusWriteSingleClib)-2);
	__x_com_send_len(buf,sizeof(st_modbusReadClib));
	::SendMessage(__hwndMain, WM_COM_SENDED,sizeof(st_modbusReadClib), (LPARAM)buf );
	Sleep(500);
}

void mb_send_get_pid(void)
{
	st_modbusReadClib* stb;
	uint8_t buf[32];
	stb=(st_modbusReadClib*)buf;
	stb->addr=modBusParam.pidDeviceId;
	stb->addr_hi=0;
	stb->addr_lo=0x04;

	stb->len_hi=0;
	stb->len_lo=3;

	stb->func=FUNC_READ_HOLDING_REGISTERS;

	crc_append(buf,sizeof(st_modbusReadClib)-2);
	__x_com_send_len(buf,sizeof(st_modbusReadClib));
	::SendMessage(__hwndMain, WM_COM_SENDED,sizeof(st_modbusReadClib), (LPARAM)buf );
	Sleep(2000);
}

void mb_send_set_pid(void)
{
	uint8_t buf[64];
	st_modbusWriteMultReg_t* stb;
	stb=(st_modbusWriteMultReg_t*)buf;
	stb->addr=modBusParam.pidDeviceId;
	stb->func=FUNC_WRITE_MULTIPLE_REGISTERS;
	stb->addr_hi=0;
	stb->addr_lo=4;
	stb->len_hi=0;
	stb->len_lo=3;
	stb->data[0]=(uint8_t)(modBusParam.pidP>>8);
	stb->data[1]=(uint8_t)(modBusParam.pidP&0xff);

	stb->data[2]=(uint8_t)(modBusParam.pidI>>8);
	stb->data[3]=(uint8_t)(modBusParam.pidI&0xff);

	stb->data[4]=(uint8_t)(modBusParam.pidD>>8);
	stb->data[5]=(uint8_t)(modBusParam.pidD&0xff);

	crc_append(buf,sizeof(st_modbusWriteMultReg_t)+6-2);
	__x_com_send_len(buf,sizeof(st_modbusWriteMultReg_t)+6);
	::SendMessage(__hwndMain, WM_COM_SENDED,sizeof(st_modbusWriteMultReg_t)+6, (LPARAM)buf );
	Sleep(2000);
}

void mb_send_set_point(void)
{
	int i;
	uint8_t buf[32]={0};
	st_modbusWriteSingleClib* stb;
	stb=(st_modbusWriteSingleClib*)buf;
	stb->addr=0;

	for(i=0;i<modBusParam.readReadAddrNum;i++){
		if(modBusParam.readReadAddrBuf[i]==253)continue;	//
		stb->addr=modBusParam.readReadAddrBuf[i];

		stb->func=FUNC_WRITE_SINGLE_REGISTER;
		if(modBusParam.setFiCover){
			stb->func=FUNC_WRITE_SINGLE_REGISTER_EX;
		}
		stb->addr_hi=0x00;
		stb->addr_lo=(modBusParam.curSetPoint)*2+0x10+1;
		stb->addr_hi=0x00;

		stb->data_hi=(modBusParam.curStandardMv>>8);
		stb->data_lo=(modBusParam.curStandardMv & 0xff);

		crc_append(buf,sizeof(st_modbusWriteSingleClib)-2);
		__x_com_send_len(buf,sizeof(st_modbusReadClib));
		::SendMessage(__hwndMain, WM_COM_SENDED,sizeof(st_modbusReadClib), (LPARAM)buf );
		Sleep(500);
	}
	Sleep(2000);
	//Sleep(1000);
}

DWORD WINAPI modbus_x_process(LPVOID pvVoid)
{
	while(1){
		switch(modbusMachineStatus){
			case MODBUS_MS_NONE:break;
			case MODBUS_MS_SEND_TEST:
				mb_send_test();
				modbusMachineStatus=MODBUS_MS_NONE;
				break;
			case MODBUS_MS_POLL_ID:
				mb_send_poll_id();
				modbusMachineStatus=MODBUS_MS_NONE;
				break;
			case MODBUS_MS_READ_VO:
				mb_send_read_vo();
				break;
			case MODBUS_MS_TASK_REQ:
				modbusMachineStatus=MODBUS_MS_NONE;
				Sleep(10);
				break;
			case MODBUS_MS_SET_ID:
				mb_send_set_id();
				modbusMachineStatus=MODBUS_MS_NONE;
				break;
			case MODBUS_MS_SET_POINT:
				mb_send_set_point();
				modbusMachineStatus=MODBUS_MS_READ_VO;
				break;
			case MODBUS_MS_GET_PID:

				mb_send_get_pid();

				modbusMachineStatus=MODBUS_MS_NONE;
				break;
			case MODBUS_MS_SET_PID:
				mb_send_set_pid();
				modbusMachineStatus=MODBUS_MS_NONE;
				break;
		}
		//modbusMachineStatus=MODBUS_MS_NONE;
		Sleep(50);
	}
}	

void CmodbusTestDlg::OnEnChangeEditInputMv()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CmodbusTestDlg::OnBnClickedCheckModifySetEn()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_modSetEn.GetCheck()){
		m_editFullFlow.EnableWindow(true);
		m_editOutZero.EnableWindow(true);
		m_editOutFull.EnableWindow(true);
	}else{
		m_editFullFlow.EnableWindow(false);
		m_editOutZero.EnableWindow(false);
		m_editOutFull.EnableWindow(false);	
	}
}


void CmodbusTestDlg::OnBnClickedCheckPidOpEn()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_checkPidOpEn.GetCheck()){
		m_pidSetID.EnableWindow(true);
		m_pidSetP.EnableWindow(true);
		m_pidSetI.EnableWindow(true);
		m_pidSetD.EnableWindow(true);
		if(m_ComPortOpened)GetDlgItem(IDC_BTN_PID_RD)->EnableWindow(true);
		if(m_ComPortOpened)GetDlgItem(IDC_BTN_PID_WR)->EnableWindow(true);
	}else{
		m_pidSetID.EnableWindow(false);
		m_pidSetP.EnableWindow(false);
		m_pidSetI.EnableWindow(false);
		m_pidSetD.EnableWindow(false);	
		GetDlgItem(IDC_BTN_PID_RD)->EnableWindow(false);
		GetDlgItem(IDC_BTN_PID_WR)->EnableWindow(false);
	}
}


void CmodbusTestDlg::OnBnClickedBtnPidRd()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!m_ComPortOpened){
		MessageBox(__T("串口未打开！！"));
		return;
	}

	//获取设备id
	CString str=__T("");
	m_pidSetID.GetWindowTextW(str);
	if(str.GetLength()==0){
		MessageBox(__T("请输入正确的id值！！"));
		return;
	}
	int t=0;
	t=	_tstoi(str.GetBuffer(str.GetLength()));
	if(t>250)t=250;
	if(t<1)t=1;
	modBusParam.pidDeviceId=(uint8_t)t;
	//m_pidSetID.SetWindowTextW(__T(""));
	//modbusMachineStatus=MODBUS_MS_SET_ID;
	if(hModbusXprocessThread)SuspendThread(hModbusXprocessThread);
	modbusMachineStatus=MODBUS_MS_NONE;
	if(hModbusXprocessThread)ResumeThread(hModbusXprocessThread);
	while(modbusMachineStatus!=MODBUS_MS_NONE){
		Sleep(10);
	}
	modbusMachineStatus=MODBUS_MS_GET_PID;
}


void CmodbusTestDlg::OnBnClickedBtnPidWr()
{
	// TODO: 在此添加控件通知处理程序代码
	int id,p,i,d;
	if(!m_ComPortOpened){
		MessageBox(__T("串口未打开！！"));
		return;
	}

	//获取设备id
	CString str=__T("");
	m_pidSetID.GetWindowTextW(str);
	if(str.GetLength()==0){
		MessageBox(__T("请输入正确的id值！！"));
		return;
	}
	id=	_tstoi(str.GetBuffer(str.GetLength()));
	if(id>250)id=250;
	if(id<1)id=1;
	modBusParam.pidDeviceId=(uint8_t)id;
	//获取P值
	m_pidSetP.GetWindowTextW(str);
	if(str.GetLength()==0){
		MessageBox(__T("请输入正确的P值！！"));
		return;
	}
	p=	_tstoi(str.GetBuffer(str.GetLength()));
	if(p>5000)p=5000;
	if(p<20)p=20;
	modBusParam.pidP=(uint16_t)p;
	//获取I值
	m_pidSetI.GetWindowTextW(str);
	if(str.GetLength()==0){
		MessageBox(__T("请输入正确的I值！！"));
		return;
	}
	i=	_tstoi(str.GetBuffer(str.GetLength()));
	if(i>300)i=300;
	if(i<1)i=1;
	modBusParam.pidI=(uint16_t)i;
	//获取D值
	m_pidSetD.GetWindowTextW(str);
	if(str.GetLength()==0){
		MessageBox(__T("请输入正确的D值！！"));
		return;
	}
	d=	_tstoi(str.GetBuffer(str.GetLength()));
	if(d>300)d=300;
	if(d<1)i=d;
	modBusParam.pidD=(uint16_t)d;

	if(hModbusXprocessThread)SuspendThread(hModbusXprocessThread);
	modbusMachineStatus=MODBUS_MS_NONE;
	if(hModbusXprocessThread)ResumeThread(hModbusXprocessThread);
	while(modbusMachineStatus!=MODBUS_MS_NONE){
		Sleep(10);
	}

	modbusMachineStatus=MODBUS_MS_SET_PID;
	
}
