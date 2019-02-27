#pragma once
#include<QMap>
#include<QObject>
class PCEveClients: public QObject
{
	Q_OBJECT
public:
	static PCEveClients* Instance();
	enum  ClientStates
	{
		Unauthorized = 0,	//û����Ȩ��EVE��ɫ
		NotReady,			//û�м���������ű����Ľ�ɫ
		Ready,				//����������ű����Ľ�ɫ
		Active,				//����ִ�нű�����Ľ�ɫ
		Undefined = -1
	};
	bool AddEveRoles(QString name, ClientStates s = Unauthorized);
	bool DelEveRoles(QString name);
	bool AlterEveRolesState(QString name,ClientStates s);
	ClientStates GetRolesState(QString name);
	QStringList GetRoles();
public:
signals:
	void EveRoleState(QString name, int);
	void DelRole(QString name);
protected:
	PCEveClients();
	~PCEveClients();
private:
	QMap<QString, ClientStates> m_roles;
	QStringList m_roleNames;
};

