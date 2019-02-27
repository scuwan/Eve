#include "Scheme_1.h"
#include "LwIWnd.h"

#include <QDebug>
#include <QPoint>
#include <Windows.h>
#include <QMutexLocker>
#include <QtGlobal>
#include <QSettings>
#include <vector>
using std::vector;

namespace
{
#define SECURITY_COLOR "#ff0000"
#define USER_INPUT_EVENT_COLOR "#00aa00"
	enum
	{ 
		idle = 0/*无状态*/,in_station/*空间站内*/,out_station/*空间站外*/,go_abnormal/*前往异常*/,\
		go_station/*回仓*/
	};
	const char errorCode[][100] =
	{
		/*0*/"ok.",\
		/*1*/"无法Attach窗口.",\
		/*2*/"船未能回空间站",\
		/*3*/"没有找到配置文件"\
		/*4*/"窗口不存在"
	};
	const QString state_char[5] =
	{
		QString::fromLocal8Bit("当前正常刷怪状态，"),
		QString::fromLocal8Bit("当前意外情况之后回空间站状态，"),
		QString::fromLocal8Bit("当前安全退出状态，"),
		QString::fromLocal8Bit("当前暂停状态，"),
		QString::fromLocal8Bit("当前释放控制权状态，")
	};
#define CHECK_STATEMACHINE_BREAK(state) if(!check_stateMachine(state))\
	{\
		break;\
	} 
#define CHECK_STATEMACHIN_RETURN(state) if(!check_stateMachine(state))\
	{\
		print_state_machine(state,m_stateMachine);\
		return m_stateMachine;\
	}
#define CHECK_RED_WITHSTAND if(check_red_withstand())\
	{\
		return RED_WITHSTAND;\
	}
#define DELAY_N_SECONDS_RETURN(n,s) if(1)\
	{\
		int nnn=n;\
		while(nnn>0)\
		{\
			if (check_red())\
			{\
				grab(m_wnd->GetWindowId(), GetRole());\
				format_out_put(QString::fromLocal8Bit("来红了!!!"),SECURITY_COLOR);\
				m_redtime.push_back(QDateTime::currentDateTime().toSecsSinceEpoch());\
				m_normalState = -1;\
				return RED;\
			}\
			if (check_hp())\
			{\
				grab(m_wnd->GetWindowId(), GetRole());\
				format_out_put(QString::fromLocal8Bit("低血了!!!"),SECURITY_COLOR);\
				m_normalState = -1;\
				return LOW_HP;\
			}\
			CHECK_STATEMACHIN_RETURN(s);\
			Sleep(1000);\
			--nnn;\
		}\
	}
#define DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(n,s) if(1)\
	{\
		int nnn=n;\
		while(nnn>0)\
		{\
			CHECK_STATEMACHIN_RETURN(s);\
			Sleep(1000);\
			--nnn;\
		}\
	}
#define DELAY_N_SECONDS_WITH_RED_WITHSTAND(n,s) if(1)\
	{\
		int nnn = n;\
		while(nnn>0)\
		{\
			CHECK_STATEMACHIN_RETURN(s);\
			CHECK_RED_WITHSTAND;\
			Sleep(1000);\
			--nnn;\
		}\
	}

	class LockWindow
	{
	public:
		LockWindow(IWnd* w)
		{
			if (ref_count.constFind(w) == ref_count.constEnd())
			{
				ref_count.insert(w, 0);
			}
			if (ref_count[w] == 0)
			{
				w->LockInput(2);
			}
			++ref_count[w];
			ww = w;
		}
		~LockWindow()
		{
			--ref_count[ww];
			if (ref_count[ww] == 0)
			{
				ww->LockInput(0);
			}
		}
	private:
		IWnd* ww=nullptr;
		static QMap<IWnd*,int> ref_count;
	};
	QMap<IWnd*, int> LockWindow::ref_count;
}

Scheme_1::Scheme_1(QString role)
	:IScheme(role)
{
	m_roleName = role;
	connect(this, SIGNAL(start()), this, SLOT(Run()));
	connect(this, SIGNAL(stop_timer()), this, SLOT(stop_timer_slot()));
	connect(this, SIGNAL(window_detection()), this, SLOT(window_dection_slot()));
}


Scheme_1::~Scheme_1()
{
	if (m_wnd != nullptr)
	{
		m_wnd->Detach();
		delete m_wnd;
		m_wnd = nullptr;
	}
}


void Scheme_1::SafeExit()
{
	emit stop_timer();
	if (m_stateMachine == -1)
	{
		emit eState(this, true, 0);
		emit quit(this);
	}
	else
		m_stateMachine = 2;
}

void Scheme_1::SafePause()
{
	emit stop_timer();
	if (m_stateMachine == -1)
	{
		emit eState(this, true, 1);
	}
	else
		m_stateMachine = 3;	//回空间站
}

void Scheme_1::Start()
{
	emit stop_timer();
	m_stateMachine = 0;
	emit start();
}

void Scheme_1::ReleaseControl()
{
	emit stop_timer();
	if (m_stateMachine == -1)
	{
		emit eState(this, true, 3);
	}
	else
		m_stateMachine = 4;
}

int Scheme_1::GetLastError()
{
	return 0;
}

QString Scheme_1::SchemeName()
{
	return QString("Scheme_1");
}
void Scheme_1::WindowPositionDetection()
{
	m_stateMachine = 5;
	emit window_detection();
}
/*
 *安全回站
 */
int Scheme_1::safe_back_station(int s)
{
	/*format_out_put(QString::fromLocal8Bit(" 执行回站操作."));*/
	click_move();
	int pos[2] = { 0,0 };
	if (is_in_space())	//空间站内
	{
		format_out_put(QString::fromLocal8Bit(" 成功回站"));
		return OK;
	}
	else if(is_out_space())	//空间站外
	{
		switch_overview_page("空间");
		//回收无人机
		int ret=return_drones_to_bay(s);
		if (ret == NOK)
		{
			format_out_put(QString::fromLocal8Bit(" 回收无人机失败."));
			if (!check_hp() && !check_red())	//如果没有红和低血,继续回收无人机
			{
				return NOK;	
			}
		}
		else if (ret == OK)
		{
			format_out_put(QString::fromLocal8Bit(" 回收无人机成功."));
		}
		else if(ret == RED_WITHSTAND)	//红已经到本地，对抗
		{
			return RED_WITHSTAND;
		}
		else
		{
			//print_state_machine(ret);
			return ret;
		}
		//进入空间页面
		switch_overview_page("空间");
		DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
		if (find_station(pos)) //空间站
		{
			l_click(pos[0], pos[1]);
			DELAY_N_SECONDS_WITH_RED_WITHSTAND(1, s);
			QPoint pt = m_configure.GetStationItemPos("Approach");
			//l_click(pt);
			//DELAY_N_SECONDS_WITH_RED_WITHSTAND(2, s);	//延时2s
			pt = m_configure.GetStationItemPos("Wrap to with 0 m");
			l_click(pt);
			/*DELAY_N_SECONDS_WITH_RED_WITHSTAND(5, s);
			QPoint pt1;
			pt1 = m_configure.GetInstrumentPanelPos("Equipment 4");
			l_click(pt1);
			format_out_put(QString::fromLocal8Bit(" 关闭推子"));*/
			int n = 60;
			while (n > 0)
			{
				DELAY_N_SECONDS_WITH_RED_WITHSTAND(2, s);	//延时2s
				if (check_tethered())
					break;
				--n;
			}
			DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(8, s);	//延时2s
			pt = m_configure.GetStationItemPos("Dock");
			switch_overview_page("空间");
			DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(2, s);
			l_click(pt);
			n = 10;
			while (n > 0)
			{
				DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(2, s);	//延时2s
				if (is_in_space())
				{
					return OK;
				}
				--n;
			}
		}
		else //找不到空间站
		{
			if (is_out_space())
			{
				format_out_put(QString::fromLocal8Bit(" 找不到空间站"));
				return NOK;
			}
			else
			{
				int n = 5;
				while (n > 0)
				{
					DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(2, s);	//延时2s
					if (is_in_space())
					{
						return OK;
					}
					--n;
				}
			}
		}
	}   
	else //正在回站(出站)的过程中
	{
		format_out_put(QString::fromLocal8Bit(" 正在回站，请等待..."));
		int n = 5;
		while (n > 0)
		{
			DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(2, s);	//延时2s
			if (is_in_space())
			{
				return OK;
			}
			--n;
		}
	}
	format_out_put(QString::fromLocal8Bit(" 回空间站失败."));
	return NOK;
}
/*
*安全出站
*/
int Scheme_1::safe_leave_station(int s)
{
	if (is_in_space())
	{
		if (check_red())
		{
			//grab(m_wnd->GetWindowId(), GetRole());
			m_redtime.push_back(QDateTime::currentDateTime().toSecsSinceEpoch());
			format_out_put(QString::fromLocal8Bit(" 来红，拒绝出站请求."));
			return RED;	//有红
		}
		QPoint pt = m_configure.GetUndockPos();
		l_click(pt);
		DELAY_N_SECONDS_RETURN(3, s);
		int n = 30;
		while(n>0)
		{
			if (is_out_space())
				break;
			if(is_in_space())
				l_click(pt);
			DELAY_N_SECONDS_RETURN(3, s);
			--n;
		}
		if (!is_in_space())
		{
			Sleep(500);
			if (!check_sub_windows())
			{
				return WINDOW_CHECK_FAIL;
			}
			//停船
			QPoint pt = m_configure.GetInstrumentPanelPos("Stop The Ship");
			l_click(pt);
			DELAY_N_SECONDS_RETURN(3, s);
			//打开武器装备
			pt = m_configure.GetInstrumentPanelPos("Equipment 1");
			l_click(pt);
			DELAY_N_SECONDS_RETURN(3, s);
			pt = m_configure.GetInstrumentPanelPos("Equipment 2");
			l_click(pt);
			DELAY_N_SECONDS_RETURN(3, s);
			pt = m_configure.GetInstrumentPanelPos("Equipment 3");
			l_click(pt);
			DELAY_N_SECONDS_RETURN(3, s);
			//bool ret=lanch_drones();
			return OK;	//成功出站
		}
		else
		{
			return NOK;	//出站操作异常
		}
	}
	else
		return OK;	//成功出站(已经在站外了)	
}

int Scheme_1::wrap_to_station(int s)
{
	int pos[2];
	//进入空间页面
	QPoint pt = m_configure.GetOverviewPos("空间");
	l_click(pt);
	Sleep(500);
	bool ret=find_station(pos);
	if (ret == false)
	{
		l_click(pt);
		DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);;
		ret = find_station(pos);
	}
	if (ret)
	{
		l_click(pos[0] + 2, pos[1] + 2);
		Sleep(500);
		pt = m_configure.GetStationItemPos("Approach");
		l_click(pt);
		Sleep(2000);
		pt = m_configure.GetStationItemPos("Wrap to with 0 m");
		l_click(pt);
		DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(10, s);
		int n = 30;
		while (n > 0)
		{
			if (check_tethered())
				return OK;
			else
			{
				--n;
				DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
			}
		}
		if (check_tethered())
			return OK;
		else
			return NOK;
	}
	else
	{
		format_out_put(QString::fromLocal8Bit(" 找不到空间站."));
		return NOK;
	}
}

int Scheme_1::normal(int s=0)
{
	QString info;
	int ret = NOK;
	int pos[2];
	switch (m_normalState)
	{
	case -1: // 第一次进入，判断是否在站内，不是回站
		if (m_shutdown <= QDateTime::currentDateTime())
		{
			return SHUT_DOWN;
		}
		format_out_put(QString::fromLocal8Bit(" 开始执行刷怪逻辑."));
		format_out_put(QString::fromLocal8Bit(" 执行刷怪前回站操作."));
		ret = safe_back_station(0);
		if (ret==OK)
		{
			format_out_put(QString::fromLocal8Bit(" 成功回站."));
			m_normalState = 0;
		}
		else if(ret==NOK)
		{
			format_out_put(QString::fromLocal8Bit(" 尝试回站异常."));
		}
		else
		{
			;// print_state_machine(ret);
		}
		break;
	case 0: // 在空间站内,出站
		if (is_in_space())
		{
			format_out_put(QString::fromLocal8Bit(" 执行刷怪出站操作."));
			int ret = safe_leave_station(s);
			if (ret == OK)
			{
				format_out_put(QString::fromLocal8Bit(" 成功出站，并开启模块."));
				if (is_an_egg())
				{
					format_out_put(QString::fromLocal8Bit(" 你是一只蛋，退出脚本."));
					m_stateMachine = 2;
					return OK;
				}
				ret = try_repair(s);
				if (RED == ret)
					return RED;
				m_normalState = 1;
			}
			else if (ret == NOK)	//异常
			{
				grab(m_wnd->GetWindowId(), GetRole());
				format_out_put(QString::fromLocal8Bit(" 出站操作异常，拒绝出站请求."));
				return NOK;
			}
			else if (ret == RED)	//来红
			{
				grab(m_wnd->GetWindowId(), GetRole());
				format_out_put(QString::fromLocal8Bit(" 出站操作异常，拒绝出站请求."));
				return RED;
			}
			else
				return ret;
		}
		else
		{
			format_out_put(QString::fromLocal8Bit(" 成功出站，并开启模块."));
			m_normalState = 1;
		}
		break;
	case 1:	// 站外，选取异常
	{
		if (!is_out_space())
		{
			m_normalState = 0;	//站内未出站
			break;
		}
		else
		{
			format_out_put(QString::fromLocal8Bit(" 选取异常."));
			ret = open_probe_scanner();
			if (!ret)
			{
				format_out_put(QString::fromLocal8Bit(" 打开异常面板失败."));
				break;
			}
			else
			{
				ret = find_exception(pos);
				//如果找不到异常，返回
				if (!ret)
				{
					close_probe_scanner();
					format_out_put(QString::fromLocal8Bit("找不到异常."));
					return NOK;
				}
				 //找到异常
				if(1)
				{
					QPoint currnt_exception_pt;
					if (1)
					{
						QMutexLocker locker(m_gmutexs[m_groupName].data());
						DELAY_N_SECONDS_RETURN(1, s);
						LockWindow t(m_wnd);
						format_out_put(QString::fromLocal8Bit(" 找到可刷异常,前往..."));
						QPoint currnt_exception_pt_a(pos[0] + 5, pos[1] + 2);
						currnt_exception_pt = currnt_exception_pt_a;
						QPoint o;
						o = m_configure.GetProbeScannerWraptoWithin_o("Wrap to Within");
						QPoint wrap_to_within(pos[0] + o.x(), pos[1] + o.y());
						o = m_configure.GetProbeScannerWraptoWithin_o(m_configure.GetWrapWithin());
						QPoint within_n_km(pos[0] + o.x(), pos[1] + o.y());
						r_click(pos[0] + 5, pos[1] + 2);
						Sleep(2000);
						m_wnd->MouseMoveTo(wrap_to_within.x(), wrap_to_within.y());
						Sleep(1000);
						l_click(within_n_km);
						Sleep(1000);
						m_wnd->MouseMoveTo(500, 10);
						close_popup_window();
						DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
						close_popup_window();
						DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
						/*close_probe_scanner();
						DELAY_N_SECONDS_RETURN(2, s);*/
						int n = 60;
						//下面的while循环判断是否有怪
						DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(8, s);
						format_out_put(QString::fromLocal8Bit(" 检查异常里是否有怪..."));
						while (n > 0)
						{
							switch_overview_page("刷怪");
							if (is_nothing_found())
							{
								DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);	//延时1s
								--n;
							}
							else
							{
								DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);	//延时1s再判一次
								if (is_nothing_found())
								{
									DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
									--n;
								}
								else
								{
									format_out_put(QString::fromLocal8Bit(" 检查到异常里有怪..."));
									break;
								}
							}

						}
						//异常中没有怪，wrap to station
						if (is_nothing_found())	//如果没有怪，则屏蔽异常
						{
							format_out_put(QString::fromLocal8Bit(" 异常中没有怪可刷，屏蔽异常, Wrap to空间站 0m. "));
							//屏蔽异常代码
							ignore_exception(currnt_exception_pt);
							DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
							close_probe_scanner();
							DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(2, s);
							DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(10, s);
							//wrap to 空间站
							int ret = wrap_to_station(s);
							if (OK == ret)
							{
								DELAY_N_SECONDS_RETURN(10, s);
								break;
							}
							else if (NOK == ret)
							{
								return NOK;
							}
							else
							{
								return ret;
							}
						}
						//有怪，检查友军，检查建筑物，检查红
						format_out_put(QString::fromLocal8Bit(" 异常中有怪可刷，检查友军..."));
						//检查友军，如果存在友军，wrap to station
						ret = check_green();
					}
					if (ret)
					{
						format_out_put(QString::fromLocal8Bit(" 存在友军，屏蔽异常,8s后 Wrap to 空间站 0m."));
						DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(8, s);
						//屏蔽异常代码
						ignore_exception(currnt_exception_pt);
						//关闭异常窗口
						close_probe_scanner();
						//wrap to 空间站
						int ret = wrap_to_station(s);
						if (OK == ret)
						{
							DELAY_N_SECONDS_RETURN(10, s);
							break;
						}
						else if (NOK == ret)
						{
							return NOK;
						}
						else
						{
							return ret;
						}
					}
					//没有友军
					close_probe_scanner();
					format_out_put(QString::fromLocal8Bit(" 没有友军，探测是否有可环绕的建筑物..."));
					CHECK_STATEMACHIN_RETURN(s);
					
					//检查建筑物，如果有环绕
					if (check_building(pos))
					{
						format_out_put(QString::fromLocal8Bit(" 探测到可环绕建筑物，并环绕..."));
						l_click(pos[0] + 10, pos[1] + 2);
						pos[0] = 0;
						pos[1] = 0;
						Sleep(500);
						int n = 20;
						while (n > 0)
						{
							if (find_orbit(pos))
							{
								l_click(pos[0] + 5, pos[1] + 2);
								format_out_put(QString::fromLocal8Bit(" 环绕建筑物成功.执行释放无人机操作..."));
								break;
							}
							else
							{
								DELAY_N_SECONDS_RETURN(1, s);
							}
						}
						QPoint pt;
						pt = m_configure.GetInstrumentPanelPos("Equipment 4");
						l_click(pt);
						int ret = lanch_drones(s);
						if (ret == OK)
						{
							DELAY_N_SECONDS_RETURN(1, s);
							format_out_put(QString::fromLocal8Bit(" 释放无人机成功，切换到刷怪页面."));
							switch_overview_page("刷怪");

						}
						else if (ret == NOK)
						{
							format_out_put(QString::fromLocal8Bit(" 释放无人机失败."));
							m_stateMachine = -1;	//刷怪异常操作，回站
							return NOK;
						}
						else
						{
							return ret;
						}

					}
					else //环绕第一个怪
					{
						format_out_put(QString::fromLocal8Bit(" 未探测到可环绕的建筑物，环绕第一个怪..."));
						switch_overview_page("刷怪");
						QPoint pt_one = m_configure.GetOverviewFirstItem();
						l_click(pt_one);
						int n = 5;
						bool ff = false;
						while (n > 0)
						{
							switch_overview_page("刷怪");
							if (find_orbit(pos))
							{
								l_click(pos[0] + 5, pos[1] + 2);
								format_out_put(QString::fromLocal8Bit(" 环绕第一个怪成功.执行释放无人机操作..."));
								ff = true;
								break;
							}
							else
								DELAY_N_SECONDS_RETURN(2, s);
							--n;
						}
						if (!ff)
						{
							format_out_put(QString::fromLocal8Bit("环绕怪失败..."));
							return NOK;
						}
						QPoint pt;
						pt = m_configure.GetInstrumentPanelPos("Equipment 4");
						l_click(pt);
						int ret = lanch_drones(s);
						if (OK == ret)	//释放无人机成功
						{
							/*QPoint pt;
							pt = m_configure.GetInstrumentPanelPos("Equipment 4");
							l_click(pt);*/
							DELAY_N_SECONDS_RETURN(1, s);
							format_out_put(QString::fromLocal8Bit(" 释放无人机成功.刷怪，准备环绕残骸..."));
							switch_overview_page("残骸");
							DELAY_N_SECONDS_RETURN(5, s);	//避免切换之后，界面延迟
							while (1)
							{
								switch_overview_page("残骸");
								DELAY_N_SECONDS_RETURN(1, s);
								if (!is_nothing_found())
								{
									l_click(pt_one);
									DELAY_N_SECONDS_RETURN(1, s);
									if (find_orbit(pos))
									{
										l_click(pos[0] + 5, pos[1] + 2);
										format_out_put(QString::fromLocal8Bit(" 环绕残骸成功,切换到刷怪界面."));
										m_normalState = 2;
										break;
									}
								}
								DELAY_N_SECONDS_RETURN(1, s);
							}
							switch_overview_page("刷怪");
						}
						else if (NOK == ret)
						{
							format_out_put(QString::fromLocal8Bit(" 释放无人机失败."));
							m_normalState = -1;	//刷怪异常，回站
							return NOK;
						}
						else
						{
							return ret;
						}
					}
					m_normalState = 2;
				}
			}
		}
		break;
	}
	case 2:
		static int count_t_check_guai = 0;
		static int count_t_flash_yellow = 0;
		//检查扰断
		if (check_broken())
		{
			format_out_put(QString::fromLocal8Bit("检查到扰断,打掉它"), "#55ffff");
			switch_overview_page("刷怪");
			DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
			int pos[2] = { 0,0 };
			bool ret = find_broken_overview(pos);
			if (ret)
			{
				l_click(pos[0], pos[1]);
				DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
				int n = 5;
				while (n > 0)
				{
					if (find_lockedtarget(pos))
					{
						l_click(100, 3);
						m_wnd->KeyPress('F');
						DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
						m_wnd->KeyPress('F');
						DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
						m_wnd->KeyPress('F');
						break;
					}
					else if (find_locktarget(pos))
					{
						l_click(pos[0] + 5, pos[1] + 2);
						DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(5, s);
						l_click(100, 3);
						m_wnd->KeyPress('F');
						DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
					}
					else
					{
						DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(2, s);
					}
					--n;
				}
			}
			break;
		}
		else
		{
			int pos[2] = { 0,0 };
			bool ret = find_broken_overview(pos);
			if (ret)
			{
				format_out_put(QString::fromLocal8Bit("检查到扰断,打掉它"), "#55ffff");
				l_click(pos[0], pos[1]);
				DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
				int n = 5;
				while (n > 0)
				{
					if (find_lockedtarget(pos))
					{
						l_click(100, 3);
						m_wnd->KeyPress('F');
						DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
						m_wnd->KeyPress('F');
						DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
						m_wnd->KeyPress('F');
						break;
					}
					else if (find_locktarget(pos))
					{
						l_click(pos[0] + 5, pos[1] + 2);
						DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(5, s);
						l_click(100, 3);
						m_wnd->KeyPress('F');
						DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
					}
					else
					{
						DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(2, s);
					}
					--n;
				}
				break;
			}
		}
		//网子检查
		if (check_web())
		{
			switch_overview_page("刷怪");
			DELAY_N_SECONDS_RETURN(1, s);
			int pos[2] = { 0,0 };
			bool ret = find_web_overview(pos);
			if (ret)
			{
				format_out_put(QString::fromLocal8Bit("检查到网子,打掉它"),"#55ffff");
				l_click(pos[0], pos[1]);
				DELAY_N_SECONDS_RETURN(1, s);
				int n = 5;
				while (n > 0)
				{
					if (find_lockedtarget(pos))
					{
						l_click(100, 3);
						m_wnd->KeyPress('F');
						DELAY_N_SECONDS_RETURN(1, s);
						m_wnd->KeyPress('F');
						DELAY_N_SECONDS_RETURN(1, s);
						m_wnd->KeyPress('F');
						break;
					}
					else if (find_locktarget(pos))
					{
						l_click(pos[0] + 5, pos[1] + 2);
						DELAY_N_SECONDS_RETURN(5, s);
						l_click(100, 3);
						m_wnd->KeyPress('F');
						DELAY_N_SECONDS_RETURN(1, s);
					}
					else
					{
						DELAY_N_SECONDS_RETURN(2, s);
					}
					--n;
				}
			}
			break;
		}
		else
		{
			int pos[2] = { 0,0 };
			bool ret = find_web_overview(pos);
			if (ret)
			{
				format_out_put(QString::fromLocal8Bit("检查到网子,打掉它"),"#55ffff");
				l_click(pos[0], pos[1]);
				DELAY_N_SECONDS_RETURN(1, s);
				int n = 5;
				while (n > 0)
				{
					if (find_lockedtarget(pos))
					{
						l_click(100, 3);
						m_wnd->KeyPress('F');
						DELAY_N_SECONDS_RETURN(1, s);
						m_wnd->KeyPress('F');
						DELAY_N_SECONDS_RETURN(1, s);
						m_wnd->KeyPress('F');
						break;
					}
					else if (find_locktarget(pos))
					{
						l_click(pos[0] + 5, pos[1] + 2);
						DELAY_N_SECONDS_RETURN(5, s);
						l_click(100, 3);
						m_wnd->KeyPress('F');
						DELAY_N_SECONDS_RETURN(1, s);
					}
					else
					{
						DELAY_N_SECONDS_RETURN(2, s);
					}
					--n;
				}
				break;
			}
		}
		//来红检查
		if (check_red())
		{
			grab(m_wnd->GetWindowId(), GetRole());
			format_out_put(QString::fromLocal8Bit("来红!!!"), SECURITY_COLOR);
			m_redtime.push_back(QDateTime::currentDateTime().toSecsSinceEpoch());
			m_normalState = -1;
			return RED;
		}
		//低血检查
		if (check_hp())	
		{
			grab(m_wnd->GetWindowId(), GetRole());
			format_out_put(QString::fromLocal8Bit("低血!!!"), SECURITY_COLOR);
			m_normalState = -1;
			return LOW_HP;
		}
		//检查怪是否刷完，每20s检测一次
		if (count_t_check_guai % 5 == 0)
		{
			LockWindow t(m_wnd);
			count_t_check_guai = 0;
			switch_overview_page("刷怪");
			DELAY_N_SECONDS_RETURN(1, s);
			if (is_nothing_found())
			{
				if (is_nothing_found())
				{
					//grab(m_wnd->GetWindowId(), GetRole());
					bool ret = return_drones_to_bay(s);
					DELAY_N_SECONDS_RETURN(3, s);

					if (ret&!is_drone_in_space())
					{
						if (m_shutdown > QDateTime::currentDateTime())
						{
							format_out_put(QString::fromLocal8Bit(" 异常中怪已经清理完毕!!! 跳下个异常"), "#0000ff");
							m_normalState = 1;
							return OK;
						}
						else
						{
							return SHUT_DOWN;
						}
					}
				}
			}	
		}
		//怪物锁定，闪黄
		if (count_t_flash_yellow % 2 == 0&&is_flash_yellow()&&!is_nothing_found())
		{
			count_t_flash_yellow = 0;
			switch_overview_page("刷怪");
			QPoint pt_one = m_configure.GetOverviewFirstItem();
			l_click(pt_one);
			int n = 5;
			while (n > 0)
			{
				if (find_locktarget(pos))
				{
					l_click(pos[0] + 5, pos[1] + 2);
					DELAY_N_SECONDS_RETURN(5,s);
					l_click(100, 3);
					m_wnd->KeyPress('F');
					DELAY_N_SECONDS_RETURN(1, s);
					if (!is_flash_yellow())
					{
						format_out_put(QString::fromLocal8Bit("锁怪成功."));
						break;
					}
				}
				else
				{
					if (is_flash_yellow())
					{
						l_click(100, 3);
						m_wnd->KeyPress('F');
						DELAY_N_SECONDS_RETURN(5, s);
					}
					else
					{
						format_out_put(QString::fromLocal8Bit("锁怪成功."));
						break;
					}
				}
				--n;
			}
			if (is_flash_yellow() && !is_nothing_found())
			{
				format_out_put(QString::fromLocal8Bit(" 检测到环绕的残骸消失，环绕新的残骸ing."));
				switch_overview_page("残骸");
				DELAY_N_SECONDS_RETURN(2, s);
				QPoint pt_one;
				pt_one = m_configure.GetOverviewFirstItem();
				l_click(pt_one);
				DELAY_N_SECONDS_RETURN(1, s);
				if (find_orbit(pos))
				{
					l_click(pos[0] + 5, pos[1] + 2);
					format_out_put(QString::fromLocal8Bit(" 环绕残骸成功,切换到刷怪界面."));
				}
			}
		}
		DELAY_N_SECONDS_RETURN(1, s);
		++count_t_check_guai;
		++count_t_flash_yellow;
		break;
	}
	return OK;
}


int Scheme_1::return_drones_to_bay(int s)
{
	LockWindow t(m_wnd);
	//如果是蛋直接返回成功
	if (is_an_egg())
		return OK;
	if (is_in_space())
	{
		return OK;	//操作执行成功
	}
	QPoint pt = m_configure.GetDronesPos("Base");
	int pos[2];
	bool ret = m_wnd->FindPicture(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Drones_in_Bay_2.bmp", pos, "000000", 0.8);
	if (ret)
	{
		//pt = m_configure.GetDronesPos("Drones in Bay");
		l_click(pos[0]+50,pos[1]+5);
		DELAY_N_SECONDS_WITH_RED_WITHSTAND(1, s);
	}
	ret = m_wnd->FindPicture(pt.x(),pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Drones_in_Space_1.bmp", pos, "000000", 0.8);
	if (ret)
	{
		//pt = m_configure.GetDronesPos("Drones in Bay");
		l_click(pos[0] + 50, pos[1] + 5);
		DELAY_N_SECONDS_WITH_RED_WITHSTAND(1, s);
	}
	if (is_drone_in_space())
	{
		//判断Drones in Bay是否展开，若展开就收起
		QPoint pt = m_configure.GetDronesPos("Base");
		bool ret = m_wnd->FindPicture(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Drones_in_Bay_2.bmp", nullptr, "000000", 0.8);
		if (ret)
		{
			pt = m_configure.GetDronesPos("Drones in Bay");
			l_click(pt);
			DELAY_N_SECONDS_WITH_RED_WITHSTAND(1, s);
		}
		pt = m_configure.GetDronesPos("Drones in Local Space");
		QPoint pt1 = m_configure.GetDronesLocalSpacePos("Return to Drone Bay");
		r_click(pt);
		Sleep(1000);
		l_click(pt1);
		if (find_station(pos)) //空间站
		{
			l_click(pos[0], pos[1]);
			DELAY_N_SECONDS_WITH_RED_WITHSTAND(1, s);
			QPoint pt2 = m_configure.GetStationItemPos("Approach");
			l_click(pt2);
		}
		DELAY_N_SECONDS_WITH_RED_WITHSTAND(5, s);
		QPoint pt3;
		pt3 = m_configure.GetInstrumentPanelPos("Equipment 4");
		l_click(pt3);
		format_out_put(QString::fromLocal8Bit(" 关闭推子"));
		int n = 20;
		while (n>0)
		{
			if (is_drone_in_space())
			{
				if (!is_drone_backing())
				{
					r_click(pt);
					DELAY_N_SECONDS_WITH_RED_WITHSTAND(1, s);
					l_click(pt1);
					//DELAY_N_SECONDS_WITH_RED_WITHSTAND(2, s);
				}
				else
					DELAY_N_SECONDS_WITH_RED_WITHSTAND(2, s);
				--n;
			}
			else
			{
				break;
			}
		}
		if (is_drone_in_space())
		{
			return NOK;
		}
		else
			return OK;
	}
	else
		return OK;
}

int Scheme_1::lanch_drones(int s)
{
	LockWindow t(m_wnd);
	if (is_in_space())
	{
		return false;	//在空间站内
	}
	QPoint pt = m_configure.GetDronesPos("Drones in Bay");
	QPoint pt_1 = m_configure.GetDroneBayPos("Lanch drones");
	r_click(pt);
	Sleep(1000);
	l_click(pt_1);
	DELAY_N_SECONDS_RETURN(4, s);
	int n = 10;
	while (n>0)
	{
		if (!is_drone_in_space())
		{
			--n;
			r_click(pt);
			Sleep(1000);
			l_click(pt_1);
			DELAY_N_SECONDS_RETURN(2, s);
		}
		else
		{
			return OK;
		}
	}
	return NOK;
}

bool Scheme_1::is_drone_in_space()
{
	QPoint pt;
	pt=m_configure.GetDronesPos();
	
	//判红,绿，黄
	/*std::string r = m_wnd->FindColorEx(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "F80000-000000|F40000-000000|00F400-000000|00F800-000000|F4F400-000000|FFFF00-000000|E1E101-000000|F5F500-000000",0.7);
	if (r.size() == 0)
	{
		return false;
	}
	else
		return true;*/
	bool ret = m_wnd->FindPicture(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Space_0.bmp", nullptr, "000000", 0.8);
	if (ret)
	{
		return false;
	}
	else
	{
		Sleep(500);
		bool ret = m_wnd->FindPicture(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Space_0.bmp", nullptr, "000000", 0.8);
		if (ret)
			return false;
		else
			return true;
	}
}

bool Scheme_1::is_drone_backing()
{
	QPoint pt;
	pt = m_configure.GetDronesPos();
	//黄
	vector<int> r = m_wnd->FindColorEx(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "F4F400-000000|FFFF00-000000|E1E101-000000|F5F500-000000");
	if (r.size() == 0)
		return false;
	else
		return true;
}

bool Scheme_1::is_flash_yellow()
{
	QPoint pt;
	pt = m_configure.GetDronesPos();
	//判绿
	vector<int> r = m_wnd->FindColorEx(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "00F400-000000|00F800-000000");
	if (r.size() == 0)
		return false;
	else
		return true;
}

bool Scheme_1::try_attach_window()
{
	if (m_wnd == nullptr)
		m_wnd = new LwIWnd;
	QString title("EVE - ");
	title += m_roleName;
	const wchar_t * wtitle = reinterpret_cast<const wchar_t *>(title.utf16());
	int repeat_time = 5;
	bool ret = false;
	while (repeat_time > 0)
	{
		HWND wid = FindWindow(__T("triuiScreen"), wtitle);
		if (!m_wnd->Attch((int)wid))
		{
			--repeat_time;
			Sleep(5000);	//5 秒attach一次
		}
		else
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool Scheme_1::is_in_space()
{
	bool ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Undock.bmp", nullptr, "000000", 0.7);
	return ret;
}

bool Scheme_1::is_out_space()
{
	return m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Overview.bmp", nullptr, "000000", 0.7);
}

bool Scheme_1::find_undock(int * pos)
{
	bool ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Undock.bmp", pos, "000000", 0.7);
	return ret;
}

bool Scheme_1::find_local(int * pos)
{
	bool ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Local.bmp", pos, "000000", 0.7);
	return ret;
}

bool Scheme_1::find_station(int * pos)
{
	QPoint pt_overview = m_configure.GetOverviewPos();
	//bool ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Station1.bmp", pos, "000000", 0.7);
	//if (ret)
	//{
	//	pos[0] += 10;
	//	pos[1] += 8;
	//	return ret;
	//}
	//ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Station2.bmp", pos, "000000", 0.7);
	//if (ret)
	//{
	//	pos[0] += 10;
	//	pos[1] += 8;
	//	return ret;
	//}
	//ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Station3.bmp", pos, "000000", 0.7);
	//if (ret)
	//{
	//	pos[0] += 10;
	//	pos[1] += 8;
	//	return ret;
	//}
	//ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Station4.bmp", pos, "000000", 0.7);
	//if (ret)
	//{
	//	pos[0] += 10;
	//	pos[1] += 8;
	//	return ret;
	//}
	//ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Station5.bmp", pos, "000000", 0.7);
	//if (ret)
	//{
	//	pos[0] += 10;
	//	pos[1] += 8;
	//	return ret;
	//}
	//ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Station6.bmp", pos, "000000", 0.7);
	//if (ret)
	//{
	//	pos[0] += 10;
	//	pos[1] += 8;
	//	return ret;
	//}
	//ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Station7.bmp", pos, "000000", 0.7);
	//if (ret)
	//{
	//	pos[0] += 10;
	//	pos[1] += 8;
	//	return ret;
	//}
	//ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Station8.bmp", pos, "000000", 0.7);
	//if (ret)
	//{
	//	pos[0] += 10;
	//	pos[1] += 8;
	//	return ret;
	//}
	//ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/StationNormal.bmp", pos, "000000", 0.7);
	vector<int> rr = m_wnd->FindPictureEx(pt_overview.x(), pt_overview.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Station1.bmp|pic/Station2.bmp|pic/Station3.bmp|pic/Station4.bmp|pic/Station5.bmp|pic/Station6.bmp|pic/Station7.bmp|pic/Station8.bmp", "000000", 0.7);
	if (rr.size() < 3)
	{
		return false;
	}
	else
	{
		if (pos != nullptr)
		{
			pos[0] = rr[1] + 10;
			pos[1] = rr[2] + 8;
		}
		return true;
	}
}

bool Scheme_1::find_overview(int * pos)
{
	return m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Overview.bmp", pos, "000000", 0.7);
}

bool Scheme_1::find_orbit(int *pos)
{
	QPoint pt_si= m_configure.GetSelectItemPos();
	bool ret = m_wnd->FindPicture(pt_si.x(), pt_si.y(), m_wnd->GetWindowRect().right, pt_si.y()+150, "pic/Orbit.bmp", pos, "000000",0.7);
	return ret;
}

bool Scheme_1::find_locktarget(int * pos)
{
	QPoint pt_si = m_configure.GetSelectItemPos();
	bool ret = m_wnd->FindPicture(pt_si.x(), pt_si.y(), m_wnd->GetWindowRect().right, pt_si.y() + 150, "pic/LockTarget.bmp", pos, "000000",0.7);
	return ret;
}

bool Scheme_1::find_lockedtarget(int * pos)
{
	QPoint pt_si = m_configure.GetSelectItemPos();
	bool ret = m_wnd->FindPicture(pt_si.x(), pt_si.y(), m_wnd->GetWindowRect().right, pt_si.y() + 150, "pic/LockedTarget.bmp", pos, "000000", 0.7);
	return ret;
}

bool Scheme_1::find_select_item(int * pos)
{
	return m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/select_item.bmp", pos, "000000", 0.7);
}

bool Scheme_1::find_drones(int * pos)
{
	return m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Drones.bmp", pos, "000000", 0.7);
}

bool Scheme_1::find_panel(int * pos)
{
	return m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/RightArrow.bmp", pos, "000000", 0.7);
}

void Scheme_1::l_click(QPoint pt)
{
	LockWindow t(m_wnd);
	m_wnd->MouseMoveTo(pt.x(), pt.y());
	Sleep(200);
	m_wnd->MouseLeftClicked();
}

void Scheme_1::l_click(int x, int y)
{
	LockWindow t(m_wnd);
	m_wnd->MouseMoveTo(x, y);
	Sleep(200);
	m_wnd->MouseLeftClicked();
	Sleep(200);
}

void Scheme_1::r_click(QPoint pt)
{
	LockWindow t(m_wnd);
	m_wnd->MouseMoveTo(pt.x(),pt.y());
	Sleep(200);
	m_wnd->MouseRightClicked();
	Sleep(200);
}

void Scheme_1::r_click(int x, int y)
{
	LockWindow t(m_wnd);
	m_wnd->MouseMoveTo(x, y);
	Sleep(200);
	m_wnd->MouseRightClicked();
}

void Scheme_1::click_move()
{
	LockWindow t(m_wnd);
	m_wnd->MouseMoveTo(512, 300);
	m_wnd->MouseLeftDown();
	Sleep(10);
	m_wnd->MouseMoveTo(514, 300);
	Sleep(9);
	m_wnd->MouseMoveTo(520, 300);
	Sleep(10);
	m_wnd->MouseMoveTo(530, 300);
	m_wnd->MouseLeftUp();
}

void Scheme_1::start_rerun_timer()
{
	int reds_in_hour=1;
	int reds_15_min = 0;
	quint64 currnt_time = QDateTime::currentDateTime().toMSecsSinceEpoch();
	for (int i = 0; i < m_redtime.size(); ++i)
	{
		//计算2小时之内来的红的次数
		if (currnt_time - m_redtime[i] <= 3600*2)
		{
			reds_in_hour = m_redtime.size() - i;
			break;
		}
	}
	for (int i = m_redtime.size() - 1; i >0; --i)
	{
		if (m_redtime[i] - m_redtime[i - 1] > 900)
		{
			break;
		}
		++reds_15_min;
	}
	quint64 sces = 2 * 60 * 1000;
	quint64 sces_inc = 0;
	qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
	sces_inc += (qrand() % (reds_in_hour*5)) * 60 * 1000;
	if (reds_15_min > 0)
	{
		sces_inc += (qrand() % (reds_15_min * 30)) * 60 * 1000;
	}
	sces = sces + sces_inc;
	if (m_timer == nullptr)
	{
		m_timer = new QTimer(this);
		connect(m_timer, SIGNAL(timeout()), this, SLOT(rerun_timeout()));
	}
	format_out_put(QString::fromLocal8Bit("重启定时器 ") + QString::number(sces / 60/1000)+QString::fromLocal8Bit(" 分钟..."), "#ffff00");
	m_timer->start(sces);
}

bool Scheme_1::check_bubble()
{
	return false;
}

bool Scheme_1::check_web()
{
	bool ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/网子无人区.bmp",nullptr,"000000",0.7);
	return ret;
}

bool Scheme_1::find_web_overview(int * pos)
{
	QPoint pt = m_configure.GetOverviewPos();
	bool ret = m_wnd->FindPicture(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/网子无人区总览.bmp",pos, "000000", 0.7);
	if (ret)
	{
		if (pos != nullptr)
		{
			pos[0] = pos[0] - 10;
			pos[1] = pos[1] + 5;
		}
	}
	return ret;
}

bool Scheme_1::check_broken()
{
	/*bool ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/扰断无人机.bmp", nullptr, "000000", 0.7);
	if (ret)
		return true;
	ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/扰断.bmp", nullptr, "000000", 0.7);
	if (ret)
		return true;
	ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/扰频.bmp", nullptr, "000000", 0.7);
	return ret;*/
	vector<int> rr = m_wnd->FindPictureEx(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/扰断无人机.bmp|pic/扰断.bmp|pic/扰频.bmp", "000000", 0.7);
	if (rr.size() != 0)
		return true;
	else
		return false;
}

bool Scheme_1::find_broken_overview(int * pos)
{
	QPoint pt = m_configure.GetOverviewPos();
	vector<int> rr = m_wnd->FindPictureEx(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/扰乱无人机总览.bmp|pic/扰断总览.bmp|pic/扰频总览.bmp|pic/扰频总览2.bmp", "000000", 0.7);
	if (rr.size() < 3)
		return false;
	else
	{
		if (pos != nullptr)
		{
			pos[0] = rr[1] - 10;
			pos[1] = rr[2] + 5;
		}
		return true;
	}
	/*bool ret = m_wnd->FindPicture(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/扰乱无人机总览.bmp", pos, "000000", 0.7);
	if (ret)
	{
		if (pos != nullptr)
		{
			pos[0] = pos[0] - 10;
			pos[1] = pos[1] + 5;
		}
		return true;
	}
	ret = m_wnd->FindPicture(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/扰断总览.bmp", pos, "000000", 0.7);
	if (ret)
	{
		if (pos != nullptr)
		{
			pos[0] = pos[0] - 10;
			pos[1] = pos[1] + 5;
		}
		return true;
	}
	ret = m_wnd->FindPicture(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/扰频总览.bmp", pos, "000000", 0.7);
	if (ret)
	{
		if (pos != nullptr)
		{
			pos[0] = pos[0] - 10;
			pos[1] = pos[1] + 5;
		}
		return true;
	}
	ret = m_wnd->FindPicture(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/扰频总览2.bmp", pos, "000000", 0.7);
	if (ret)
	{
		if (pos != nullptr)
		{
			pos[0] = pos[0] - 10;
			pos[1] = pos[1] + 5;
		}
		return true;
	}
	return false;*/
}

bool Scheme_1::check_red()
{
	QPoint pt_a = m_configure.GetAlliancePos();
	QPoint pt_l = m_configure.GetLocalPos();
	/*int pos[2];
	bool ret = m_wnd->FindPicture(pt_l.x(), pt_l.y(), pt_l.x()+300, m_wnd->GetWindowRect().bottom, "pic/Red.bmp",pos , "000000", 0.9);
	if (ret)
	{
		if (pos[1] < pt_a.y())
			return true;
	}
	ret = m_wnd->FindPicture(pt_l.x(), pt_l.y(), pt_l.x() + 300, m_wnd->GetWindowRect().bottom, "pic/Red1.bmp", pos, "000000", 0.9);
	if (ret)
	{
		if (pos[1] < pt_a.y())
			return true;
	}
	ret = m_wnd->FindPicture(pt_l.x(), pt_l.y(), pt_l.x() + 300, m_wnd->GetWindowRect().bottom, "pic/Red2.bmp", pos, "000000", 0.9);
	if (ret)
	{
		if (pos[1] < pt_a.y())
			return true;
	}*/
	vector<int> rr = m_wnd->FindPictureEx(pt_l.x(), pt_l.y(), pt_l.x() + 300, m_wnd->GetWindowRect().bottom, "pic/Red.bmp|pic/Red1.bmp|pic/Red2.bmp", "000000", 0.9);
	int cnt = rr.size() / 3;
	for (int i = 0; i < cnt; ++i)
	{
		if (rr[i * 3 + 2] < pt_a.y())
			return true;
	}
	return false;
}

bool Scheme_1::check_red_withstand(int *pos)
{
	/*switch_overview_page("空间");
	Sleep(500);
	QPoint pt_o = m_configure.GetOverviewPos();
	QPoint pt_drones = m_configure.GetDronesPos();
	bool ret = m_wnd->FindColor(pt_o.x(), pt_o.y(), m_wnd->GetWindowRect().right, pt_drones.y(), "FF1F1F - 000000", pos, 0.7);
	if (!ret)
		ret = m_wnd->FindColor(pt_o.x(), pt_o.y(), m_wnd->GetWindowRect().right, pt_drones.y(), "FF1E1E-000000", pos, 0.7);
	if (pos != nullptr)
	{
		pos[0] = pos[0] + 10;
		pos[1] = pos[1] + 4;
	}
	return ret;*/
	if (check_broken())
	{
		switch_overview_page("刷怪");
		DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, m_stateMachine);
		bool ret=find_broken_overview(pos);
		return ret;
	}
	return false;
}

int Scheme_1::withstand(int s)
{
	int orbit = 0;
	int pos[2] = { 0,0 };
	QPoint pt;
	pt = m_configure.GetInstrumentPanelPos("Equipment 4");
	l_click(pt);
	lanch_drones(s);
	bool ret = check_red_withstand(pos);
	if (!ret)
	{
		pt = m_configure.GetOverviewFirstItem();
		pos[0] = pt.x();
		pos[1] = pt.y();
	}
	if (1)
	{
		//环绕怪
		l_click(pos[0] + 5, pos[1] + 2);
		int n = 20;
		while (n > 0)
		{
			if (find_orbit(pos))
			{
				l_click(pos[0] + 5, pos[1] + 2);
				format_out_put(QString::fromLocal8Bit(" 环绕怪ok..."));
				break;
			}
			else
			{
				DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
			}
			--n;
		}
		n = 8;
		while (n > 0)
		{
			if (find_lockedtarget(pos))
			{
				l_click(100, 3);
				m_wnd->KeyPress('F');
				DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
				m_wnd->KeyPress('F');
				DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
				m_wnd->KeyPress('F');
				break;
			}
			else if (find_locktarget(pos))
			{
				l_click(pos[0] + 5, pos[1] + 2);
				DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(5, s);
				l_click(100, 3);
				m_wnd->KeyPress('F');
				DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
			}
			else
			{
				DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(2, s);
			}
			--n;
		}
	}
	while(1)
	{
		ret = check_red_withstand(pos);
		if (!ret)
			return OK;
		DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
	}
}

bool Scheme_1::check_hp()
{
	QPoint pt_panel = m_configure.GetInstrumentPanelPos();
	int sa = m_configure.GetShiledArmour();
	int p_hp[2];
	if(sa==1)
	{ 
		p_hp[0] = pt_panel.x() + 36;
		p_hp[1] = pt_panel.y() - 65;	//甲抗
	}
	else if (sa == 0)
	{
		p_hp[0] = pt_panel.x() + 5;
		p_hp[1] = pt_panel.y() - 55;	//盾抗 75%
	}
	else
	{
		p_hp[0] = pt_panel.x() + 5;
		p_hp[1] = pt_panel.y() - 55;	//盾抗 75%
	}
	//p_hp[0] = pt_panel.x() - 11;
	//p_hp[1] = pt_panel.y() - 30;	//盾抗
	

	if (m_wnd->FindColor(p_hp[0], p_hp[1], p_hp[0] + 5, p_hp[1] + 3, "FF1F1F-000000"))
	{
		return true;
	}
	if (m_wnd->FindColor(p_hp[0], p_hp[1], p_hp[0] + 5, p_hp[1] + 3, "FF1E1E-000000"))
	{
		return true;
	}
	return false;
}

int Scheme_1::low_hp_process(int s)
{
	int ret;
	ret=return_drones_to_bay(s);
	if (ret)
	{
		format_out_put(QString::fromLocal8Bit("无人机成功回站!!!"));
	}
	else
	{
		format_out_put(QString::fromLocal8Bit("回收无人机失败!!!"));
	}
	ret = wrap_to_station(s);
	if (ret)
	{
		DELAY_N_SECONDS_WITH_RED_WITHSTAND(60, 6);
		return OK;
	}
	return NOK;
}

int Scheme_1::try_repair(int s)
{
	QPoint pt_top = m_configure.GetInstrumentPanelPos("hp_lefttop");
	QPoint pt_buttom = m_configure.GetInstrumentPanelPos("hp_rightbuttom");
	//while (1)
	//{
	//	/*if (m_wnd->FindColor(pt_top.x(), pt_top.y(), pt_buttom.x(), pt_buttom.y(), "FF1F1F-000000"))
	//	{
	//		DELAY_N_SECONDS_RETURN(10, s);
	//	}
	//	else if (m_wnd->FindColor(pt_top.x(), pt_top.y(), pt_buttom.x(), pt_buttom.y(), "FF1E1E-000000"))
	//	{
	//		DELAY_N_SECONDS_RETURN(10, s);
	//	}
	//	else
	//		return OK;*/

	//}
	DELAY_N_SECONDS_RETURN(35, s);
	return OK;
}

bool Scheme_1::check_tethered()
{
	bool ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Tethered.bmp", nullptr, "000000", 0.7);
	return ret;
}

bool Scheme_1::open_probe_scanner()
{
	if (is_opened_probe_scanner())
		return true;
	else
	{
		l_click(100, 3);
		m_wnd->KeyPress('P');
		Sleep(1000);
		if (is_opened_probe_scanner())
			return true;
		else
			return false;
	}
}

bool Scheme_1::close_probe_scanner()
{
	if (is_opened_probe_scanner())
	{
		l_click(500, 50);
		m_wnd->KeyPress('P');
		Sleep(1000);
		if (is_opened_probe_scanner())
			return false;
		else
			return true;
	}
	else
		return true;
}

bool Scheme_1::is_opened_probe_scanner()
{
	bool ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/ProbeScanner.bmp", nullptr, "000000",0.7);
	return ret;
}

bool Scheme_1::find_exception(int * pos)
{
	bool ret = false;
	ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/ForsakenHub1.bmp", pos, "000000");
	if (ret)
		return true;
	ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/ForsakenHub2.bmp", pos, "000000");
	if (ret)
		return true;
	ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Forsaken Rally1.bmp", pos, "000000");
	if (ret)
		return true;
	ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Forsaken Rally2.bmp", pos, "000000");
	if (ret)
		return true;
	else
		return false;
}

void Scheme_1::close_popup_window()
{
	int pos[2] = { 0,0 };
	bool ret = false;
	ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/CloseComputer.bmp", pos, "000000", 0.8);
	if (ret)
	{
		l_click(pos[0] + 5, pos[1] + 3);
	}
	ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/CloseComputer.bmp", pos, "000000", 0.8);
	if (ret)
	{
		l_click(pos[0] + 5, pos[1] + 3);
	}
}

bool Scheme_1::is_nothing_found()
{
	QPoint pt_overview = m_configure.GetOverviewPos();
	bool ret= m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), pt_overview.x()+200, pt_overview.y()+100, "pic/Noting_Found.bmp", nullptr, "000000", 0.6);
	// 增加可靠性
	if (ret == false)
	{
		Sleep(200);
		ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), pt_overview.x() + 200, pt_overview.y() + 100, "pic/Noting_Found.bmp", nullptr, "000000", 0.6);
	}
	return ret;
}

bool Scheme_1::check_green()
{
	QPoint pt_youjun = m_configure.GetOverviewPos("友军");
	QPoint pt_overview = m_configure.GetOverviewPos("Base");
	l_click(pt_youjun);
	Sleep(1000);
	bool ret = false;
	ret=m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), pt_overview.x() + 200, pt_overview.y() + 400, "pic/Green_1.bmp", nullptr, "000000", 0.7);
	if (ret)return true;
	ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), pt_overview.x() + 200, pt_overview.y() + 400, "pic/Green_2.bmp", nullptr, "000000", 0.7);
	if (ret)return true;
	ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), pt_overview.x() + 200, pt_overview.y() + 400, "pic/Green_3.bmp", nullptr, "000000", 0.7);
	if (ret)return true;
	return false;
}

bool Scheme_1::check_building(int * pos)
{
	QPoint pt_buliding = m_configure.GetOverviewPos("建筑");
	QPoint pt_overview = m_configure.GetOverviewPos("Base");
	l_click(pt_buliding);
	Sleep(1000);
	bool ret = false;
	ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), pt_overview.x() + 200, pt_overview.y() + 200, "pic/Building_1.bmp", pos, "000000",0.7);
	if (ret)return true;
	ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), pt_overview.x() + 200, pt_overview.y() + 200, "pic/Building_2.bmp", pos, "000000",0.7);
	if (ret)return true;
	ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), pt_overview.x() + 200, pt_overview.y() + 200, "pic/Building_3.bmp", pos, "000000",0.7);
	if (ret)return true;
	ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), pt_overview.x() + 200, pt_overview.y() + 200, "pic/Building_4.bmp", pos, "000000",0.7);
	if (ret)return true;
	return false;
}

void Scheme_1::ignore_exception(QPoint pt)
{
	open_probe_scanner();
	r_click(pt);
	Sleep(500);
	QPoint o;
	o = m_configure.GetProbeScannerWraptoWithin_o("Ignore Result");
	l_click(pt.x() + o.x(), pt.y() + o.y());
	Sleep(500);
	l_click(100, 5);
}

bool Scheme_1::switch_overview_page(QString page)
{
	QPoint pt = m_configure.GetOverviewPos(page);
	if (pt.x() > 0 && pt.y() > 0)
	{
		l_click(pt);
		return true;
	}
	else
		return false;
}

void Scheme_1::format_out_put(const QString& i,const QString& color)
{
	QString info;
	info = m_roleName + i;
	output(info,color);
}

bool Scheme_1::check_stateMachine(int csm)
{
	if (m_stateMachine != csm)
		return false;
	else
		return true;
}

void Scheme_1::print_state_machine(int cs,int s)
{
	switch (s)
	{
		/* 0-正常刷怪 1-意外情况之后回空间站 2-安全退出 3-暂停 4-释放控制权*/
	case 0:
		format_out_put(state_char[cs]+QString::fromLocal8Bit(" 响应正常刷怪事件"),USER_INPUT_EVENT_COLOR);
		break;
	case 1:
		format_out_put(state_char[cs] + QString::fromLocal8Bit(" 响应回空间站事件"), USER_INPUT_EVENT_COLOR);
		break;
	case 2:
		format_out_put(state_char[cs] + QString::fromLocal8Bit(" 响应安全退出事件"), USER_INPUT_EVENT_COLOR);
		break;
	case 3:
		format_out_put(state_char[cs] + QString::fromLocal8Bit(" 响应暂停事件"), USER_INPUT_EVENT_COLOR);
		break;
	case 4:
		format_out_put(state_char[cs] + QString::fromLocal8Bit(" 响应释放控制权事件"), USER_INPUT_EVENT_COLOR);
		break;
	default:
		break;
	}
}

bool Scheme_1::is_an_egg()
{
	QPoint pt_drones = m_configure.GetDronesPos();
	bool ret = m_wnd->FindPicture(pt_drones.x()-1, pt_drones.y()-1, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Drones.bmp", nullptr, "000000", 0.7);
	return !ret;
}

bool Scheme_1::check_sub_windows()
{
	QString setting = QString("./setting/") + m_roleName + "-Setting.ini";
	QSettings settings(setting, QSettings::IniFormat);
	settings.beginGroup("Window");
	int pos[2] = { 0,0 };
	QPoint porigin;
	bool update_flag = false;
	vector<int> rr = m_wnd->FindPictureEx(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/select_item.bmp|pic/Overview.bmp|pic/RightArrow.bmp|pic/Local.bmp|pic/Drones.bmp", "000000", 0.7);
	if (rr.size() != 5 * 3)
	{
		format_out_put(QString::fromLocal8Bit("检测窗口失败，窗口检测失败."), "#ff0000");
		return false;
	}
	for (int i = 0; i < 5; ++i)
	{
		int id = rr[i * 3];
		pos[0] = rr[i * 3 + 1];
		pos[1] = rr[i * 3 + 2];
		switch (id)
		{
		case 1:
			porigin = m_configure.GetSelectItemPos();
			if (porigin.x() != pos[0] || porigin.y() != pos[1])
			{
				settings.setValue("Select Item", QString::number(pos[0]) + " " + QString::number(pos[1]));
				update_flag = true;
			}
			break;
		case 2:
			porigin = m_configure.GetOverviewPos();
			if (porigin.x() != pos[0] || porigin.y() != pos[1])
			{
				settings.setValue("Overview", QString::number(pos[0]) + " " + QString::number(pos[1]));
				update_flag = true;
			}
			break;
		case 3:
			porigin = m_configure.GetInstrumentPanelPos();
			if (porigin.x() != pos[0] + 9 || porigin.y() != pos[1] + 4)
			{
				settings.setValue("Instrument Panel", QString::number(pos[0] + 9) + " " + QString::number(pos[1] + 4));
				update_flag = true;
			}
			break;
		case 4:
			porigin = m_configure.GetLocalPos();
			if (porigin.x() != pos[0] || porigin.y() != pos[1])
			{
				settings.setValue("Local", QString::number(pos[0]) + " " + QString::number(pos[1]));
				update_flag = true;
			}
			break;
		case 5:
			porigin = m_configure.GetDronesPos();
			if (porigin.x() != pos[0] || porigin.y() != pos[1])
			{
				settings.setValue("Drones", QString::number(pos[0]) + " " + QString::number(pos[1]));
				update_flag = true;
			}
			break;
		default:
			break;
		}
	}
	settings.endGroup();
	if (update_flag)
	{
		m_configure.Loadsettings(m_roleName);
	}
	//bool ret = find_select_item(pos);
	//if (!ret)
	//{
	//	format_out_put(QString::fromLocal8Bit("未检测到Select_Item，窗口检测失败."), "#ff0000");
	//	return false;
	//}
	//porigin = m_configure.GetSelectItemPos();
	//if (porigin.x() != pos[0] || porigin.y() != pos[1])
	//{
	//	settings.setValue("Select Item", QString::number(pos[0]) + " " + QString::number(pos[1]));
	//	update_flag = true;
	//}
	//Sleep(500);
	//ret = find_overview(pos);
	//if (!ret)
	//{
	//	format_out_put(QString::fromLocal8Bit("未检测到Overview，窗口检测失败."), "#ff0000");
	//	return false;
	//}
	//porigin = m_configure.GetOverviewPos();
	//if (porigin.x() != pos[0] || porigin.y() != pos[1])
	//{
	//	settings.setValue("Overview", QString::number(pos[0]) + " " + QString::number(pos[1]));
	//	update_flag = true;
	//}
	//Sleep(500);
	//ret = find_panel(pos);
	//if (!ret)
	//{
	//	format_out_put(QString::fromLocal8Bit("未检测到Panel，窗口检测失败."), "#ff0000");
	//	return false;
	//}
	//porigin = m_configure.GetInstrumentPanelPos();
	//if (porigin.x() != pos[0] + 9 || porigin.y() != pos[1] + 4)
	//{
	//	settings.setValue("Instrument Panel", QString::number(pos[0] + 9) + " " + QString::number(pos[1] + 4));
	//	update_flag = true;
	//}
	//Sleep(500);
	//ret = find_local(pos);
	//if (!ret)
	//{
	//	format_out_put(QString::fromLocal8Bit("未检测到Local，窗口检测失败."), "#ff0000");
	//	return false;
	//}
	//porigin = m_configure.GetLocalPos();
	//if (porigin.x() != pos[0] || porigin.y() != pos[1])
	//{
	//	settings.setValue("Local", QString::number(pos[0]) + " " + QString::number(pos[1]));
	//	update_flag = true;
	//}
	//if (update_flag)
	//{
	//	m_configure.Loadsettings(m_roleName);
	//}
	//switch_overview_page("空间");
	//Sleep(500);
	//if (!find_station(pos))
	//{
	//	format_out_put(QString::fromLocal8Bit("窗口检测，找不到空间站"), "#ff0000");
	//	return false;
	//}
	////if (!open_probe_scanner())
	////{
	////	format_out_put(QString::fromLocal8Bit("窗口检测，找不到异常窗口"), "#ff0000");
	////	return false;
	////}
	////switch_overview_page("刷怪");
	////DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(2, m_stateMachine);
	////close_probe_scanner();
	//ret = find_drones(pos);
	//if (!ret)
	//{
	//format_out_put(QString::fromLocal8Bit("未检测到Drones，窗口检测失败."), "#ff0000");
	//return false;
	//}
	//porigin = m_configure.GetDronesPos();
	//if (porigin.x() != pos[0] || porigin.y() != pos[1])
	//{
	//settings.setValue("Drones", QString::number(pos[0]) + " " + QString::number(pos[1]));
	//update_flag = true;
	//}
	return true;
}

void Scheme_1::stop_timer_slot()
{
	QMutexLocker locker(&m_tMutex);
	if (m_timer != nullptr)
	{
		if (m_timer->isActive())
		{
			m_timer->stop();
		}
	}
}

void Scheme_1::window_dection_slot()
{
	format_out_put(QString::fromLocal8Bit("窗口检测"));
	QString title("EVE - ");
	title += m_roleName;
	const wchar_t * wtitle = reinterpret_cast<const wchar_t *>(title.utf16());
	HWND wid = FindWindow(__T("triuiScreen"), wtitle);
	if (wid == NULL)
	{
		m_errorCode = 4;
		format_out_put(QString::fromLocal8Bit(errorCode[m_errorCode]));
		emit eState(this, false, 2);
		emit quit(this);
		if (m_wnd != nullptr)
		{
			m_wnd->Detach();
			delete m_wnd;
			m_wnd = nullptr;
		}
		return;
	}
	if (m_wnd == nullptr)
	{
		m_wnd = new LwIWnd();
		bool ret = false;
		if (!m_wnd->Attch((int)wid))
		{
			ret = try_attach_window();
			if (false == ret)
			{
				m_errorCode = 1;	//无法Attach窗口
				format_out_put(QString::fromLocal8Bit(errorCode[m_errorCode]));
				delete m_wnd;
				m_wnd = nullptr;
				emit eState(this, false, 2);
				emit quit(this);
				return;
			}
			else
			{
				format_out_put(QString::fromLocal8Bit("绑定窗口成功."));
			}
		}
	}
	QString setting = QString("./setting/") + m_roleName + "-Setting.ini";
	QSettings settings(setting, QSettings::IniFormat);
	settings.beginGroup("Window");
	RECT wrect;
	GetWindowRect(wid, &wrect);
	settings.setValue("Window Geometry", QString::number(wrect.right-wrect.left) + " " + QString::number(wrect.bottom-wrect.top));

	int pos[2] = { 0,0 };
	bool ret = find_undock(pos);
	if (!ret)
	{
		format_out_put(QString::fromLocal8Bit("未检测到Undock,窗口检测失败."), "#ff0000");
		emit WindowPositionDetectionFinish(false);
		return;
	}
	settings.setValue("Undock", QString::number(pos[0]) + " " + QString::number(pos[1]));
	l_click(pos[0],pos[1]);
	Sleep(20000);
	ret = find_select_item(pos);
	if (!ret)
	{
		format_out_put(QString::fromLocal8Bit("未检测到Select_Item，窗口检测失败."), "#ff0000");
		emit WindowPositionDetectionFinish(false);
		return;
	}
	settings.setValue("Select Item", QString::number(pos[0]) + " " + QString::number(pos[1]));
	ret = find_overview(pos);
	if (!ret)
	{
		format_out_put(QString::fromLocal8Bit("未检测到Overview，窗口检测失败."), "#ff0000");
		emit WindowPositionDetectionFinish(false);
		return;
	}
	settings.setValue("Overview", QString::number(pos[0]) + " " + QString::number(pos[1]));
	ret = find_drones(pos);
	if (!ret)
	{
		format_out_put(QString::fromLocal8Bit("未检测到Drones，窗口检测失败."), "#ff0000");
		emit WindowPositionDetectionFinish(false);
		return;
	}
	settings.setValue("Drones", QString::number(pos[0]) + " " + QString::number(pos[1]));
	ret = find_panel(pos);
	if (!ret)
	{
		format_out_put(QString::fromLocal8Bit("未检测到Panel，窗口检测失败."), "#ff0000");
		emit WindowPositionDetectionFinish(false);
		return;
	}
	settings.setValue("Instrument Panel", QString::number(pos[0]+9) + " " + QString::number(pos[1]+4));
	ret = find_local(pos);
	if (!ret)
	{
		format_out_put(QString::fromLocal8Bit("未检测到Local，窗口检测失败."), "#ff0000");
		emit WindowPositionDetectionFinish(false);
		return;
	}
	settings.setValue("Local", QString::number(pos[0]) + " " + QString::number(pos[1]));
	settings.endGroup();
	format_out_put(QString::fromLocal8Bit("窗口检测完毕."), "#00ff00");
	emit WindowPositionDetectionFinish(true);
}

void Scheme_1::rerun_timeout()
{
	m_timer->stop();
	format_out_put(QString::fromLocal8Bit(" 重新尝试刷怪"));
	m_stateMachine = 0;
	m_normalState = -1;
	run();
}

void Scheme_1::run()
{
	bool quit_flag = false;
	//加载配置文件
	if (!m_configure.Loadsettings(m_roleName))
	{
		m_errorCode = 3;
		format_out_put(QString::fromLocal8Bit(errorCode[m_errorCode]));
		emit eState(this, false, 2);
		emit quit(this);
		return;
	}
	QString title("EVE - ");
	title += m_roleName;
	const wchar_t * wtitle = reinterpret_cast<const wchar_t *>(title.utf16());
	HWND wid = FindWindow(__T("triuiScreen"), wtitle);
	if (wid == NULL)
	{
		m_errorCode = 4;
		format_out_put(QString::fromLocal8Bit(errorCode[m_errorCode]));
		emit eState(this, false, 2);
		emit quit(this);
		if (m_wnd != nullptr)
		{
			m_wnd->Detach();
			delete m_wnd;
			m_wnd = nullptr;
		}
		return;
	}
	if (m_wnd == nullptr)
	{
		m_wnd = new LwIWnd();
		bool ret = false;
		if (!m_wnd->Attch((int)wid))
		{
			ret = try_attach_window();
			if (false == ret)
			{
				m_errorCode = 1;	//无法Attach窗口
				format_out_put(QString::fromLocal8Bit(errorCode[m_errorCode]));
				delete m_wnd;
				m_wnd = nullptr;
				emit eState(this, false, 2);
				emit quit(this);
				return;
			}
			else
			{
				format_out_put(QString::fromLocal8Bit("绑定窗口成功."));
			}
		}
	}
	emit eState(this, true, 2);
	bool n_quit = true;
	//检查
	if (is_out_space())
	{
		if (!check_sub_windows())
		{
			format_out_put(QString::fromLocal8Bit("窗口检测失败,退出线程."));
			m_wnd->Detach();
			delete m_wnd;
			m_wnd = nullptr;
			emit eState(this, true, 0);	/* 成功安全退出*/
			emit quit(this);
			return;
		}
	}
	else
	{
		format_out_put(QString::fromLocal8Bit("空间站内"));
	}
	Sleep(2000);
	//主循环
	while (n_quit)
	{
		int ret = NOK;
		switch (m_stateMachine)
		{
		case 1:	//意外情况之后,回空间站
			CHECK_STATEMACHINE_BREAK(1);
			ret = safe_back_station(1);
			if (OK==ret)
			{
				if (m_shutdown > QDateTime::currentDateTime())
				{
					format_out_put(QString::fromLocal8Bit(" 有异常情况,本次刷怪结束，n分钟之后重新尝试刷怪."));
					start_rerun_timer();
					m_stateMachine = -1;
				}
				else
				{
					format_out_put(QString::fromLocal8Bit(" 达到下线时间，退出脚本!!!"), "#ff0000");
					m_stateMachine = 2;
				}
			}
			else if(RED_WITHSTAND == ret)
			{
				m_stateMachine = 5;	//对抗
			}
			else
			{
				;
			}
			break;
		case 6:	 //低血处理
			{
				ret = safe_back_station(6);
				if (OK == ret)
				{
					m_stateMachine = 0;
					m_normalState = -1;
				}
				else if (RED_WITHSTAND == ret)
				{
					m_stateMachine = 5;	//对抗
				}
				else
				{
					;
				}
			}
			break;
		case 2: //安全退出
			if (OK==safe_back_station(2))
			{
				m_stateMachine = -1;
				format_out_put(QString::fromLocal8Bit(" 安全退出刷怪脚本."));
				quit_flag = true;
			}
			break;
		case 3:	//暂停
		{
			if (OK==safe_back_station(3))
			{
				m_stateMachine = -1;
				format_out_put(QString::fromLocal8Bit(" 刷怪操作暂停成功."));
				emit eState(this, true, 1);
			}
			break;
		}
		case 0:	//正常刷怪
			ret = normal();
			if (OK==ret)
			{
				//m_stateMachine = 0;	//有红，安全回站之后退出
				;
			}
			else if (RED == ret)
			{
				m_stateMachine = 1;
				format_out_put(QString::fromLocal8Bit(" 刷怪过程中出现异常情况（来红）!!!"));
			}
			else if (LOW_HP == ret)
			{
				m_stateMachine = 6;
				format_out_put(QString::fromLocal8Bit(" 刷怪过程找那个出现异常情况(低血)!!!"));
			}
			else if (NOK == ret)
			{
				m_stateMachine = 1;
				format_out_put(QString::fromLocal8Bit(" 刷怪过程找那个出现异常情况!!!"));
			}
			else if(SHUT_DOWN == ret)
			{
				format_out_put(QString::fromLocal8Bit(" 到达下线时间!!! 下线"), "#ff0000");
				m_stateMachine = 1;
				;
			}
			else if (WINDOW_CHECK_FAIL == ret)
			{
				format_out_put(QString::fromLocal8Bit(" 窗口检测失败,下线"), "#ff0000");
				safe_back_station(0);
				m_stateMachine = -1;
				quit_flag = true;
			}
			break;
		case 4://释放控制权
			m_stateMachine = -1;
			emit eState(this, true, 3);
			break;
		case 5:
			{
				ret = withstand(5);
				if (OK == ret)
				{
					m_stateMachine = 1;
					format_out_put(QString::fromLocal8Bit(" 对抗红结束!!!"));
				}
			}break;
		default:
			n_quit = false;
			break;
		}
	}
	//m_wnd->Detach();
	//delete m_wnd;
	//m_wnd = nullptr;
	m_stateMachine = -1;
	m_normalState = -1;
	format_out_put(QString::fromLocal8Bit(" 退出刷怪主程."));
	if (quit_flag)
	{
		m_wnd->Detach();
		delete m_wnd;
		m_wnd = nullptr;
		emit eState(this, true, 0);	/* 成功安全退出*/
		emit quit(this);
	}
	//while (1)
	//{
	//	Sleep(2000);
	//	qDebug() << "test.";
	//}
}