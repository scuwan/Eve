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
	enum {OK=-1,NOK=-2,RED=-3,LOW_HP=-4,RED_WITHSTAND=-5/*����Կ�*/};
	virtual void run();
	//��ȫ�ؿռ�վ
	int safe_back_station(int s);
	//��ȫ��վ
	int safe_leave_station(int s);
	//wrap to �ռ�վ
	int wrap_to_station(int s);
	int normal(int s);
	int return_drones_to_bay(int s);
	int lanch_drones(int s);
	//�ж����˻��Ƿ��ڿռ���
	bool is_drone_in_space();
	//�ж����˻��Ƿ��ڴ��Ĺ�����
	bool is_drone_backing();
	//�����ж�,�ж������˻���ɫ�����йֶ϶�����
	bool is_flash_yellow();
	//�󶨴���
	bool try_attach_window();
	//�жϴ��ڿռ�վ��
	bool is_in_space();
	//�жϴ��ڿռ�վ��
	bool is_out_space();
	//�ռ�վ
	bool find_station(int *pos=nullptr);
	//����
	bool find_overview(int *pos = nullptr);
	//���һ���
	bool find_orbit(int *pos=nullptr);
	//�������Ŀ��
	bool find_locktarget(int *pos = nullptr);
	bool find_lockedtarget(int *pos = nullptr);
	//Select item
	bool find_select_item(int *pos = nullptr);
	//���������
	void l_click(QPoint pt);
	void l_click(int x, int y);
	//����Ҽ����
	void r_click(QPoint pt);
	void r_click(int x, int y);
	//����ˢ�ֶ�ʱ�� n����
	void start_rerun_timer();
	//����Ƿ�������
	bool check_bubble();
	//���Ӽ��
	bool check_web();
	bool find_web_overview(int * pos);
	//�Ŷϼ��
	bool check_broken();
	bool find_broken_overview(int *pos);
	//����Ƿ�����
	bool check_red();
	//����Ƿ�ִ������Կ�,�ڿռ�ҳ��
	bool check_red_withstand(int *pos=nullptr);
	//�Կ��߼�
	int withstand(int s);
	//����Ѫ
	bool check_hp();
	//��Ѫ����
	int low_hp_process(int s=6);
	//����
	int try_repair(int s);
	//���Tethered
	bool check_tethered();
	//���쳣����
	bool open_probe_scanner();
	//�ر��쳣����
	bool close_probe_scanner();
	//�ж��쳣�����Ƿ���
	bool is_opened_probe_scanner();
	//�����쳣
	bool find_exception(int *pos = nullptr);
	//�رյ���
	void close_popup_window();
	//
	bool is_nothing_found();
	//����Ѿ�
	bool check_green();
	//��齨��
	bool check_building(int* pos=nullptr);
	//�����쳣
	void ignore_exception(QPoint pt);
	//
	bool switch_overview_page(QString page);
	//��ʽ�����
	void format_out_put(const QString& i,const QString& color="#ffffff");
	//���״̬�ı�
	bool check_stateMachine(int csm);
	//�û����������ӡ
	void print_state_machine(int cs,int s);
	//�ж��Ƿ��ǵ�
	bool is_an_egg();
signals:
	void start();
	void stop_timer();
private slots:
	void rerun_timeout();
	void stop_timer_slot();
private:
	int m_state=0;
	int m_stateMachine = 0;	/* 0-����ˢ�� 1-�������֮��ؿռ�վ 2-��ȫ�˳� 3-��ͣ 4-�ͷſ���Ȩ*/
	int m_normalState =-1;
	bool m_isRunning=false;
	IWnd * m_wnd =nullptr;
	int m_errorCode =0;
	RoleConfigure m_configure;
	QTimer *m_timer=nullptr;
	QVector<quint64> m_redtime;	//�����ʱ��
	QMutex m_tMutex;
};

