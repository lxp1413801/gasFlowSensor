
// modbusTestDlg.h : 头文件
//
#include <stdint.h>

#pragma once

#define WM_COM_RECEIVED	WM_USER + 1122
#define WM_COM_SENDED	WM_USER + 1123
#define WM_COM_MODBUS_WRITED_OVER WM_USER + 1124
#define RECEVIED_BUFFER_LEN 1024

extern DWORD WINAPI ThreadProcReceived(LPVOID lpParameter );

typedef enum{
	MODBUS_MS_NONE,
	MODBUS_MS_TASK_REQ,
	MODBUS_MS_SEND_TEST,
	MODBUS_MS_POLL_ID,
	MODBUS_MS_READ_VO,
	MODBUS_MS_SET_ID,
	MODBUS_MS_SET_ZERO,
	MODBUS_MS_SET_POINT,
	MODBUS_MS_GET_PID,
	MODBUS_MS_SET_PID,
}modbusMachineStatus_t;

extern CRITICAL_SECTION cs;

//extern volatile uint8_t realAddrBuf[32];
typedef  uint16_t modbusMvBuf[32] ;
typedef struct{
	uint8_t idSetIdValue;
	uint16_t poolExceptAddrNum;
	uint8_t poolExceptAddrBuf[256];
	uint16_t readReadAddrNum;
	uint8_t readReadAddrBuf[256];

	float curStandardFlow;
	uint16_t curStandardMv;
	uint8_t curSetPoint;
	bool setFiCover;

	float lastStandardFlow;
	uint16_t lastStandardMv;
	int8_t lastSetPoint;
	
	float fullFlow;
	float outMvZero;
	float outMvFull;
	modbusMvBuf mvBuf[256];
	//pid参数部分
	uint16_t pidP;
	uint16_t pidI;
	uint16_t pidD;
	uint8_t pidDeviceId;

}stModBusParam_t;
extern volatile stModBusParam_t modBusParam;
//extern uint8_t idSetValue;
//extern int8_t standDeviceId;
extern volatile modbusMachineStatus_t modbusMachineStatus;
extern DWORD WINAPI modbus_x_process(LPVOID pvVoid);
extern HANDLE hModbusXprocessThread;

extern bool m_ComPortOpened;
extern HANDLE hCom;
// CmodbusTestDlg 对话框
class CmodbusTestDlg : public CDialogEx
{
// 构造
public:
	CmodbusTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MODBUSTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	int m_comb_init(void);
	int m_button_init(void);
	int m_dlg_init_call_back(void);
	afx_msg void OnBnClickedButtonOpenCom();
	//HANDLE hCom;
	//HANDLE hCommThread;
	//bool m_ComPortOpened;
	bool m_com_open(void);
	void m_com_close(void);
	afx_msg void OnBnClickedButtonCloseCom();
	afx_msg void OnBnClickedButtonComSendTest();

	uint16_t m_com_send_byte(uint8_t b);
	uint16_t m_com_send_len( uint8_t* buf,uint16_t len );
	void m_com_send_str( uint8_t* str );
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//
	uint8_t receivedBuf[RECEVIED_BUFFER_LEN];
	uint16_t receivedBufCount;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CEdit m_calibration_point;
	CListCtrl m_list;
	//int m_com_received_echo(uint8_t* rbuf, uint16_t len);
	CEdit m_dmsg;
	afx_msg void OnBnClickedButtonpreviouspoint();
	afx_msg void OnBnClickedButtonlastpoint();
	CEdit m_referenceDeviceId;
	afx_msg void OnEnChangeEditReferenceDeviceId();
	//afx_msg void OnBnClickedButtonSetZero2();
	int modbus_pool_slave(void);
	uint8_t modBusAddrArr[256];
	uint16_t modBusAddrCount;
	//bool uartRreceivedFlg;
	void set_uartRreceivedFlg(bool b){uartRreceivedFlg=b;}
	bool uartRreceivedFlg;
	afx_msg void OnBnClickedButtonPoolSalave();
	afx_msg void OnBnClickedButtonSetZero();
	int m_modebus_received_process(uint8_t* buf, uint16_t len);
	int enum_com_port(void);
	afx_msg void OnCbnSelchangeComboComPort();
	afx_msg void OnDropdownComboComPort();
	CListBox m_listImdMsg;
	int m_com_imd_msg(CString header , uint8_t* buf, uint16_t len);
	//afx_msg void OnBnClickedButtonStartPoll();
	//int m_modbusAddrCount;
	//uint8_t m_modbusAddrBuf[32];
	afx_msg void OnBnClickedButtonStopPoll();
	afx_msg void OnBnClickedButtonSetPoint();
	afx_msg void OnEnChangeEditcalibrationpoint();
	void m_get_clib_point(void);
	void m_get_clib_standard_flow(void);
	int m_get_manual_input_modbus_addr(void);
	CEdit m_manulInputAddr;
	CButton m_check_cover;
	CEdit m_proSetedPoint;
	afx_msg void OnBnClickedButtonStartRead();
	afx_msg void OnBnClickedBtnSetId();
	CEdit m_idSetValue;
	afx_msg void OnEnChangeEditInputMv();
	CButton m_modSetEn;
	
	afx_msg void OnBnClickedCheckModifySetEn();
	//
	CEdit m_editFullFlow;

	CEdit m_editOutZero;
	CEdit m_editOutFull;

	CButton m_checkPidOpEn;//IDC_CHECK_PID_OP_EN
	CEdit m_pidSetID;//IDC_EDIT_PID_SET_ID
	CEdit m_pidSetP;
	CEdit m_pidSetI;
	CEdit m_pidSetD;
	afx_msg void OnBnClickedCheckPidOpEn();
	afx_msg void OnBnClickedBtnPidRd();
	afx_msg void OnBnClickedBtnPidWr();
	void get_full();
};
