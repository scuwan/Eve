#pragma once
#include "IScheme.h"
#include "IWnd.h"
#include "RoleConfigure.h"
#include <QString>
#include <QTimer>
class Scheme_1 :
	public IScheme
{
	Q_OBJECT
public:
	Scheme_1(QString role="");
	~Scheme_1();
public:
	virtual void SafeExit();
	virtual void SafePause();
	virtual void Start();
	virtual void ReleaseControl();
	int GetLastError();
	virtual QString SchemeName();
private:
	enum {OK=-1,NOK=-2};
	virtual void run();
	//安全回空间站
	int safe_back_station(int s);
	//安全出站
	int safe_leave_station(int s);
	//wrap to 空间站
	int wrap_to_station(int s);
	int normal(int s);
	int return_drones_to_bay(int s);
	int lanch_drones(int s);
	bool is_drone_in_space();
	bool try_attach_window();
	//判断船在空间站内
	bool is_in_space();
	//判断船在空间站外
	bool is_out_space();
	//空间站
	bool find_station(int *pos=nullptr);
	//总揽
	bool find_overview(int *pos = nullptr);
	//查找环绕
	bool find_orbit(int *pos=nullptr);
	//Select item
	bool find_select_item(int *pos = nullptr);
	//鼠标左键点击
	void l_click(QPoint pt);
	void l_click(int x, int y);
	//鼠标右键点击
	void r_click(QPoint pt);
	void r_click(int x, int y);
	//重新刷怪定时器 5分钟
	void start_rerun_timer();
	//检查是否有泡泡
	bool check_bubble();
	//检查是否来红
	bool check_red();
	//检查低血
	bool check_hp();
	//检查Tethered
	bool check_tethered();
	//打开异常界面
	bool open_probe_scanner();
	//关闭异常界面
	bool close_probe_scanner();
	//判断异常界面是否开启
	bool is_opened_probe_scanner();
	//查找异常
	bool find_exception(int *pos = nullptr);
	//关闭弹窗
	void close_popup_window();
	//
	bool is_nothing_found();
	//检查友军
	bool check_green();
	//检查建筑
	bool check_building(int* pos=nullptr);
	//屏蔽异常
	void ignore_exception(QPoint pt);
	//
	bool switch_overview_page(QString page);
	//格式化输出
	void format_out_put(QString i);
	//检查状态改变
	bool check_stateMachine(int csm);
	//用户命令输入打印
	void print_state_machine(int cs,int s);
signals:
	void start();
private slots:
	void rerun_timeout();
private:
	int m_state=0;
	int m_stateMachine = 0;	/* 0-正常刷怪 1-意外情况之后回空间站 2-安全退出 3-暂停 4-释放控制权*/
	int m_normalState =-1;
	bool m_isRunning=false;
	IWnd * m_wnd =nullptr;
	int m_errorCode =0;
	RoleConfigure m_configure;
	QTimer *m_timer=nullptr;
};

