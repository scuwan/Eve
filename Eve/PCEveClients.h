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
		Unauthorized = 0,	//没有授权的EVE角色
		NotReady,			//没有加入待启动脚本区的角色
		Ready,				//加入待启动脚本区的角色
		Active,				//正在执行脚本程序的角色
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

