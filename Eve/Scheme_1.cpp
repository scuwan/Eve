#include "Scheme_1.h"
#include "LwIWnd.h"

#include <QDebug>
#include <QPoint>
#include <Windows.h>
#include <QMutexLocker>

namespace
{
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
#define DELAY_N_SECONDS_RETURN(n,s) if(1)\
	{\
		int nnn=n;\
		while(nnn>0)\
		{\
			if (check_red())\
			{\
				format_out_put(QString::fromLocal8Bit(" 来红!!!"));\
				m_normalState = -1;\
				return NOK;\
			}\
			if (check_hp())\
			{\
				format_out_put(QString::fromLocal8Bit(" 低血!!!"));\
				m_normalState = -1;\
				return NOK;\
			}\
			CHECK_STATEMACHIN_RETURN(s);\
			Sleep(1000);\
			--nnn;\
		}\
	}
}

Scheme_1::Scheme_1(QString role)
	:IScheme(role)
{
	m_roleName = role;
	connect(this, SIGNAL(start()), this, SLOT(Run()));
}


Scheme_1::~Scheme_1()
{
	if (m_timer != nullptr)
	{
		m_timer->stop();
		delete m_timer;
		m_timer = nullptr;
	}
}


void Scheme_1::SafeExit()
{
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
	if (m_stateMachine == -1)
	{
		emit eState(this, true, 1);
	}
	else
		m_stateMachine = 3;	//回空间站
}

void Scheme_1::Start()
{
	m_stateMachine = 0;
	emit start();
}

void Scheme_1::ReleaseControl()
{
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
/*
 *安全回站
 */
int Scheme_1::safe_back_station(int s)
{
	format_out_put(QString::fromLocal8Bit(" 执行回站操作."));
	int repeat_times = 5;
	for (int i = 0; i < repeat_times; ++i)
	{
		CHECK_STATEMACHIN_RETURN(s);
		int pos[2] = { 0,0 };
		if (is_in_space())	//空间站内
		{
			format_out_put(QString::fromLocal8Bit(" 成功回站"));
			return OK;
		}
		else if(is_out_space())	//空间站外
		{
			//回收无人机
			int ret=return_drones_to_bay(s);
			if (ret == NOK)
			{
				format_out_put(QString::fromLocal8Bit(" 回收无人机失败."));
			}
			else if (ret == OK)
			{
				format_out_put(QString::fromLocal8Bit(" 回收无人机成功."));
			}
			else
			{
				//print_state_machine(ret);
				return ret;
			}
			//进入空间页面
			QPoint pt=m_configure.GetOverviewPos("空间");
			l_click(pt);
			DELAY_N_SECONDS_RETURN(1, s);
			if (find_station(pos)) //空间站
			{
				l_click(pos[0] + 2, pos[1] + 2);
				Sleep(500);
				pt = m_configure.GetStationItemPos("Approach");
				l_click(pt);
				DELAY_N_SECONDS_RETURN(2, s);	//延时2s
				pt = m_configure.GetStationItemPos("Dock");
				l_click(pt);
				DELAY_N_SECONDS_RETURN(30, s);	//延时30s
				if (is_in_space())
				{
					format_out_put(QString::fromLocal8Bit(" 成功回站."));
					return OK;
				}
				else
				{
					continue;
				}
			}
			else //找不到空间站
			{
				format_out_put(QString::fromLocal8Bit(" 找不到空间站"));
				DELAY_N_SECONDS_RETURN(5, s);	//延时30s
				continue;
			}
		}   
		else //正在回站(出站)的过程中
		{
			format_out_put(QString::fromLocal8Bit(" 正在回站，请等待..."));
			DELAY_N_SECONDS_RETURN(5, s);
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
			format_out_put(QString::fromLocal8Bit(" 来红，拒绝出站请求."));
			return NOK;	//有红
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
		DELAY_N_SECONDS_RETURN(1, s);
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
		DELAY_N_SECONDS_RETURN(10, s);
		int n = 30;
		while (n > 0)
		{
			if (check_tethered())
				return OK;
			else
			{
				--n;
				DELAY_N_SECONDS_RETURN(1, s);
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
				m_normalState = 1;
			}
			else if (ret == NOK)	//有红
			{
				format_out_put(QString::fromLocal8Bit(" 出站操作异常，拒绝出站请求."));
				return NOK;
			}
			else	//出站操作异常
			{
				;//print_state_machine(ret);
			}
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
				QMutexLocker locker(m_gmutexs[m_groupName].data());
				ret = find_exception(pos);
				if (ret) //打开异常
				{
					format_out_put(QString::fromLocal8Bit(" 找到可刷异常,前往..."));
					QPoint currnt_exception_pt(pos[0] + 5, pos[1] + 2);
					QPoint o;
					o = m_configure.GetProbeScannerWraptoWithin_o("Wrap to Within");
					QPoint wrap_to_within(pos[0] + o.x(), pos[1] + o.y());
					o = m_configure.GetProbeScannerWraptoWithin_o("Within 30 km");
					QPoint within_n_km(pos[0] + o.x(), pos[1] + o.y());
					r_click(pos[0] + 5, pos[1] + 2);
					DELAY_N_SECONDS_RETURN(2, s);
					m_wnd->MouseMoveTo(wrap_to_within.x(), wrap_to_within.y());
					DELAY_N_SECONDS_RETURN(1, s);
					l_click(within_n_km);
					Sleep(1000);
					m_wnd->MouseMoveTo(500, 10);
					close_popup_window();
					DELAY_N_SECONDS_RETURN(1, s);
					close_popup_window();
					DELAY_N_SECONDS_RETURN(1, s);
					/*close_probe_scanner();
					DELAY_N_SECONDS_RETURN(2, s);*/
					int n = 60;
					//下面的while循环判断是否有怪
					format_out_put(QString::fromLocal8Bit(" 检查异常里是否有怪..."));
					while (n > 0)
					{
						switch_overview_page("刷怪");
						if (is_nothing_found())
						{
							DELAY_N_SECONDS_RETURN(1, s);	//延时1s
							--n;
						}
						else
						{
							DELAY_N_SECONDS_RETURN(1, s);	//延时1s再判一次
							if (is_nothing_found())
							{
								DELAY_N_SECONDS_RETURN(1, s);
								--n;
							}
							else
							{
								format_out_put(QString::fromLocal8Bit(" 检查到异常里有怪..."));
								break;
							}
						}

					}
					if (is_nothing_found())	//如果没有怪，则屏蔽异常
					{
						format_out_put(QString::fromLocal8Bit(" 异常中没有怪可刷，屏蔽异常, Wrap to空间站 0m. "));
						//屏蔽异常代码
						ignore_exception(currnt_exception_pt);
						DELAY_N_SECONDS_RETURN(1, s);
						close_probe_scanner();
						DELAY_N_SECONDS_RETURN(2, s);
						DELAY_N_SECONDS_RETURN(10, s);
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
					else //有怪，检查友军，检查建筑物，检查红
					{
						format_out_put(QString::fromLocal8Bit(" 异常中有怪可刷，检查友军..."));
						//检查友军
						if (check_green())
						{
							format_out_put(QString::fromLocal8Bit(" 存在友军，屏蔽异常,8s后 Wrap to 空间站 0m."));
							DELAY_N_SECONDS_RETURN(8, s);
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
						else
						{
							close_probe_scanner();
							format_out_put(QString::fromLocal8Bit(" 没有友军，探测是否有可环绕的建筑物..."));
							CHECK_STATEMACHIN_RETURN(s);
						}
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
							int ret = lanch_drones(s);
							if (ret == OK)
							{
								QPoint pt;
								pt = m_configure.GetInstrumentPanelPos("Equipment 4");
								l_click(pt);
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
							while (n > 0)
							{
								if (find_orbit(pos))
								{
									l_click(pos[0] + 5, pos[1] + 2);
									format_out_put(QString::fromLocal8Bit(" 环绕第一个怪成功.执行释放无人机操作..."));
									break;
								}
								else
									DELAY_N_SECONDS_RETURN(1, s);
							}
							int ret = lanch_drones(s);
							if (OK == ret)	//释放无人机成功
							{
								QPoint pt;
								pt = m_configure.GetInstrumentPanelPos("Equipment 4");
								l_click(pt);
								DELAY_N_SECONDS_RETURN(1, s);
								format_out_put(QString::fromLocal8Bit(" 释放无人机成功.刷怪，准备环绕残骸..."));
								switch_overview_page("残骸");
								while (1)
								{
									switch_overview_page("残骸");
									if (check_red())	//来红
									{
										format_out_put(QString::fromLocal8Bit(" 来红!!!"));
										m_normalState = -1;
										return NOK;
									}
									if (check_hp())	//低血
									{
										format_out_put(QString::fromLocal8Bit(" 低血!!!"));
										m_normalState = -1;
										return NOK;
									}
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

					}
					m_normalState = 2;
				}
				else
				{
					format_out_put(QString::fromLocal8Bit(" 找不到异常."));
					return NOK;
				}
			}
		}
		break;
	}
	case 2:
		static int count_t_check_guai = 0;
		static int count_t_flash_yellow = 0;
		if (check_red())	//来红
		{
			format_out_put(QString::fromLocal8Bit(" 来红!!!"));
			m_normalState = -1;
			return NOK;
		}
		if (check_hp())	//低血
		{
			format_out_put(QString::fromLocal8Bit(" 低血!!!"));
			m_normalState = -1;
			return NOK;
		}
		//检查怪是否刷完，每20s检测一次
		if (count_t_check_guai % 10 == 0)
		{
			count_t_check_guai = 0;
			switch_overview_page("刷怪");
			m_wnd->LockInput(2);
			DELAY_N_SECONDS_RETURN(1, s);
			if (is_nothing_found())
			{
				if (is_nothing_found())
				{
					m_wnd->LockInput(0);
					format_out_put(QString::fromLocal8Bit(" 异常中怪已经清理完毕!!!"));
					m_normalState = -1;
					return NOK;
				}
				else
					m_wnd->LockInput(0);
			}
			else
				m_wnd->LockInput(0);
			
		}
		//怪物锁定，闪黄
		if (count_t_flash_yellow % 5 == 0&&is_flash_yellow()&&!is_nothing_found())
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
		}
		DELAY_N_SECONDS_RETURN(2, s);
		++count_t_check_guai;
		++count_t_flash_yellow;
		break;
	}
	return OK;
}


int Scheme_1::return_drones_to_bay(int s)
{
	if (is_in_space())
	{
		return OK;	//操作执行成功
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
			Sleep(200);
		}
		pt = m_configure.GetDronesPos("Drones in Local Space");
		QPoint pt1 = m_configure.GetDronesLocalSpacePos("Return to Drone Bay");
		r_click(pt);
		Sleep(1000);
		l_click(pt1);
		DELAY_N_SECONDS_RETURN(5, s);
		int n = 15;
		while (n>0)
		{
			if (!is_drone_backing())
			{
				if (is_drone_in_space())
				{
					--n;
					DELAY_N_SECONDS_RETURN(1, s);
					m_wnd->LockInput(2);
					r_click(pt);
					Sleep(1000);
					l_click(pt1);
					m_wnd->LockInput(0);
					DELAY_N_SECONDS_RETURN(2, s);
					//format_out_put(QString::fromLocal8Bit("无人机在空间中."));
				}
				else
					break;
			}
			else
			{
				--n;
				DELAY_N_SECONDS_RETURN(2, s);
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
	if (is_in_space())
	{
		return false;	//在空间站内
	}
	QPoint pt = m_configure.GetDronesPos("Drones in Bay");
	r_click(pt);
	DELAY_N_SECONDS_RETURN(1, s);
	QPoint pt_1 = m_configure.GetDroneBayPos("Lanch drones");
	l_click(pt_1);
	int n = 10;
	while (n>0)
	{
		if (!is_drone_in_space())
		{
			--n;
			DELAY_N_SECONDS_RETURN(1, s);
			r_click(pt);
			DELAY_N_SECONDS_RETURN(1, s);
			l_click(pt_1);
			DELAY_N_SECONDS_RETURN(1, s);
		}
		else
		{
			return OK;
		}
	}
	qDebug() << "pt: (" << pt.x() << "," << pt.y() << ")";
	qDebug() << "pt_1: (" << pt_1.x() << "," << pt_1.y() << ")";
	return NOK;
}

bool Scheme_1::is_drone_in_space()
{
	//if (is_in_space())
	//{
	//	return false;	//在空间站内
	//}
	//QPoint pt = m_configure.GetOverviewPos("无人机");
	//l_click(pt);
	//Sleep(200);
	//pt = m_configure.GetOverviewPos("Base");
	//bool ret = m_wnd->FindPicture(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Drones_icon.bmp", nullptr, "000000", 0.7);
	//return ret;
	
	QPoint pt;
	pt=m_configure.GetDronesPos();
	//判红,绿，黄
	std::string r = m_wnd->FindColorEx(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "F80000-000000|F40000-000000|00F400-000000|00F800-000000|F4F400-000000|FFFF00-000000|E1E101-000000|F5F500-000000");
	if (r.size() == 0)
		return false;
	else
		return true;
}

bool Scheme_1::is_drone_backing()
{
	QPoint pt;
	pt = m_configure.GetDronesPos();
	//判红,绿，黄
	std::string r = m_wnd->FindColorEx(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "F4F400-000000|FFFF00-000000|E1E101-000000|F5F500-000000");
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
	std::string r = m_wnd->FindColorEx(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "00F400-000000|00F800-000000");
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
	bool ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Overview.bmp", nullptr, "000000",0.7);
	return ret;
}

bool Scheme_1::find_station(int * pos)
{
	QPoint pt_overview = m_configure.GetOverviewPos();
	bool ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Station1.bmp", pos, "000000", 0.7);
	if (!ret)
	{
		ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Station2.bmp", pos, "000000", 0.7);
		if (!ret)
		{
			ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Station3.bmp", pos, "000000", 0.7);
		}
		if (!ret)
		{
			ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/StationNormal.bmp", pos, "000000", 0.7);
		}
	}
	return ret;
}

bool Scheme_1::find_overview(int * pos)
{
	return m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Overview.bmp", pos, "000000", 0.7);
}

bool Scheme_1::find_orbit(int *pos)
{
	QPoint pt_si= m_configure.GetSelectItemPos();
	bool ret = m_wnd->FindPicture(pt_si.x(), pt_si.y(),pt_si.x()+150, pt_si.y()+100, "pic/Orbit.bmp", pos, "000000",0.7);
	return ret;
}

bool Scheme_1::find_locktarget(int * pos)
{
	QPoint pt_si = m_configure.GetSelectItemPos();
	bool ret = m_wnd->FindPicture(pt_si.x(), pt_si.y(), pt_si.x() + 150, pt_si.y() + 100, "pic/LockTarget.bmp", pos, "000000",0.7);
	return ret;
}

bool Scheme_1::find_select_item(int * pos)
{
	return m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/select_item.bmp", pos, "000000", 0.7);
}

void Scheme_1::l_click(QPoint pt)
{
	m_wnd->LockInput(2);
	m_wnd->MouseMoveTo(pt.x(), pt.y());
	Sleep(200);
	m_wnd->MouseLeftClicked();
	m_wnd->LockInput(0);
}

void Scheme_1::l_click(int x, int y)
{
	m_wnd->LockInput(2);
	m_wnd->MouseMoveTo(x, y);
	Sleep(200);
	m_wnd->MouseLeftClicked();
	m_wnd->LockInput(0);
}

void Scheme_1::r_click(QPoint pt)
{
	m_wnd->LockInput(2);
	m_wnd->MouseMoveTo(pt.x(),pt.y());
	Sleep(200);
	m_wnd->MouseRightClicked();
	m_wnd->LockInput(0);
}

void Scheme_1::r_click(int x, int y)
{
	m_wnd->LockInput(2);
	m_wnd->MouseMoveTo(x, y);
	Sleep(200);
	m_wnd->MouseRightClicked();
	m_wnd->LockInput(0);
}

void Scheme_1::start_rerun_timer()
{
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(rerun_timeout()));
	m_timer->start(1000 * 60 * 2);
}

bool Scheme_1::check_bubble()
{
	return false;
}

bool Scheme_1::check_red()
{
	QPoint pt_a = m_configure.GetAlliancePos();
	QPoint pt_l = m_configure.GetLocalPos();
	int pos[2];
	bool ret = m_wnd->FindPicture(pt_l.x(), pt_l.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/Red.bmp",pos , "000000", 0.7);
	if (ret)
	{
		if (pos[1] > pt_a.y())
			return false;
		else
			return true;
	}
	else
		return false;
}

bool Scheme_1::check_hp()
{
	QPoint pt_panel = m_configure.GetInstrumentPanelPos();
	int p_hp[2];
	p_hp[0] = pt_panel.x() + 36;
	p_hp[1] = pt_panel.y() - 65;
	if (m_wnd->FindColor(p_hp[0], p_hp[1], p_hp[0] + 5, p_hp[1] + 3, "FF1F1F-000000"))
		return true;
	if (m_wnd->FindColor(p_hp[0], p_hp[1], p_hp[0] + 5, p_hp[1] + 3, "FF1E1E-000000"))
		return true;
	return false;
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
	ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/CloseComputer.bmp", pos, "000000", 0.7);
	if (ret)
	{
		l_click(pos[0] + 5, pos[1] + 3);
	}
}

bool Scheme_1::is_nothing_found()
{
	QPoint pt_overview = m_configure.GetOverviewPos();
	return m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), pt_overview.x()+200, pt_overview.y()+100, "pic/Noting_Found.bmp", nullptr, "000000", 0.7);
}

bool Scheme_1::check_green()
{
	QPoint pt_youjun = m_configure.GetOverviewPos("友军");
	QPoint pt_overview = m_configure.GetOverviewPos("Base");
	l_click(pt_youjun);
	Sleep(1000);
	bool ret = false;
	ret=m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), pt_overview.x() + 200, pt_overview.y() + 200, "pic/Green_1.bmp", nullptr, "000000", 0.7);
	if (ret)return true;
	ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), pt_overview.x() + 200, pt_overview.y() + 200, "pic/Green_2.bmp", nullptr, "000000", 0.7);
	if (ret)return true;
	ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), pt_overview.x() + 200, pt_overview.y() + 200, "pic/Green_3.bmp", nullptr, "000000", 0.7);
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

void Scheme_1::format_out_put(QString  i)
{
	QString info;
	info = m_roleName + i;
	output(info);
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
		format_out_put(state_char[cs]+QString::fromLocal8Bit(" 响应正常刷怪事件"));
		break;
	case 1:
		format_out_put(state_char[cs] + QString::fromLocal8Bit(" 响应回空间站事件"));
		break;
	case 2:
		format_out_put(state_char[cs] + QString::fromLocal8Bit(" 响应安全退出事件"));
		break;
	case 3:
		format_out_put(state_char[cs] + QString::fromLocal8Bit(" 响应暂停事件"));
		break;
	case 4:
		format_out_put(state_char[cs] + QString::fromLocal8Bit(" 响应释放控制权事件"));
		break;
	default:
		break;
	}
}

void Scheme_1::rerun_timeout()
{
	m_timer->stop();
	delete m_timer;
	m_timer = nullptr;
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
	//主循环
	while (n_quit)
	{
		int ret = NOK;
		switch (m_stateMachine)
		{
		case 1:	//意外情况之后,回空间站
			CHECK_STATEMACHINE_BREAK(1);
			if (OK==safe_back_station(1))
			{
				format_out_put(QString::fromLocal8Bit(" 有异常情况或者当前异常中怪已清理完毕，本次刷怪结束，5分钟之后重新尝试刷怪."));
				start_rerun_timer();
				m_stateMachine = -1;
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
			else if (NOK == ret)
			{
				m_stateMachine = 1;
				format_out_put(QString::fromLocal8Bit(" 刷怪过程中出现异常情况（低血、来红、无人机释放失败等等...）或者异常中已无怪!!!"));
			}
			else
			{
				//print_state_machine(ret);
				;
			}
			break;
		case 4://释放控制权
			m_stateMachine = -1;
			emit eState(this, true, 3);
			break;
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