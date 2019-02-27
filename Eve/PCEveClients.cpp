#include "PCEveClients.h"



bool PCEveClients::AddEveRoles(QString name, ClientStates s)
{
	if (m_roles.contains(name))
	{
		if (m_roles[name] != s)
		{
			m_roles[name] = s;
			emit EveRoleState(name, s);
		}
		return true;
	}
	else
	{
		m_roles.insert(name, s);
		m_roleNames.append(name);
		return true;
	}
}

bool PCEveClients::DelEveRoles(QString name)
{
	if (m_roles.contains(name))
	{
		m_roles.remove(name);
		m_roleNames.removeOne(name);
		emit DelRole(name);
		return true;
	}
	else
		return false;
}

bool PCEveClients::AlterEveRolesState(QString name,ClientStates s)
{
	if (m_roles.contains(name))
	{
		m_roles[name] = s;
		emit EveRoleState(name, s);
		return true;
	}
	else
		return false;
}

PCEveClients::ClientStates PCEveClients::GetRolesState(QString name)
{
	if (m_roles.contains(name))
	{
		return m_roles[name];
	}
	else
	{
		return PCEveClients::ClientStates::Undefined;
	}
}

QStringList PCEveClients::GetRoles()
{
	return m_roleNames;
}

PCEveClients::PCEveClients()
{
}


PCEveClients::~PCEveClients()
{
}

PCEveClients *PCEveClients::Instance()
{
	static PCEveClients s;
	return &s;
}