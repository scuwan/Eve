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
	virtual void WindowPositionDetection();
private:
	enum {OK=-1,NOK=-2,RED=-3,LOW_HP=-4,RED_WITHSTAND=-5/*来红对抗*/,SHUT_DOWN=-6,WINDOW_CHECK_FAIL=-7/*窗口检查失败*/};
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
	//判断无人机是否在空间中
	bool is_drone_in_space();
	//判断无人机是否在船的过程中
	bool is_drone_backing();
	//闪黄判定,判定到无人机绿色并且有怪断定闪黄
	bool is_flash_yellow();
	//绑定窗口
	bool try_attach_window();
	//判断船在空间站内
	bool is_in_space();
	//判断船在空间站外
	bool is_out_space();
	//Undock
	bool find_undock(int *pos = nullptr);
	bool find_local(int* pos = nullptr);
	//空间站
	bool find_station(int *pos=nullptr);
	//总揽
	bool find_overview(int *pos = nullptr);
	//查找环绕
	bool find_orbit(int *pos=nullptr);
	//检查锁定目标
	bool find_locktarget(int *pos = nullptr);
	bool find_lockedtarget(int *pos = nullptr);
	//Select item
	bool find_select_item(int *pos = nullptr);
	//Drones
	bool find_drones(int *pos = nullptr);
	//控制面板
	bool find_panel(int *pos = nullptr);
	//鼠标左键点击
	void l_click(QPoint pt);
	void l_click(int x, int y);
	//鼠标右键点击
	void r_click(QPoint pt);
	void r_click(int x, int y);
	//鼠标点击拖动
	void click_move();
	//重新刷怪定时器 n分钟
	void start_rerun_timer();
	//检查是否有泡泡
	bool check_bubble();
	//网子检查
	bool check_web();
	bool find_web_overview(int * pos);
	//扰断检查
	bool check_broken();
	bool find_broken_overview(int *pos);
	//检查是否来红
	bool check_red();
	//检查是否执行来红对抗,在空间页面
	bool check_red_withstand(int *pos=nullptr);
	//对抗逻辑
	int withstand(int s);
	//检查低血
	bool check_hp();
	//低血处理
	int low_hp_process(int s=6);
	//修理
	int try_repair(int s);
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
	void format_out_put(const QString& i,const QString& color="#ffffff");
	//检查状态改变
	bool check_stateMachine(int csm);
	//用户命令输入打印
	void print_state_machine(int cs,int s);
	//判断是否是蛋
	bool is_an_egg();
	//刷怪前检测窗口位置是否发生变化并更新
	bool check_sub_windows();
signals:
	void start();
	void stop_timer();
	void window_detection();
private slots:
	void rerun_timeout();
	void stop_timer_slot();
	void window_dection_slot();
private:
	int m_state=0;
	int m_stateMachine = 0;	/* 0-正常刷怪 1-意外情况之后回空间站 2-安全退出 3-暂停 4-释放控制权 5-窗口检测*/
	int m_normalState =-1;
	bool m_isRunning=false;
	IWnd * m_wnd =nullptr;
	int m_errorCode =0;
	RoleConfigure m_configure;
	QTimer *m_timer=nullptr;
	QVector<quint64> m_redtime;	//来红的时间
	QMutex m_tMutex;
};

