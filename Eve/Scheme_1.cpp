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
		idle = 0/*��״̬*/,in_station/*�ռ�վ��*/,out_station/*�ռ�վ��*/,go_abnormal/*ǰ���쳣*/,\
		go_station/*�ز�*/
	};
	const char errorCode[][100] =
	{
		/*0*/"ok.",\
		/*1*/"�޷�Attach����.",\
		/*2*/"��δ�ܻؿռ�վ",\
		/*3*/"û���ҵ������ļ�"\
		/*4*/"���ڲ�����"
	};
	const QString state_char[5] =
	{
		QString::fromLocal8Bit("��ǰ����ˢ��״̬��"),
		QString::fromLocal8Bit("��ǰ�������֮��ؿռ�վ״̬��"),
		QString::fromLocal8Bit("��ǰ��ȫ�˳�״̬��"),
		QString::fromLocal8Bit("��ǰ��ͣ״̬��"),
		QString::fromLocal8Bit("��ǰ�ͷſ���Ȩ״̬��")
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
				format_out_put(QString::fromLocal8Bit("������!!!"),SECURITY_COLOR);\
				m_redtime.push_back(QDateTime::currentDateTime().toSecsSinceEpoch());\
				m_normalState = -1;\
				return RED;\
			}\
			if (check_hp())\
			{\
				grab(m_wnd->GetWindowId(), GetRole());\
				format_out_put(QString::fromLocal8Bit("��Ѫ��!!!"),SECURITY_COLOR);\
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
		m_stateMachine = 3;	//�ؿռ�վ
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
 *��ȫ��վ
 */
int Scheme_1::safe_back_station(int s)
{
	/*format_out_put(QString::fromLocal8Bit(" ִ�л�վ����."));*/
	click_move();
	int pos[2] = { 0,0 };
	if (is_in_space())	//�ռ�վ��
	{
		format_out_put(QString::fromLocal8Bit(" �ɹ���վ"));
		return OK;
	}
	else if(is_out_space())	//�ռ�վ��
	{
		switch_overview_page("�ռ�");
		//�������˻�
		int ret=return_drones_to_bay(s);
		if (ret == NOK)
		{
			format_out_put(QString::fromLocal8Bit(" �������˻�ʧ��."));
			if (!check_hp() && !check_red())	//���û�к�͵�Ѫ,�����������˻�
			{
				return NOK;	
			}
		}
		else if (ret == OK)
		{
			format_out_put(QString::fromLocal8Bit(" �������˻��ɹ�."));
		}
		else if(ret == RED_WITHSTAND)	//���Ѿ������أ��Կ�
		{
			return RED_WITHSTAND;
		}
		else
		{
			//print_state_machine(ret);
			return ret;
		}
		//����ռ�ҳ��
		switch_overview_page("�ռ�");
		DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
		if (find_station(pos)) //�ռ�վ
		{
			l_click(pos[0], pos[1]);
			DELAY_N_SECONDS_WITH_RED_WITHSTAND(1, s);
			QPoint pt = m_configure.GetStationItemPos("Approach");
			//l_click(pt);
			//DELAY_N_SECONDS_WITH_RED_WITHSTAND(2, s);	//��ʱ2s
			pt = m_configure.GetStationItemPos("Wrap to with 0 m");
			l_click(pt);
			/*DELAY_N_SECONDS_WITH_RED_WITHSTAND(5, s);
			QPoint pt1;
			pt1 = m_configure.GetInstrumentPanelPos("Equipment 4");
			l_click(pt1);
			format_out_put(QString::fromLocal8Bit(" �ر�����"));*/
			int n = 60;
			while (n > 0)
			{
				DELAY_N_SECONDS_WITH_RED_WITHSTAND(2, s);	//��ʱ2s
				if (check_tethered())
					break;
				--n;
			}
			DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(8, s);	//��ʱ2s
			pt = m_configure.GetStationItemPos("Dock");
			switch_overview_page("�ռ�");
			DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(2, s);
			l_click(pt);
			n = 10;
			while (n > 0)
			{
				DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(2, s);	//��ʱ2s
				if (is_in_space())
				{
					return OK;
				}
				--n;
			}
		}
		else //�Ҳ����ռ�վ
		{
			if (is_out_space())
			{
				format_out_put(QString::fromLocal8Bit(" �Ҳ����ռ�վ"));
				return NOK;
			}
			else
			{
				int n = 5;
				while (n > 0)
				{
					DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(2, s);	//��ʱ2s
					if (is_in_space())
					{
						return OK;
					}
					--n;
				}
			}
		}
	}   
	else //���ڻ�վ(��վ)�Ĺ�����
	{
		format_out_put(QString::fromLocal8Bit(" ���ڻ�վ����ȴ�..."));
		int n = 5;
		while (n > 0)
		{
			DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(2, s);	//��ʱ2s
			if (is_in_space())
			{
				return OK;
			}
			--n;
		}
	}
	format_out_put(QString::fromLocal8Bit(" �ؿռ�վʧ��."));
	return NOK;
}
/*
*��ȫ��վ
*/
int Scheme_1::safe_leave_station(int s)
{
	if (is_in_space())
	{
		if (check_red())
		{
			//grab(m_wnd->GetWindowId(), GetRole());
			m_redtime.push_back(QDateTime::currentDateTime().toSecsSinceEpoch());
			format_out_put(QString::fromLocal8Bit(" ���죬�ܾ���վ����."));
			return RED;	//�к�
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
			//ͣ��
			QPoint pt = m_configure.GetInstrumentPanelPos("Stop The Ship");
			l_click(pt);
			DELAY_N_SECONDS_RETURN(3, s);
			//������װ��
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
			return OK;	//�ɹ���վ
		}
		else
		{
			return NOK;	//��վ�����쳣
		}
	}
	else
		return OK;	//�ɹ���վ(�Ѿ���վ����)	
}

int Scheme_1::wrap_to_station(int s)
{
	int pos[2];
	//����ռ�ҳ��
	QPoint pt = m_configure.GetOverviewPos("�ռ�");
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
		format_out_put(QString::fromLocal8Bit(" �Ҳ����ռ�վ."));
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
	case -1: // ��һ�ν��룬�ж��Ƿ���վ�ڣ����ǻ�վ
		if (m_shutdown <= QDateTime::currentDateTime())
		{
			return SHUT_DOWN;
		}
		format_out_put(QString::fromLocal8Bit(" ��ʼִ��ˢ���߼�."));
		format_out_put(QString::fromLocal8Bit(" ִ��ˢ��ǰ��վ����."));
		ret = safe_back_station(0);
		if (ret==OK)
		{
			format_out_put(QString::fromLocal8Bit(" �ɹ���վ."));
			m_normalState = 0;
		}
		else if(ret==NOK)
		{
			format_out_put(QString::fromLocal8Bit(" ���Ի�վ�쳣."));
		}
		else
		{
			;// print_state_machine(ret);
		}
		break;
	case 0: // �ڿռ�վ��,��վ
		if (is_in_space())
		{
			format_out_put(QString::fromLocal8Bit(" ִ��ˢ�ֳ�վ����."));
			int ret = safe_leave_station(s);
			if (ret == OK)
			{
				format_out_put(QString::fromLocal8Bit(" �ɹ���վ��������ģ��."));
				if (is_an_egg())
				{
					format_out_put(QString::fromLocal8Bit(" ����һֻ�����˳��ű�."));
					m_stateMachine = 2;
					return OK;
				}
				ret = try_repair(s);
				if (RED == ret)
					return RED;
				m_normalState = 1;
			}
			else if (ret == NOK)	//�쳣
			{
				grab(m_wnd->GetWindowId(), GetRole());
				format_out_put(QString::fromLocal8Bit(" ��վ�����쳣���ܾ���վ����."));
				return NOK;
			}
			else if (ret == RED)	//����
			{
				grab(m_wnd->GetWindowId(), GetRole());
				format_out_put(QString::fromLocal8Bit(" ��վ�����쳣���ܾ���վ����."));
				return RED;
			}
			else
				return ret;
		}
		else
		{
			format_out_put(QString::fromLocal8Bit(" �ɹ���վ��������ģ��."));
			m_normalState = 1;
		}
		break;
	case 1:	// վ�⣬ѡȡ�쳣
	{
		if (!is_out_space())
		{
			m_normalState = 0;	//վ��δ��վ
			break;
		}
		else
		{
			format_out_put(QString::fromLocal8Bit(" ѡȡ�쳣."));
			ret = open_probe_scanner();
			if (!ret)
			{
				format_out_put(QString::fromLocal8Bit(" ���쳣���ʧ��."));
				break;
			}
			else
			{
				ret = find_exception(pos);
				//����Ҳ����쳣������
				if (!ret)
				{
					close_probe_scanner();
					format_out_put(QString::fromLocal8Bit("�Ҳ����쳣."));
					return NOK;
				}
				 //�ҵ��쳣
				if(1)
				{
					QPoint currnt_exception_pt;
					if (1)
					{
						QMutexLocker locker(m_gmutexs[m_groupName].data());
						DELAY_N_SECONDS_RETURN(1, s);
						LockWindow t(m_wnd);
						format_out_put(QString::fromLocal8Bit(" �ҵ���ˢ�쳣,ǰ��..."));
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
						//�����whileѭ���ж��Ƿ��й�
						DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(8, s);
						format_out_put(QString::fromLocal8Bit(" ����쳣���Ƿ��й�..."));
						while (n > 0)
						{
							switch_overview_page("ˢ��");
							if (is_nothing_found())
							{
								DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);	//��ʱ1s
								--n;
							}
							else
							{
								DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);	//��ʱ1s����һ��
								if (is_nothing_found())
								{
									DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
									--n;
								}
								else
								{
									format_out_put(QString::fromLocal8Bit(" ��鵽�쳣���й�..."));
									break;
								}
							}

						}
						//�쳣��û�й֣�wrap to station
						if (is_nothing_found())	//���û�й֣��������쳣
						{
							format_out_put(QString::fromLocal8Bit(" �쳣��û�йֿ�ˢ�������쳣, Wrap to�ռ�վ 0m. "));
							//�����쳣����
							ignore_exception(currnt_exception_pt);
							DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(1, s);
							close_probe_scanner();
							DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(2, s);
							DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(10, s);
							//wrap to �ռ�վ
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
						//�й֣�����Ѿ�����齨�������
						format_out_put(QString::fromLocal8Bit(" �쳣���йֿ�ˢ������Ѿ�..."));
						//����Ѿ�����������Ѿ���wrap to station
						ret = check_green();
					}
					if (ret)
					{
						format_out_put(QString::fromLocal8Bit(" �����Ѿ��������쳣,8s�� Wrap to �ռ�վ 0m."));
						DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(8, s);
						//�����쳣����
						ignore_exception(currnt_exception_pt);
						//�ر��쳣����
						close_probe_scanner();
						//wrap to �ռ�վ
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
					//û���Ѿ�
					close_probe_scanner();
					format_out_put(QString::fromLocal8Bit(" û���Ѿ���̽���Ƿ��пɻ��ƵĽ�����..."));
					CHECK_STATEMACHIN_RETURN(s);
					
					//��齨�������л���
					if (check_building(pos))
					{
						format_out_put(QString::fromLocal8Bit(" ̽�⵽�ɻ��ƽ����������..."));
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
								format_out_put(QString::fromLocal8Bit(" ���ƽ�����ɹ�.ִ���ͷ����˻�����..."));
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
							format_out_put(QString::fromLocal8Bit(" �ͷ����˻��ɹ����л���ˢ��ҳ��."));
							switch_overview_page("ˢ��");

						}
						else if (ret == NOK)
						{
							format_out_put(QString::fromLocal8Bit(" �ͷ����˻�ʧ��."));
							m_stateMachine = -1;	//ˢ���쳣��������վ
							return NOK;
						}
						else
						{
							return ret;
						}

					}
					else //���Ƶ�һ����
					{
						format_out_put(QString::fromLocal8Bit(" δ̽�⵽�ɻ��ƵĽ�������Ƶ�һ����..."));
						switch_overview_page("ˢ��");
						QPoint pt_one = m_configure.GetOverviewFirstItem();
						l_click(pt_one);
						int n = 5;
						bool ff = false;
						while (n > 0)
						{
							switch_overview_page("ˢ��");
							if (find_orbit(pos))
							{
								l_click(pos[0] + 5, pos[1] + 2);
								format_out_put(QString::fromLocal8Bit(" ���Ƶ�һ���ֳɹ�.ִ���ͷ����˻�����..."));
								ff = true;
								break;
							}
							else
								DELAY_N_SECONDS_RETURN(2, s);
							--n;
						}
						if (!ff)
						{
							format_out_put(QString::fromLocal8Bit("���ƹ�ʧ��..."));
							return NOK;
						}
						QPoint pt;
						pt = m_configure.GetInstrumentPanelPos("Equipment 4");
						l_click(pt);
						int ret = lanch_drones(s);
						if (OK == ret)	//�ͷ����˻��ɹ�
						{
							/*QPoint pt;
							pt = m_configure.GetInstrumentPanelPos("Equipment 4");
							l_click(pt);*/
							DELAY_N_SECONDS_RETURN(1, s);
							format_out_put(QString::fromLocal8Bit(" �ͷ����˻��ɹ�.ˢ�֣�׼�����Ʋк�..."));
							switch_overview_page("�к�");
							DELAY_N_SECONDS_RETURN(5, s);	//�����л�֮�󣬽����ӳ�
							while (1)
							{
								switch_overview_page("�к�");
								DELAY_N_SECONDS_RETURN(1, s);
								if (!is_nothing_found())
								{
									l_click(pt_one);
									DELAY_N_SECONDS_RETURN(1, s);
									if (find_orbit(pos))
									{
										l_click(pos[0] + 5, pos[1] + 2);
										format_out_put(QString::fromLocal8Bit(" ���Ʋк��ɹ�,�л���ˢ�ֽ���."));
										m_normalState = 2;
										break;
									}
								}
								DELAY_N_SECONDS_RETURN(1, s);
							}
							switch_overview_page("ˢ��");
						}
						else if (NOK == ret)
						{
							format_out_put(QString::fromLocal8Bit(" �ͷ����˻�ʧ��."));
							m_normalState = -1;	//ˢ���쳣����վ
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
		//����Ŷ�
		if (check_broken())
		{
			format_out_put(QString::fromLocal8Bit("��鵽�Ŷ�,�����"), "#55ffff");
			switch_overview_page("ˢ��");
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
				format_out_put(QString::fromLocal8Bit("��鵽�Ŷ�,�����"), "#55ffff");
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
		//���Ӽ��
		if (check_web())
		{
			switch_overview_page("ˢ��");
			DELAY_N_SECONDS_RETURN(1, s);
			int pos[2] = { 0,0 };
			bool ret = find_web_overview(pos);
			if (ret)
			{
				format_out_put(QString::fromLocal8Bit("��鵽����,�����"),"#55ffff");
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
				format_out_put(QString::fromLocal8Bit("��鵽����,�����"),"#55ffff");
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
		//������
		if (check_red())
		{
			grab(m_wnd->GetWindowId(), GetRole());
			format_out_put(QString::fromLocal8Bit("����!!!"), SECURITY_COLOR);
			m_redtime.push_back(QDateTime::currentDateTime().toSecsSinceEpoch());
			m_normalState = -1;
			return RED;
		}
		//��Ѫ���
		if (check_hp())	
		{
			grab(m_wnd->GetWindowId(), GetRole());
			format_out_put(QString::fromLocal8Bit("��Ѫ!!!"), SECURITY_COLOR);
			m_normalState = -1;
			return LOW_HP;
		}
		//�����Ƿ�ˢ�꣬ÿ20s���һ��
		if (count_t_check_guai % 5 == 0)
		{
			LockWindow t(m_wnd);
			count_t_check_guai = 0;
			switch_overview_page("ˢ��");
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
							format_out_put(QString::fromLocal8Bit(" �쳣�й��Ѿ��������!!! ���¸��쳣"), "#0000ff");
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
		//��������������
		if (count_t_flash_yellow % 2 == 0&&is_flash_yellow()&&!is_nothing_found())
		{
			count_t_flash_yellow = 0;
			switch_overview_page("ˢ��");
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
						format_out_put(QString::fromLocal8Bit("���ֳɹ�."));
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
						format_out_put(QString::fromLocal8Bit("���ֳɹ�."));
						break;
					}
				}
				--n;
			}
			if (is_flash_yellow() && !is_nothing_found())
			{
				format_out_put(QString::fromLocal8Bit(" ��⵽���ƵĲк���ʧ�������µĲк�ing."));
				switch_overview_page("�к�");
				DELAY_N_SECONDS_RETURN(2, s);
				QPoint pt_one;
				pt_one = m_configure.GetOverviewFirstItem();
				l_click(pt_one);
				DELAY_N_SECONDS_RETURN(1, s);
				if (find_orbit(pos))
				{
					l_click(pos[0] + 5, pos[1] + 2);
					format_out_put(QString::fromLocal8Bit(" ���Ʋк��ɹ�,�л���ˢ�ֽ���."));
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
	//����ǵ�ֱ�ӷ��سɹ�
	if (is_an_egg())
		return OK;
	if (is_in_space())
	{
		return OK;	//����ִ�гɹ�
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
		//�ж�Drones in Bay�Ƿ�չ������չ��������
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
		if (find_station(pos)) //�ռ�վ
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
		format_out_put(QString::fromLocal8Bit(" �ر�����"));
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
		return false;	//�ڿռ�վ��
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
	
	//�к�,�̣���
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
	//��
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
	//����
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
			Sleep(5000);	//5 ��attachһ��
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
		//����2Сʱ֮�����ĺ�Ĵ���
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
	format_out_put(QString::fromLocal8Bit("������ʱ�� ") + QString::number(sces / 60/1000)+QString::fromLocal8Bit(" ����..."), "#ffff00");
	m_timer->start(sces);
}

bool Scheme_1::check_bubble()
{
	return false;
}

bool Scheme_1::check_web()
{
	bool ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/����������.bmp",nullptr,"000000",0.7);
	return ret;
}

bool Scheme_1::find_web_overview(int * pos)
{
	QPoint pt = m_configure.GetOverviewPos();
	bool ret = m_wnd->FindPicture(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/��������������.bmp",pos, "000000", 0.7);
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
	/*bool ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/�Ŷ����˻�.bmp", nullptr, "000000", 0.7);
	if (ret)
		return true;
	ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/�Ŷ�.bmp", nullptr, "000000", 0.7);
	if (ret)
		return true;
	ret = m_wnd->FindPicture(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/��Ƶ.bmp", nullptr, "000000", 0.7);
	return ret;*/
	vector<int> rr = m_wnd->FindPictureEx(0, 0, m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/�Ŷ����˻�.bmp|pic/�Ŷ�.bmp|pic/��Ƶ.bmp", "000000", 0.7);
	if (rr.size() != 0)
		return true;
	else
		return false;
}

bool Scheme_1::find_broken_overview(int * pos)
{
	QPoint pt = m_configure.GetOverviewPos();
	vector<int> rr = m_wnd->FindPictureEx(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/�������˻�����.bmp|pic/�Ŷ�����.bmp|pic/��Ƶ����.bmp|pic/��Ƶ����2.bmp", "000000", 0.7);
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
	/*bool ret = m_wnd->FindPicture(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/�������˻�����.bmp", pos, "000000", 0.7);
	if (ret)
	{
		if (pos != nullptr)
		{
			pos[0] = pos[0] - 10;
			pos[1] = pos[1] + 5;
		}
		return true;
	}
	ret = m_wnd->FindPicture(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/�Ŷ�����.bmp", pos, "000000", 0.7);
	if (ret)
	{
		if (pos != nullptr)
		{
			pos[0] = pos[0] - 10;
			pos[1] = pos[1] + 5;
		}
		return true;
	}
	ret = m_wnd->FindPicture(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/��Ƶ����.bmp", pos, "000000", 0.7);
	if (ret)
	{
		if (pos != nullptr)
		{
			pos[0] = pos[0] - 10;
			pos[1] = pos[1] + 5;
		}
		return true;
	}
	ret = m_wnd->FindPicture(pt.x(), pt.y(), m_wnd->GetWindowRect().right, m_wnd->GetWindowRect().bottom, "pic/��Ƶ����2.bmp", pos, "000000", 0.7);
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
	/*switch_overview_page("�ռ�");
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
		switch_overview_page("ˢ��");
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
		//���ƹ�
		l_click(pos[0] + 5, pos[1] + 2);
		int n = 20;
		while (n > 0)
		{
			if (find_orbit(pos))
			{
				l_click(pos[0] + 5, pos[1] + 2);
				format_out_put(QString::fromLocal8Bit(" ���ƹ�ok..."));
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
		p_hp[1] = pt_panel.y() - 65;	//�׿�
	}
	else if (sa == 0)
	{
		p_hp[0] = pt_panel.x() + 5;
		p_hp[1] = pt_panel.y() - 55;	//�ܿ� 75%
	}
	else
	{
		p_hp[0] = pt_panel.x() + 5;
		p_hp[1] = pt_panel.y() - 55;	//�ܿ� 75%
	}
	//p_hp[0] = pt_panel.x() - 11;
	//p_hp[1] = pt_panel.y() - 30;	//�ܿ�
	

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
		format_out_put(QString::fromLocal8Bit("���˻��ɹ���վ!!!"));
	}
	else
	{
		format_out_put(QString::fromLocal8Bit("�������˻�ʧ��!!!"));
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
	// ���ӿɿ���
	if (ret == false)
	{
		Sleep(200);
		ret = m_wnd->FindPicture(pt_overview.x(), pt_overview.y(), pt_overview.x() + 200, pt_overview.y() + 100, "pic/Noting_Found.bmp", nullptr, "000000", 0.6);
	}
	return ret;
}

bool Scheme_1::check_green()
{
	QPoint pt_youjun = m_configure.GetOverviewPos("�Ѿ�");
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
	QPoint pt_buliding = m_configure.GetOverviewPos("����");
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
		/* 0-����ˢ�� 1-�������֮��ؿռ�վ 2-��ȫ�˳� 3-��ͣ 4-�ͷſ���Ȩ*/
	case 0:
		format_out_put(state_char[cs]+QString::fromLocal8Bit(" ��Ӧ����ˢ���¼�"),USER_INPUT_EVENT_COLOR);
		break;
	case 1:
		format_out_put(state_char[cs] + QString::fromLocal8Bit(" ��Ӧ�ؿռ�վ�¼�"), USER_INPUT_EVENT_COLOR);
		break;
	case 2:
		format_out_put(state_char[cs] + QString::fromLocal8Bit(" ��Ӧ��ȫ�˳��¼�"), USER_INPUT_EVENT_COLOR);
		break;
	case 3:
		format_out_put(state_char[cs] + QString::fromLocal8Bit(" ��Ӧ��ͣ�¼�"), USER_INPUT_EVENT_COLOR);
		break;
	case 4:
		format_out_put(state_char[cs] + QString::fromLocal8Bit(" ��Ӧ�ͷſ���Ȩ�¼�"), USER_INPUT_EVENT_COLOR);
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
		format_out_put(QString::fromLocal8Bit("��ⴰ��ʧ�ܣ����ڼ��ʧ��."), "#ff0000");
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
	//	format_out_put(QString::fromLocal8Bit("δ��⵽Select_Item�����ڼ��ʧ��."), "#ff0000");
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
	//	format_out_put(QString::fromLocal8Bit("δ��⵽Overview�����ڼ��ʧ��."), "#ff0000");
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
	//	format_out_put(QString::fromLocal8Bit("δ��⵽Panel�����ڼ��ʧ��."), "#ff0000");
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
	//	format_out_put(QString::fromLocal8Bit("δ��⵽Local�����ڼ��ʧ��."), "#ff0000");
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
	//switch_overview_page("�ռ�");
	//Sleep(500);
	//if (!find_station(pos))
	//{
	//	format_out_put(QString::fromLocal8Bit("���ڼ�⣬�Ҳ����ռ�վ"), "#ff0000");
	//	return false;
	//}
	////if (!open_probe_scanner())
	////{
	////	format_out_put(QString::fromLocal8Bit("���ڼ�⣬�Ҳ����쳣����"), "#ff0000");
	////	return false;
	////}
	////switch_overview_page("ˢ��");
	////DELAY_N_SECONDS_WITH_NO_RED_HP_RETURN(2, m_stateMachine);
	////close_probe_scanner();
	//ret = find_drones(pos);
	//if (!ret)
	//{
	//format_out_put(QString::fromLocal8Bit("δ��⵽Drones�����ڼ��ʧ��."), "#ff0000");
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
	format_out_put(QString::fromLocal8Bit("���ڼ��"));
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
				m_errorCode = 1;	//�޷�Attach����
				format_out_put(QString::fromLocal8Bit(errorCode[m_errorCode]));
				delete m_wnd;
				m_wnd = nullptr;
				emit eState(this, false, 2);
				emit quit(this);
				return;
			}
			else
			{
				format_out_put(QString::fromLocal8Bit("�󶨴��ڳɹ�."));
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
		format_out_put(QString::fromLocal8Bit("δ��⵽Undock,���ڼ��ʧ��."), "#ff0000");
		emit WindowPositionDetectionFinish(false);
		return;
	}
	settings.setValue("Undock", QString::number(pos[0]) + " " + QString::number(pos[1]));
	l_click(pos[0],pos[1]);
	Sleep(20000);
	ret = find_select_item(pos);
	if (!ret)
	{
		format_out_put(QString::fromLocal8Bit("δ��⵽Select_Item�����ڼ��ʧ��."), "#ff0000");
		emit WindowPositionDetectionFinish(false);
		return;
	}
	settings.setValue("Select Item", QString::number(pos[0]) + " " + QString::number(pos[1]));
	ret = find_overview(pos);
	if (!ret)
	{
		format_out_put(QString::fromLocal8Bit("δ��⵽Overview�����ڼ��ʧ��."), "#ff0000");
		emit WindowPositionDetectionFinish(false);
		return;
	}
	settings.setValue("Overview", QString::number(pos[0]) + " " + QString::number(pos[1]));
	ret = find_drones(pos);
	if (!ret)
	{
		format_out_put(QString::fromLocal8Bit("δ��⵽Drones�����ڼ��ʧ��."), "#ff0000");
		emit WindowPositionDetectionFinish(false);
		return;
	}
	settings.setValue("Drones", QString::number(pos[0]) + " " + QString::number(pos[1]));
	ret = find_panel(pos);
	if (!ret)
	{
		format_out_put(QString::fromLocal8Bit("δ��⵽Panel�����ڼ��ʧ��."), "#ff0000");
		emit WindowPositionDetectionFinish(false);
		return;
	}
	settings.setValue("Instrument Panel", QString::number(pos[0]+9) + " " + QString::number(pos[1]+4));
	ret = find_local(pos);
	if (!ret)
	{
		format_out_put(QString::fromLocal8Bit("δ��⵽Local�����ڼ��ʧ��."), "#ff0000");
		emit WindowPositionDetectionFinish(false);
		return;
	}
	settings.setValue("Local", QString::number(pos[0]) + " " + QString::number(pos[1]));
	settings.endGroup();
	format_out_put(QString::fromLocal8Bit("���ڼ�����."), "#00ff00");
	emit WindowPositionDetectionFinish(true);
}

void Scheme_1::rerun_timeout()
{
	m_timer->stop();
	format_out_put(QString::fromLocal8Bit(" ���³���ˢ��"));
	m_stateMachine = 0;
	m_normalState = -1;
	run();
}

void Scheme_1::run()
{
	bool quit_flag = false;
	//���������ļ�
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
				m_errorCode = 1;	//�޷�Attach����
				format_out_put(QString::fromLocal8Bit(errorCode[m_errorCode]));
				delete m_wnd;
				m_wnd = nullptr;
				emit eState(this, false, 2);
				emit quit(this);
				return;
			}
			else
			{
				format_out_put(QString::fromLocal8Bit("�󶨴��ڳɹ�."));
			}
		}
	}
	emit eState(this, true, 2);
	bool n_quit = true;
	//���
	if (is_out_space())
	{
		if (!check_sub_windows())
		{
			format_out_put(QString::fromLocal8Bit("���ڼ��ʧ��,�˳��߳�."));
			m_wnd->Detach();
			delete m_wnd;
			m_wnd = nullptr;
			emit eState(this, true, 0);	/* �ɹ���ȫ�˳�*/
			emit quit(this);
			return;
		}
	}
	else
	{
		format_out_put(QString::fromLocal8Bit("�ռ�վ��"));
	}
	Sleep(2000);
	//��ѭ��
	while (n_quit)
	{
		int ret = NOK;
		switch (m_stateMachine)
		{
		case 1:	//�������֮��,�ؿռ�վ
			CHECK_STATEMACHINE_BREAK(1);
			ret = safe_back_station(1);
			if (OK==ret)
			{
				if (m_shutdown > QDateTime::currentDateTime())
				{
					format_out_put(QString::fromLocal8Bit(" ���쳣���,����ˢ�ֽ�����n����֮�����³���ˢ��."));
					start_rerun_timer();
					m_stateMachine = -1;
				}
				else
				{
					format_out_put(QString::fromLocal8Bit(" �ﵽ����ʱ�䣬�˳��ű�!!!"), "#ff0000");
					m_stateMachine = 2;
				}
			}
			else if(RED_WITHSTAND == ret)
			{
				m_stateMachine = 5;	//�Կ�
			}
			else
			{
				;
			}
			break;
		case 6:	 //��Ѫ����
			{
				ret = safe_back_station(6);
				if (OK == ret)
				{
					m_stateMachine = 0;
					m_normalState = -1;
				}
				else if (RED_WITHSTAND == ret)
				{
					m_stateMachine = 5;	//�Կ�
				}
				else
				{
					;
				}
			}
			break;
		case 2: //��ȫ�˳�
			if (OK==safe_back_station(2))
			{
				m_stateMachine = -1;
				format_out_put(QString::fromLocal8Bit(" ��ȫ�˳�ˢ�ֽű�."));
				quit_flag = true;
			}
			break;
		case 3:	//��ͣ
		{
			if (OK==safe_back_station(3))
			{
				m_stateMachine = -1;
				format_out_put(QString::fromLocal8Bit(" ˢ�ֲ�����ͣ�ɹ�."));
				emit eState(this, true, 1);
			}
			break;
		}
		case 0:	//����ˢ��
			ret = normal();
			if (OK==ret)
			{
				//m_stateMachine = 0;	//�к죬��ȫ��վ֮���˳�
				;
			}
			else if (RED == ret)
			{
				m_stateMachine = 1;
				format_out_put(QString::fromLocal8Bit(" ˢ�ֹ����г����쳣��������죩!!!"));
			}
			else if (LOW_HP == ret)
			{
				m_stateMachine = 6;
				format_out_put(QString::fromLocal8Bit(" ˢ�ֹ������Ǹ������쳣���(��Ѫ)!!!"));
			}
			else if (NOK == ret)
			{
				m_stateMachine = 1;
				format_out_put(QString::fromLocal8Bit(" ˢ�ֹ������Ǹ������쳣���!!!"));
			}
			else if(SHUT_DOWN == ret)
			{
				format_out_put(QString::fromLocal8Bit(" ��������ʱ��!!! ����"), "#ff0000");
				m_stateMachine = 1;
				;
			}
			else if (WINDOW_CHECK_FAIL == ret)
			{
				format_out_put(QString::fromLocal8Bit(" ���ڼ��ʧ��,����"), "#ff0000");
				safe_back_station(0);
				m_stateMachine = -1;
				quit_flag = true;
			}
			break;
		case 4://�ͷſ���Ȩ
			m_stateMachine = -1;
			emit eState(this, true, 3);
			break;
		case 5:
			{
				ret = withstand(5);
				if (OK == ret)
				{
					m_stateMachine = 1;
					format_out_put(QString::fromLocal8Bit(" �Կ������!!!"));
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
	format_out_put(QString::fromLocal8Bit(" �˳�ˢ������."));
	if (quit_flag)
	{
		m_wnd->Detach();
		delete m_wnd;
		m_wnd = nullptr;
		emit eState(this, true, 0);	/* �ɹ���ȫ�˳�*/
		emit quit(this);
	}
	//while (1)
	//{
	//	Sleep(2000);
	//	qDebug() << "test.";
	//}
}