////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Bingfeng Zhao . All rights reserved.
// 

#include <sstream>
#include <cassert>
#include "soapVimBindingProxy.h"
#include "VimBinding.nsmap"
#include "vim_wrapper.h"

namespace vim
{
binding_proxy::binding_proxy(const char* ip, bool use_ssl) : _binding_proxy(new VimBindingProxy)
{
	std::stringstream ss;
	ss << (use_ssl? std::string("https") : std::string("http"))
		<< "://" << ip << "/sdk" << std::ends;
	_url = ss.str();
	_binding_proxy->soap_endpoint = _url.c_str();
	if (use_ssl)
	{
		soap_ssl_init();
		if (soap_ssl_client_context(_binding_proxy, SOAP_SSL_NO_AUTHENTICATION,
			NULL, NULL, NULL, NULL, NULL))
		{
			std::ostringstream oss;
			soap_stream_fault(_binding_proxy, oss);
			throw oss.str().c_str();
		}
	}
}

binding_proxy::~binding_proxy()
{
	delete _binding_proxy;
}

mor_handle::mor_handle(vw1__ManagedObjectReference* mor) : _mor(mor) {}
mor_handle::~mor_handle() {}

template<typename REQ, typename RSP, typename OBJ, typename PROXY>
bool proxy_call_defs(REQ& req, RSP& rsp, OBJ& obj, int (PROXY::*pfm)(REQ*, RSP*))
{
	VimBindingProxy* p = VimInstance().proxy()->operator VimBindingProxy*();
	if ((p->*pfm)(&req, &rsp) == SOAP_OK)
	{
		obj.set_last_error();
		return true;
	}
	else
	{
		obj.set_last_error(p->soap_fault_detail());
		return false;
	}
}

const char* AlarmManager::type = "AlarmManager";
AlarmManager::AlarmManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
AlarmManager::~AlarmManager() {}

void AlarmManager::AcknowledgeAlarm(Alarm alarm, ManagedEntity entity)
{
	vw1__AcknowledgeAlarmRequestType req;
	req._USCOREthis = _mor;
	req.alarm = alarm;
	req.entity = entity;

	// call_defs AcknowledgeAlarm to post the request to ESX server or virtual center
	_vw1__AcknowledgeAlarmResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AcknowledgeAlarm))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

bool AlarmManager::AreAlarmActionsEnabled(ManagedEntity entity)
{
	vw1__AreAlarmActionsEnabledRequestType req;
	req._USCOREthis = _mor;
	req.entity = entity;

	// call_defs AreAlarmActionsEnabled to post the request to ESX server or virtual center
	_vw1__AreAlarmActionsEnabledResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AreAlarmActionsEnabled))
	{
		return static_cast<bool>(rsp.returnval);
	}

	throw get_last_error();
}

Alarm AlarmManager::CreateAlarm(ManagedEntity entity, vw1__AlarmSpec*  spec)
{
	vw1__CreateAlarmRequestType req;
	req._USCOREthis = _mor;
	req.entity = entity;
	req.spec = spec;

	// call_defs CreateAlarm to post the request to ESX server or virtual center
	_vw1__CreateAlarmResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateAlarm))
	{
		return Alarm(rsp.returnval);
	}

	throw get_last_error();
}

void AlarmManager::EnableAlarmActions(ManagedEntity entity, bool enabled)
{
	vw1__EnableAlarmActionsRequestType req;
	req._USCOREthis = _mor;
	req.entity = entity;
	req.enabled = enabled;

	// call_defs EnableAlarmActions to post the request to ESX server or virtual center
	_vw1__EnableAlarmActionsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::EnableAlarmActions))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

std::vector<Alarm> AlarmManager::GetAlarm(ManagedEntity entity)
{
	vw1__GetAlarmRequestType req;
	req._USCOREthis = _mor;
	req.entity = entity;

	// call_defs GetAlarm to post the request to ESX server or virtual center
	_vw1__GetAlarmResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::GetAlarm))
	{
		std::vector<Alarm> alarms;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			Alarm tmp(rsp.returnval[i]);
			alarms.push_back(tmp);
		}
		return alarms;
	}

	throw get_last_error();
}

std::vector<vw1__AlarmState> AlarmManager::GetAlarmState(ManagedEntity entity)
{
	vw1__GetAlarmStateRequestType req;
	req._USCOREthis = _mor;
	req.entity = entity;

	// call_defs GetAlarmState to post the request to ESX server or virtual center
	_vw1__GetAlarmStateResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::GetAlarmState))
	{
		std::vector<vw1__AlarmState> vw1__alarmstates;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__AlarmState tmp(*rsp.returnval[i]);
			vw1__alarmstates.push_back(tmp);
		}
		return vw1__alarmstates;
	}

	throw get_last_error();
}
std::vector<vw1__AlarmExpression> AlarmManager::get_defaultExpression() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("defaultExpression", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__AlarmExpression>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfAlarmExpression* amo = dynamic_cast<vw1__ArrayOfAlarmExpression*>(it->val);
	std::vector<vw1__AlarmExpression> ret;
	for (int i = 0; i < amo->__sizeAlarmExpression; ++i)
		ret.push_back(*amo->AlarmExpression[i]);
	return ret;
}

vw1__AlarmDescription AlarmManager::get_description() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("description", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__AlarmDescription();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__AlarmDescription*>(it->val);
}


const char* AuthorizationManager::type = "AuthorizationManager";
AuthorizationManager::AuthorizationManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
AuthorizationManager::~AuthorizationManager() {}

int AuthorizationManager::AddAuthorizationRole(char* name, int sizeprivIds, char** privIds)
{
	vw1__AddAuthorizationRoleRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.__sizeprivIds = sizeprivIds;
	req.privIds = privIds;

	// call_defs AddAuthorizationRole to post the request to ESX server or virtual center
	_vw1__AddAuthorizationRoleResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AddAuthorizationRole))
	{
		return static_cast<int>(rsp.returnval);
	}

	throw get_last_error();
}

std::vector<bool> AuthorizationManager::HasPrivilegeOnEntity(ManagedEntity entity, char* sessionId, int sizeprivId, char** privId)
{
	vw1__HasPrivilegeOnEntityRequestType req;
	req._USCOREthis = _mor;
	req.entity = entity;
	req.sessionId = sessionId;
	req.__sizeprivId = sizeprivId;
	req.privId = privId;

	// call_defs HasPrivilegeOnEntity to post the request to ESX server or virtual center
	_vw1__HasPrivilegeOnEntityResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::HasPrivilegeOnEntity))
	{
		std::vector<bool> bools;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			bool tmp(rsp.returnval[i]);
			bools.push_back(tmp);
		}
		return bools;
	}

	throw get_last_error();
}

void AuthorizationManager::MergePermissions(int srcRoleId, int dstRoleId)
{
	vw1__MergePermissionsRequestType req;
	req._USCOREthis = _mor;
	req.srcRoleId = srcRoleId;
	req.dstRoleId = dstRoleId;

	// call_defs MergePermissions to post the request to ESX server or virtual center
	_vw1__MergePermissionsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MergePermissions))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void AuthorizationManager::RemoveAuthorizationRole(int roleId, bool failIfUsed)
{
	vw1__RemoveAuthorizationRoleRequestType req;
	req._USCOREthis = _mor;
	req.roleId = roleId;
	req.failIfUsed = failIfUsed;

	// call_defs RemoveAuthorizationRole to post the request to ESX server or virtual center
	_vw1__RemoveAuthorizationRoleResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemoveAuthorizationRole))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void AuthorizationManager::RemoveEntityPermission(ManagedEntity entity, char* user, bool isGroup)
{
	vw1__RemoveEntityPermissionRequestType req;
	req._USCOREthis = _mor;
	req.entity = entity;
	req.user = user;
	req.isGroup = isGroup;

	// call_defs RemoveEntityPermission to post the request to ESX server or virtual center
	_vw1__RemoveEntityPermissionResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemoveEntityPermission))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void AuthorizationManager::ResetEntityPermissions(ManagedEntity entity, int sizepermission, vw1__Permission**  permission)
{
	vw1__ResetEntityPermissionsRequestType req;
	req._USCOREthis = _mor;
	req.entity = entity;
	req.__sizepermission = sizepermission;
	req.permission = permission;

	// call_defs ResetEntityPermissions to post the request to ESX server or virtual center
	_vw1__ResetEntityPermissionsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ResetEntityPermissions))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

std::vector<vw1__Permission> AuthorizationManager::RetrieveAllPermissions()
{
	vw1__RetrieveAllPermissionsRequestType req;
	req._USCOREthis = _mor;

	// call_defs RetrieveAllPermissions to post the request to ESX server or virtual center
	_vw1__RetrieveAllPermissionsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RetrieveAllPermissions))
	{
		std::vector<vw1__Permission> vw1__permissions;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__Permission tmp(*rsp.returnval[i]);
			vw1__permissions.push_back(tmp);
		}
		return vw1__permissions;
	}

	throw get_last_error();
}

std::vector<vw1__Permission> AuthorizationManager::RetrieveEntityPermissions(ManagedEntity entity, bool inherited)
{
	vw1__RetrieveEntityPermissionsRequestType req;
	req._USCOREthis = _mor;
	req.entity = entity;
	req.inherited = inherited;

	// call_defs RetrieveEntityPermissions to post the request to ESX server or virtual center
	_vw1__RetrieveEntityPermissionsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RetrieveEntityPermissions))
	{
		std::vector<vw1__Permission> vw1__permissions;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__Permission tmp(*rsp.returnval[i]);
			vw1__permissions.push_back(tmp);
		}
		return vw1__permissions;
	}

	throw get_last_error();
}

std::vector<vw1__Permission> AuthorizationManager::RetrieveRolePermissions(int roleId)
{
	vw1__RetrieveRolePermissionsRequestType req;
	req._USCOREthis = _mor;
	req.roleId = roleId;

	// call_defs RetrieveRolePermissions to post the request to ESX server or virtual center
	_vw1__RetrieveRolePermissionsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RetrieveRolePermissions))
	{
		std::vector<vw1__Permission> vw1__permissions;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__Permission tmp(*rsp.returnval[i]);
			vw1__permissions.push_back(tmp);
		}
		return vw1__permissions;
	}

	throw get_last_error();
}

void AuthorizationManager::SetEntityPermissions(ManagedEntity entity, int sizepermission, vw1__Permission**  permission)
{
	vw1__SetEntityPermissionsRequestType req;
	req._USCOREthis = _mor;
	req.entity = entity;
	req.__sizepermission = sizepermission;
	req.permission = permission;

	// call_defs SetEntityPermissions to post the request to ESX server or virtual center
	_vw1__SetEntityPermissionsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SetEntityPermissions))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void AuthorizationManager::UpdateAuthorizationRole(int roleId, char* newName, int sizeprivIds, char** privIds)
{
	vw1__UpdateAuthorizationRoleRequestType req;
	req._USCOREthis = _mor;
	req.roleId = roleId;
	req.newName = newName;
	req.__sizeprivIds = sizeprivIds;
	req.privIds = privIds;

	// call_defs UpdateAuthorizationRole to post the request to ESX server or virtual center
	_vw1__UpdateAuthorizationRoleResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateAuthorizationRole))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__AuthorizationDescription AuthorizationManager::get_description() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("description", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__AuthorizationDescription();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__AuthorizationDescription*>(it->val);
}

std::vector<vw1__AuthorizationPrivilege> AuthorizationManager::get_privilegeList() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("privilegeList", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__AuthorizationPrivilege>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfAuthorizationPrivilege* amo = dynamic_cast<vw1__ArrayOfAuthorizationPrivilege*>(it->val);
	std::vector<vw1__AuthorizationPrivilege> ret;
	for (int i = 0; i < amo->__sizeAuthorizationPrivilege; ++i)
		ret.push_back(*amo->AuthorizationPrivilege[i]);
	return ret;
}

std::vector<vw1__AuthorizationRole> AuthorizationManager::get_roleList() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("roleList", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__AuthorizationRole>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfAuthorizationRole* amo = dynamic_cast<vw1__ArrayOfAuthorizationRole*>(it->val);
	std::vector<vw1__AuthorizationRole> ret;
	for (int i = 0; i < amo->__sizeAuthorizationRole; ++i)
		ret.push_back(*amo->AuthorizationRole[i]);
	return ret;
}


const char* CustomFieldsManager::type = "CustomFieldsManager";
CustomFieldsManager::CustomFieldsManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
CustomFieldsManager::~CustomFieldsManager() {}

vw1__CustomFieldDef CustomFieldsManager::AddCustomFieldDef(char* name, char* moType, vw1__PrivilegePolicyDef*  fieldDefPolicy, vw1__PrivilegePolicyDef*  fieldPolicy)
{
	vw1__AddCustomFieldDefRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.moType = moType;
	req.fieldDefPolicy = fieldDefPolicy;
	req.fieldPolicy = fieldPolicy;

	// call_defs AddCustomFieldDef to post the request to ESX server or virtual center
	_vw1__AddCustomFieldDefResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AddCustomFieldDef))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

void CustomFieldsManager::RemoveCustomFieldDef(int key)
{
	vw1__RemoveCustomFieldDefRequestType req;
	req._USCOREthis = _mor;
	req.key = key;

	// call_defs RemoveCustomFieldDef to post the request to ESX server or virtual center
	_vw1__RemoveCustomFieldDefResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemoveCustomFieldDef))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void CustomFieldsManager::RenameCustomFieldDef(int key, char* name)
{
	vw1__RenameCustomFieldDefRequestType req;
	req._USCOREthis = _mor;
	req.key = key;
	req.name = name;

	// call_defs RenameCustomFieldDef to post the request to ESX server or virtual center
	_vw1__RenameCustomFieldDefResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RenameCustomFieldDef))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void CustomFieldsManager::SetField(ManagedEntity entity, int key, char* value)
{
	vw1__SetFieldRequestType req;
	req._USCOREthis = _mor;
	req.entity = entity;
	req.key = key;
	req.value = value;

	// call_defs SetField to post the request to ESX server or virtual center
	_vw1__SetFieldResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SetField))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
std::vector<vw1__CustomFieldDef> CustomFieldsManager::get_field() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("field", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__CustomFieldDef>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfCustomFieldDef* amo = dynamic_cast<vw1__ArrayOfCustomFieldDef*>(it->val);
	std::vector<vw1__CustomFieldDef> ret;
	for (int i = 0; i < amo->__sizeCustomFieldDef; ++i)
		ret.push_back(*amo->CustomFieldDef[i]);
	return ret;
}


const char* CustomizationSpecManager::type = "CustomizationSpecManager";
CustomizationSpecManager::CustomizationSpecManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
CustomizationSpecManager::~CustomizationSpecManager() {}

void CustomizationSpecManager::CheckCustomizationResources(char* guestOs)
{
	vw1__CheckCustomizationResourcesRequestType req;
	req._USCOREthis = _mor;
	req.guestOs = guestOs;

	// call_defs CheckCustomizationResources to post the request to ESX server or virtual center
	_vw1__CheckCustomizationResourcesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CheckCustomizationResources))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void CustomizationSpecManager::CreateCustomizationSpec(vw1__CustomizationSpecItem*  item)
{
	vw1__CreateCustomizationSpecRequestType req;
	req._USCOREthis = _mor;
	req.item = item;

	// call_defs CreateCustomizationSpec to post the request to ESX server or virtual center
	_vw1__CreateCustomizationSpecResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateCustomizationSpec))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

std::string CustomizationSpecManager::CustomizationSpecItemToXml(vw1__CustomizationSpecItem*  item)
{
	vw1__CustomizationSpecItemToXmlRequestType req;
	req._USCOREthis = _mor;
	req.item = item;

	// call_defs CustomizationSpecItemToXml to post the request to ESX server or virtual center
	_vw1__CustomizationSpecItemToXmlResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CustomizationSpecItemToXml))
	{
		return static_cast<std::string>(rsp.returnval);
	}

	throw get_last_error();
}

void CustomizationSpecManager::DeleteCustomizationSpec(char* name)
{
	vw1__DeleteCustomizationSpecRequestType req;
	req._USCOREthis = _mor;
	req.name = name;

	// call_defs DeleteCustomizationSpec to post the request to ESX server or virtual center
	_vw1__DeleteCustomizationSpecResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DeleteCustomizationSpec))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

bool CustomizationSpecManager::DoesCustomizationSpecExist(char* name)
{
	vw1__DoesCustomizationSpecExistRequestType req;
	req._USCOREthis = _mor;
	req.name = name;

	// call_defs DoesCustomizationSpecExist to post the request to ESX server or virtual center
	_vw1__DoesCustomizationSpecExistResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DoesCustomizationSpecExist))
	{
		return static_cast<bool>(rsp.returnval);
	}

	throw get_last_error();
}

void CustomizationSpecManager::DuplicateCustomizationSpec(char* name, char* newName)
{
	vw1__DuplicateCustomizationSpecRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.newName = newName;

	// call_defs DuplicateCustomizationSpec to post the request to ESX server or virtual center
	_vw1__DuplicateCustomizationSpecResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DuplicateCustomizationSpec))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

vw1__CustomizationSpecItem CustomizationSpecManager::GetCustomizationSpec(char* name)
{
	vw1__GetCustomizationSpecRequestType req;
	req._USCOREthis = _mor;
	req.name = name;

	// call_defs GetCustomizationSpec to post the request to ESX server or virtual center
	_vw1__GetCustomizationSpecResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::GetCustomizationSpec))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

void CustomizationSpecManager::OverwriteCustomizationSpec(vw1__CustomizationSpecItem*  item)
{
	vw1__OverwriteCustomizationSpecRequestType req;
	req._USCOREthis = _mor;
	req.item = item;

	// call_defs OverwriteCustomizationSpec to post the request to ESX server or virtual center
	_vw1__OverwriteCustomizationSpecResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::OverwriteCustomizationSpec))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void CustomizationSpecManager::RenameCustomizationSpec(char* name, char* newName)
{
	vw1__RenameCustomizationSpecRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.newName = newName;

	// call_defs RenameCustomizationSpec to post the request to ESX server or virtual center
	_vw1__RenameCustomizationSpecResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RenameCustomizationSpec))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

vw1__CustomizationSpecItem CustomizationSpecManager::XmlToCustomizationSpecItem(char* specItemXml)
{
	vw1__XmlToCustomizationSpecItemRequestType req;
	req._USCOREthis = _mor;
	req.specItemXml = specItemXml;

	// call_defs XmlToCustomizationSpecItem to post the request to ESX server or virtual center
	_vw1__XmlToCustomizationSpecItemResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::XmlToCustomizationSpecItem))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}
std::vector<byte> CustomizationSpecManager::get_encryptionKey() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("encryptionKey", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<byte>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfByte* amo = dynamic_cast<vw1__ArrayOfByte*>(it->val);
	std::vector<byte> ret;
	for (int i = 0; i < amo->__sizebyte; ++i)
		ret.push_back(amo->byte[i]);
	return ret;
}

std::vector<vw1__CustomizationSpecInfo> CustomizationSpecManager::get_info() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("info", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__CustomizationSpecInfo>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfCustomizationSpecInfo* amo = dynamic_cast<vw1__ArrayOfCustomizationSpecInfo*>(it->val);
	std::vector<vw1__CustomizationSpecInfo> ret;
	for (int i = 0; i < amo->__sizeCustomizationSpecInfo; ++i)
		ret.push_back(*amo->CustomizationSpecInfo[i]);
	return ret;
}


const char* DiagnosticManager::type = "DiagnosticManager";
DiagnosticManager::DiagnosticManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
DiagnosticManager::~DiagnosticManager() {}

vw1__DiagnosticManagerLogHeader DiagnosticManager::BrowseDiagnosticLog(HostSystem host, char* key, int*  start, int*  lines)
{
	vw1__BrowseDiagnosticLogRequestType req;
	req._USCOREthis = _mor;
	req.host = host;
	req.key = key;
	req.start = start;
	req.lines = lines;

	// call_defs BrowseDiagnosticLog to post the request to ESX server or virtual center
	_vw1__BrowseDiagnosticLogResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::BrowseDiagnosticLog))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

Task DiagnosticManager::GenerateLogBundles_Task(bool includeDefault, int sizehost, HostSystem* host)
{
	vw1__GenerateLogBundlesRequestType req;
	req._USCOREthis = _mor;
	req.includeDefault = includeDefault;
	req.__sizehost = sizehost;
	vw1__ManagedObjectReference ** phost = new vw1__ManagedObjectReference *[sizehost];
	for (int i = 0; i < sizehost; ++i)
		phost[i] = host[i];
	req.host = phost;

	// call_defs GenerateLogBundles_USCORETask to post the request to ESX server or virtual center
	_vw1__GenerateLogBundles_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::GenerateLogBundles_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

std::vector<vw1__DiagnosticManagerLogDescriptor> DiagnosticManager::QueryDescriptions(HostSystem host)
{
	vw1__QueryDescriptionsRequestType req;
	req._USCOREthis = _mor;
	req.host = host;

	// call_defs QueryDescriptions to post the request to ESX server or virtual center
	_vw1__QueryDescriptionsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryDescriptions))
	{
		std::vector<vw1__DiagnosticManagerLogDescriptor> vw1__diagnosticmanagerlogdescriptors;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__DiagnosticManagerLogDescriptor tmp(*rsp.returnval[i]);
			vw1__diagnosticmanagerlogdescriptors.push_back(tmp);
		}
		return vw1__diagnosticmanagerlogdescriptors;
	}

	throw get_last_error();
}

const char* DistributedVirtualSwitchManager::type = "DistributedVirtualSwitchManager";
DistributedVirtualSwitchManager::DistributedVirtualSwitchManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
DistributedVirtualSwitchManager::~DistributedVirtualSwitchManager() {}

std::vector<vw1__DistributedVirtualSwitchProductSpec> DistributedVirtualSwitchManager::QueryAvailableDvsSpec()
{
	vw1__QueryAvailableDvsSpecRequestType req;
	req._USCOREthis = _mor;

	// call_defs QueryAvailableDvsSpec to post the request to ESX server or virtual center
	_vw1__QueryAvailableDvsSpecResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryAvailableDvsSpec))
	{
		std::vector<vw1__DistributedVirtualSwitchProductSpec> vw1__distributedvirtualswitchproductspecs;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__DistributedVirtualSwitchProductSpec tmp(*rsp.returnval[i]);
			vw1__distributedvirtualswitchproductspecs.push_back(tmp);
		}
		return vw1__distributedvirtualswitchproductspecs;
	}

	throw get_last_error();
}

std::vector<HostSystem> DistributedVirtualSwitchManager::QueryCompatibleHostForExistingDvs(ManagedEntity container, bool recursive, DistributedVirtualSwitch dvs)
{
	vw1__QueryCompatibleHostForExistingDvsRequestType req;
	req._USCOREthis = _mor;
	req.container = container;
	req.recursive = recursive;
	req.dvs = dvs;

	// call_defs QueryCompatibleHostForExistingDvs to post the request to ESX server or virtual center
	_vw1__QueryCompatibleHostForExistingDvsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryCompatibleHostForExistingDvs))
	{
		std::vector<HostSystem> hostsystems;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			HostSystem tmp(rsp.returnval[i]);
			hostsystems.push_back(tmp);
		}
		return hostsystems;
	}

	throw get_last_error();
}

std::vector<HostSystem> DistributedVirtualSwitchManager::QueryCompatibleHostForNewDvs(ManagedEntity container, bool recursive, vw1__DistributedVirtualSwitchProductSpec*  switchProductSpec)
{
	vw1__QueryCompatibleHostForNewDvsRequestType req;
	req._USCOREthis = _mor;
	req.container = container;
	req.recursive = recursive;
	req.switchProductSpec = switchProductSpec;

	// call_defs QueryCompatibleHostForNewDvs to post the request to ESX server or virtual center
	_vw1__QueryCompatibleHostForNewDvsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryCompatibleHostForNewDvs))
	{
		std::vector<HostSystem> hostsystems;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			HostSystem tmp(rsp.returnval[i]);
			hostsystems.push_back(tmp);
		}
		return hostsystems;
	}

	throw get_last_error();
}

DistributedVirtualSwitch DistributedVirtualSwitchManager::QueryDvsByUuid(char* uuid)
{
	vw1__QueryDvsByUuidRequestType req;
	req._USCOREthis = _mor;
	req.uuid = uuid;

	// call_defs QueryDvsByUuid to post the request to ESX server or virtual center
	_vw1__QueryDvsByUuidResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryDvsByUuid))
	{
		return DistributedVirtualSwitch(rsp.returnval);
	}

	throw get_last_error();
}

std::vector<vw1__DistributedVirtualSwitchManagerCompatibilityResult> DistributedVirtualSwitchManager::QueryDvsCheckCompatibility(vw1__DistributedVirtualSwitchManagerHostContainer*  hostContainer, vw1__DistributedVirtualSwitchManagerDvsProductSpec*  dvsProductSpec, int sizehostFilterSpec, vw1__DistributedVirtualSwitchManagerHostDvsFilterSpec**  hostFilterSpec)
{
	vw1__QueryDvsCheckCompatibilityRequestType req;
	req._USCOREthis = _mor;
	req.hostContainer = hostContainer;
	req.dvsProductSpec = dvsProductSpec;
	req.__sizehostFilterSpec = sizehostFilterSpec;
	req.hostFilterSpec = hostFilterSpec;

	// call_defs QueryDvsCheckCompatibility to post the request to ESX server or virtual center
	_vw1__QueryDvsCheckCompatibilityResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryDvsCheckCompatibility))
	{
		std::vector<vw1__DistributedVirtualSwitchManagerCompatibilityResult> vw1__distributedvirtualswitchmanagercompatibilityresults;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__DistributedVirtualSwitchManagerCompatibilityResult tmp(*rsp.returnval[i]);
			vw1__distributedvirtualswitchmanagercompatibilityresults.push_back(tmp);
		}
		return vw1__distributedvirtualswitchmanagercompatibilityresults;
	}

	throw get_last_error();
}

std::vector<vw1__DistributedVirtualSwitchHostProductSpec> DistributedVirtualSwitchManager::QueryDvsCompatibleHostSpec(vw1__DistributedVirtualSwitchProductSpec*  switchProductSpec)
{
	vw1__QueryDvsCompatibleHostSpecRequestType req;
	req._USCOREthis = _mor;
	req.switchProductSpec = switchProductSpec;

	// call_defs QueryDvsCompatibleHostSpec to post the request to ESX server or virtual center
	_vw1__QueryDvsCompatibleHostSpecResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryDvsCompatibleHostSpec))
	{
		std::vector<vw1__DistributedVirtualSwitchHostProductSpec> vw1__distributedvirtualswitchhostproductspecs;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__DistributedVirtualSwitchHostProductSpec tmp(*rsp.returnval[i]);
			vw1__distributedvirtualswitchhostproductspecs.push_back(tmp);
		}
		return vw1__distributedvirtualswitchhostproductspecs;
	}

	throw get_last_error();
}

vw1__DVSManagerDvsConfigTarget DistributedVirtualSwitchManager::QueryDvsConfigTarget(HostSystem host, DistributedVirtualSwitch dvs)
{
	vw1__QueryDvsConfigTargetRequestType req;
	req._USCOREthis = _mor;
	req.host = host;
	req.dvs = dvs;

	// call_defs QueryDvsConfigTarget to post the request to ESX server or virtual center
	_vw1__QueryDvsConfigTargetResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryDvsConfigTarget))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__DVSFeatureCapability DistributedVirtualSwitchManager::QueryDvsFeatureCapability(vw1__DistributedVirtualSwitchProductSpec*  switchProductSpec)
{
	vw1__QueryDvsFeatureCapabilityRequestType req;
	req._USCOREthis = _mor;
	req.switchProductSpec = switchProductSpec;

	// call_defs QueryDvsFeatureCapability to post the request to ESX server or virtual center
	_vw1__QueryDvsFeatureCapabilityResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryDvsFeatureCapability))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

Task DistributedVirtualSwitchManager::RectifyDvsOnHost_Task(int sizehosts, HostSystem* hosts)
{
	vw1__RectifyDvsOnHostRequestType req;
	req._USCOREthis = _mor;
	req.__sizehosts = sizehosts;
	vw1__ManagedObjectReference ** phosts = new vw1__ManagedObjectReference *[sizehosts];
	for (int i = 0; i < sizehosts; ++i)
		phosts[i] = hosts[i];
	req.hosts = phosts;

	// call_defs RectifyDvsOnHost_USCORETask to post the request to ESX server or virtual center
	_vw1__RectifyDvsOnHost_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RectifyDvsOnHost_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

const char* EnvironmentBrowser::type = "EnvironmentBrowser";
EnvironmentBrowser::EnvironmentBrowser(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
EnvironmentBrowser::~EnvironmentBrowser() {}

vw1__VirtualMachineConfigOption EnvironmentBrowser::QueryConfigOption(char* key, HostSystem host)
{
	vw1__QueryConfigOptionRequestType req;
	req._USCOREthis = _mor;
	req.key = key;
	req.host = host;

	// call_defs QueryConfigOption to post the request to ESX server or virtual center
	_vw1__QueryConfigOptionResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryConfigOption))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

std::vector<vw1__VirtualMachineConfigOptionDescriptor> EnvironmentBrowser::QueryConfigOptionDescriptor()
{
	vw1__QueryConfigOptionDescriptorRequestType req;
	req._USCOREthis = _mor;

	// call_defs QueryConfigOptionDescriptor to post the request to ESX server or virtual center
	_vw1__QueryConfigOptionDescriptorResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryConfigOptionDescriptor))
	{
		std::vector<vw1__VirtualMachineConfigOptionDescriptor> vw1__virtualmachineconfigoptiondescriptors;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__VirtualMachineConfigOptionDescriptor tmp(*rsp.returnval[i]);
			vw1__virtualmachineconfigoptiondescriptors.push_back(tmp);
		}
		return vw1__virtualmachineconfigoptiondescriptors;
	}

	throw get_last_error();
}

vw1__ConfigTarget EnvironmentBrowser::QueryConfigTarget(HostSystem host)
{
	vw1__QueryConfigTargetRequestType req;
	req._USCOREthis = _mor;
	req.host = host;

	// call_defs QueryConfigTarget to post the request to ESX server or virtual center
	_vw1__QueryConfigTargetResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryConfigTarget))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__HostCapability EnvironmentBrowser::QueryTargetCapabilities(HostSystem host)
{
	vw1__QueryTargetCapabilitiesRequestType req;
	req._USCOREthis = _mor;
	req.host = host;

	// call_defs QueryTargetCapabilities to post the request to ESX server or virtual center
	_vw1__QueryTargetCapabilitiesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryTargetCapabilities))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}
HostDatastoreBrowser EnvironmentBrowser::get_datastoreBrowser() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("datastoreBrowser", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return HostDatastoreBrowser();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return HostDatastoreBrowser(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}


const char* EventManager::type = "EventManager";
EventManager::EventManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
EventManager::~EventManager() {}

EventHistoryCollector EventManager::CreateCollectorForEvents(vw1__EventFilterSpec*  filter)
{
	vw1__CreateCollectorForEventsRequestType req;
	req._USCOREthis = _mor;
	req.filter = filter;

	// call_defs CreateCollectorForEvents to post the request to ESX server or virtual center
	_vw1__CreateCollectorForEventsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateCollectorForEvents))
	{
		return EventHistoryCollector(rsp.returnval);
	}

	throw get_last_error();
}

void EventManager::LogUserEvent(ManagedEntity entity, char* msg)
{
	vw1__LogUserEventRequestType req;
	req._USCOREthis = _mor;
	req.entity = entity;
	req.msg = msg;

	// call_defs LogUserEvent to post the request to ESX server or virtual center
	_vw1__LogUserEventResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::LogUserEvent))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void EventManager::PostEvent(vw1__Event*  eventToPost, vw1__TaskInfo*  taskInfo)
{
	vw1__PostEventRequestType req;
	req._USCOREthis = _mor;
	req.eventToPost = eventToPost;
	req.taskInfo = taskInfo;

	// call_defs PostEvent to post the request to ESX server or virtual center
	_vw1__PostEventResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::PostEvent))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

std::vector<vw1__Event> EventManager::QueryEvents(vw1__EventFilterSpec*  filter)
{
	vw1__QueryEventsRequestType req;
	req._USCOREthis = _mor;
	req.filter = filter;

	// call_defs QueryEvents to post the request to ESX server or virtual center
	_vw1__QueryEventsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryEvents))
	{
		std::vector<vw1__Event> vw1__events;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__Event tmp(*rsp.returnval[i]);
			vw1__events.push_back(tmp);
		}
		return vw1__events;
	}

	throw get_last_error();
}

std::vector<vw1__EventArgDesc> EventManager::RetrieveArgumentDescription(char* eventTypeId)
{
	vw1__RetrieveArgumentDescriptionRequestType req;
	req._USCOREthis = _mor;
	req.eventTypeId = eventTypeId;

	// call_defs RetrieveArgumentDescription to post the request to ESX server or virtual center
	_vw1__RetrieveArgumentDescriptionResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RetrieveArgumentDescription))
	{
		std::vector<vw1__EventArgDesc> vw1__eventargdescs;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__EventArgDesc tmp(*rsp.returnval[i]);
			vw1__eventargdescs.push_back(tmp);
		}
		return vw1__eventargdescs;
	}

	throw get_last_error();
}
vw1__EventDescription EventManager::get_description() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("description", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__EventDescription();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__EventDescription*>(it->val);
}

vw1__Event EventManager::get_latestEvent() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("latestEvent", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__Event();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__Event*>(it->val);
}

int EventManager::get_maxCollector() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("maxCollector", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return int();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<xsd__int*>(it->val)->__item;
}


const char* ExtensibleManagedObject::type = "ExtensibleManagedObject";
ExtensibleManagedObject::ExtensibleManagedObject(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
ExtensibleManagedObject::~ExtensibleManagedObject() {}

void ExtensibleManagedObject::setCustomValue(char* key, char* value)
{
	vw1__setCustomValueRequestType req;
	req._USCOREthis = _mor;
	req.key = key;
	req.value = value;

	// call_defs setCustomValue to post the request to ESX server or virtual center
	_vw1__setCustomValueResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::setCustomValue))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
std::vector<vw1__CustomFieldDef> ExtensibleManagedObject::get_availableField() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("availableField", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__CustomFieldDef>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfCustomFieldDef* amo = dynamic_cast<vw1__ArrayOfCustomFieldDef*>(it->val);
	std::vector<vw1__CustomFieldDef> ret;
	for (int i = 0; i < amo->__sizeCustomFieldDef; ++i)
		ret.push_back(*amo->CustomFieldDef[i]);
	return ret;
}

std::vector<vw1__CustomFieldValue> ExtensibleManagedObject::get_value() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("value", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__CustomFieldValue>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfCustomFieldValue* amo = dynamic_cast<vw1__ArrayOfCustomFieldValue*>(it->val);
	std::vector<vw1__CustomFieldValue> ret;
	for (int i = 0; i < amo->__sizeCustomFieldValue; ++i)
		ret.push_back(*amo->CustomFieldValue[i]);
	return ret;
}


const char* ExtensionManager::type = "ExtensionManager";
ExtensionManager::ExtensionManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
ExtensionManager::~ExtensionManager() {}

vw1__Extension ExtensionManager::FindExtension(char* extensionKey)
{
	vw1__FindExtensionRequestType req;
	req._USCOREthis = _mor;
	req.extensionKey = extensionKey;

	// call_defs FindExtension to post the request to ESX server or virtual center
	_vw1__FindExtensionResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::FindExtension))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

std::string ExtensionManager::GetPublicKey()
{
	vw1__GetPublicKeyRequestType req;
	req._USCOREthis = _mor;

	// call_defs GetPublicKey to post the request to ESX server or virtual center
	_vw1__GetPublicKeyResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::GetPublicKey))
	{
		return static_cast<std::string>(rsp.returnval);
	}

	throw get_last_error();
}

std::vector<ManagedEntity> ExtensionManager::QueryManagedBy(char* extensionKey)
{
	vw1__QueryManagedByRequestType req;
	req._USCOREthis = _mor;
	req.extensionKey = extensionKey;

	// call_defs QueryManagedBy to post the request to ESX server or virtual center
	_vw1__QueryManagedByResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryManagedBy))
	{
		std::vector<ManagedEntity> managedentitys;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			ManagedEntity tmp(rsp.returnval[i]);
			managedentitys.push_back(tmp);
		}
		return managedentitys;
	}

	throw get_last_error();
}

void ExtensionManager::RegisterExtension(vw1__Extension*  extension)
{
	vw1__RegisterExtensionRequestType req;
	req._USCOREthis = _mor;
	req.extension = extension;

	// call_defs RegisterExtension to post the request to ESX server or virtual center
	_vw1__RegisterExtensionResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RegisterExtension))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void ExtensionManager::SetExtensionCertificate(char* extensionKey, char* certificatePem)
{
	vw1__SetExtensionCertificateRequestType req;
	req._USCOREthis = _mor;
	req.extensionKey = extensionKey;
	req.certificatePem = certificatePem;

	// call_defs SetExtensionCertificate to post the request to ESX server or virtual center
	_vw1__SetExtensionCertificateResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SetExtensionCertificate))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void ExtensionManager::SetPublicKey(char* extensionKey, char* publicKey)
{
	vw1__SetPublicKeyRequestType req;
	req._USCOREthis = _mor;
	req.extensionKey = extensionKey;
	req.publicKey = publicKey;

	// call_defs SetPublicKey to post the request to ESX server or virtual center
	_vw1__SetPublicKeyResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SetPublicKey))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void ExtensionManager::UnregisterExtension(char* extensionKey)
{
	vw1__UnregisterExtensionRequestType req;
	req._USCOREthis = _mor;
	req.extensionKey = extensionKey;

	// call_defs UnregisterExtension to post the request to ESX server or virtual center
	_vw1__UnregisterExtensionResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UnregisterExtension))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void ExtensionManager::UpdateExtension(vw1__Extension*  extension)
{
	vw1__UpdateExtensionRequestType req;
	req._USCOREthis = _mor;
	req.extension = extension;

	// call_defs UpdateExtension to post the request to ESX server or virtual center
	_vw1__UpdateExtensionResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateExtension))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
std::vector<vw1__Extension> ExtensionManager::get_extensionList() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("extensionList", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__Extension>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfExtension* amo = dynamic_cast<vw1__ArrayOfExtension*>(it->val);
	std::vector<vw1__Extension> ret;
	for (int i = 0; i < amo->__sizeExtension; ++i)
		ret.push_back(*amo->Extension[i]);
	return ret;
}


const char* FileManager::type = "FileManager";
FileManager::FileManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
FileManager::~FileManager() {}

void FileManager::ChangeOwner(char* name, Datacenter datacenter, char* owner)
{
	vw1__ChangeOwnerRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.datacenter = datacenter;
	req.owner = owner;

	// call_defs ChangeOwner to post the request to ESX server or virtual center
	_vw1__ChangeOwnerResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ChangeOwner))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task FileManager::CopyDatastoreFile_Task(char* sourceName, Datacenter sourceDatacenter, char* destinationName, Datacenter destinationDatacenter, bool*  force)
{
	vw1__CopyDatastoreFileRequestType req;
	req._USCOREthis = _mor;
	req.sourceName = sourceName;
	req.sourceDatacenter = sourceDatacenter;
	req.destinationName = destinationName;
	req.destinationDatacenter = destinationDatacenter;
	req.force = force;

	// call_defs CopyDatastoreFile_USCORETask to post the request to ESX server or virtual center
	_vw1__CopyDatastoreFile_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CopyDatastoreFile_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task FileManager::DeleteDatastoreFile_Task(char* name, Datacenter datacenter)
{
	vw1__DeleteDatastoreFileRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.datacenter = datacenter;

	// call_defs DeleteDatastoreFile_USCORETask to post the request to ESX server or virtual center
	_vw1__DeleteDatastoreFile_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DeleteDatastoreFile_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void FileManager::MakeDirectory(char* name, Datacenter datacenter, bool*  createParentDirectories)
{
	vw1__MakeDirectoryRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.datacenter = datacenter;
	req.createParentDirectories = createParentDirectories;

	// call_defs MakeDirectory to post the request to ESX server or virtual center
	_vw1__MakeDirectoryResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MakeDirectory))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task FileManager::MoveDatastoreFile_Task(char* sourceName, Datacenter sourceDatacenter, char* destinationName, Datacenter destinationDatacenter, bool*  force)
{
	vw1__MoveDatastoreFileRequestType req;
	req._USCOREthis = _mor;
	req.sourceName = sourceName;
	req.sourceDatacenter = sourceDatacenter;
	req.destinationName = destinationName;
	req.destinationDatacenter = destinationDatacenter;
	req.force = force;

	// call_defs MoveDatastoreFile_USCORETask to post the request to ESX server or virtual center
	_vw1__MoveDatastoreFile_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MoveDatastoreFile_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

const char* GuestAuthManager::type = "GuestAuthManager";
GuestAuthManager::GuestAuthManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
GuestAuthManager::~GuestAuthManager() {}

vw1__GuestAuthentication GuestAuthManager::AcquireCredentialsInGuest(VirtualMachine vm, vw1__GuestAuthentication*  requestedAuth, __int64*  sessionID)
{
	vw1__AcquireCredentialsInGuestRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.requestedAuth = requestedAuth;
	req.sessionID = sessionID;

	// call_defs AcquireCredentialsInGuest to post the request to ESX server or virtual center
	_vw1__AcquireCredentialsInGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AcquireCredentialsInGuest))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

void GuestAuthManager::ReleaseCredentialsInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth)
{
	vw1__ReleaseCredentialsInGuestRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.auth = auth;

	// call_defs ReleaseCredentialsInGuest to post the request to ESX server or virtual center
	_vw1__ReleaseCredentialsInGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReleaseCredentialsInGuest))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void GuestAuthManager::ValidateCredentialsInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth)
{
	vw1__ValidateCredentialsInGuestRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.auth = auth;

	// call_defs ValidateCredentialsInGuest to post the request to ESX server or virtual center
	_vw1__ValidateCredentialsInGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ValidateCredentialsInGuest))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

const char* GuestFileManager::type = "GuestFileManager";
GuestFileManager::GuestFileManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
GuestFileManager::~GuestFileManager() {}

void GuestFileManager::ChangeFileAttributesInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* guestFilePath, vw1__GuestFileAttributes*  fileAttributes)
{
	vw1__ChangeFileAttributesInGuestRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.auth = auth;
	req.guestFilePath = guestFilePath;
	req.fileAttributes = fileAttributes;

	// call_defs ChangeFileAttributesInGuest to post the request to ESX server or virtual center
	_vw1__ChangeFileAttributesInGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ChangeFileAttributesInGuest))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

std::string GuestFileManager::CreateTemporaryDirectoryInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* prefix, char* suffix, char* directoryPath)
{
	vw1__CreateTemporaryDirectoryInGuestRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.auth = auth;
	req.prefix = prefix;
	req.suffix = suffix;
	req.directoryPath = directoryPath;

	// call_defs CreateTemporaryDirectoryInGuest to post the request to ESX server or virtual center
	_vw1__CreateTemporaryDirectoryInGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateTemporaryDirectoryInGuest))
	{
		return static_cast<std::string>(rsp.returnval);
	}

	throw get_last_error();
}

std::string GuestFileManager::CreateTemporaryFileInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* prefix, char* suffix, char* directoryPath)
{
	vw1__CreateTemporaryFileInGuestRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.auth = auth;
	req.prefix = prefix;
	req.suffix = suffix;
	req.directoryPath = directoryPath;

	// call_defs CreateTemporaryFileInGuest to post the request to ESX server or virtual center
	_vw1__CreateTemporaryFileInGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateTemporaryFileInGuest))
	{
		return static_cast<std::string>(rsp.returnval);
	}

	throw get_last_error();
}

void GuestFileManager::DeleteDirectoryInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* directoryPath, bool recursive)
{
	vw1__DeleteDirectoryInGuestRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.auth = auth;
	req.directoryPath = directoryPath;
	req.recursive = recursive;

	// call_defs DeleteDirectoryInGuest to post the request to ESX server or virtual center
	_vw1__DeleteDirectoryInGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DeleteDirectoryInGuest))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void GuestFileManager::DeleteFileInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* filePath)
{
	vw1__DeleteFileInGuestRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.auth = auth;
	req.filePath = filePath;

	// call_defs DeleteFileInGuest to post the request to ESX server or virtual center
	_vw1__DeleteFileInGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DeleteFileInGuest))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

vw1__FileTransferInformation GuestFileManager::InitiateFileTransferFromGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* guestFilePath)
{
	vw1__InitiateFileTransferFromGuestRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.auth = auth;
	req.guestFilePath = guestFilePath;

	// call_defs InitiateFileTransferFromGuest to post the request to ESX server or virtual center
	_vw1__InitiateFileTransferFromGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::InitiateFileTransferFromGuest))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

std::string GuestFileManager::InitiateFileTransferToGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* guestFilePath, vw1__GuestFileAttributes*  fileAttributes, __int64 fileSize, bool overwrite)
{
	vw1__InitiateFileTransferToGuestRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.auth = auth;
	req.guestFilePath = guestFilePath;
	req.fileAttributes = fileAttributes;
	req.fileSize = fileSize;
	req.overwrite = overwrite;

	// call_defs InitiateFileTransferToGuest to post the request to ESX server or virtual center
	_vw1__InitiateFileTransferToGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::InitiateFileTransferToGuest))
	{
		return static_cast<std::string>(rsp.returnval);
	}

	throw get_last_error();
}

vw1__GuestListFileInfo GuestFileManager::ListFilesInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* filePath, int*  index, int*  maxResults, char* matchPattern)
{
	vw1__ListFilesInGuestRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.auth = auth;
	req.filePath = filePath;
	req.index = index;
	req.maxResults = maxResults;
	req.matchPattern = matchPattern;

	// call_defs ListFilesInGuest to post the request to ESX server or virtual center
	_vw1__ListFilesInGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ListFilesInGuest))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

void GuestFileManager::MakeDirectoryInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* directoryPath, bool createParentDirectories)
{
	vw1__MakeDirectoryInGuestRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.auth = auth;
	req.directoryPath = directoryPath;
	req.createParentDirectories = createParentDirectories;

	// call_defs MakeDirectoryInGuest to post the request to ESX server or virtual center
	_vw1__MakeDirectoryInGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MakeDirectoryInGuest))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void GuestFileManager::MoveDirectoryInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* srcDirectoryPath, char* dstDirectoryPath)
{
	vw1__MoveDirectoryInGuestRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.auth = auth;
	req.srcDirectoryPath = srcDirectoryPath;
	req.dstDirectoryPath = dstDirectoryPath;

	// call_defs MoveDirectoryInGuest to post the request to ESX server or virtual center
	_vw1__MoveDirectoryInGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MoveDirectoryInGuest))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void GuestFileManager::MoveFileInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* srcFilePath, char* dstFilePath, bool overwrite)
{
	vw1__MoveFileInGuestRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.auth = auth;
	req.srcFilePath = srcFilePath;
	req.dstFilePath = dstFilePath;
	req.overwrite = overwrite;

	// call_defs MoveFileInGuest to post the request to ESX server or virtual center
	_vw1__MoveFileInGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MoveFileInGuest))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

const char* GuestOperationsManager::type = "GuestOperationsManager";
GuestOperationsManager::GuestOperationsManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
GuestOperationsManager::~GuestOperationsManager() {}
GuestAuthManager GuestOperationsManager::get_authManager() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("authManager", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return GuestAuthManager();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return GuestAuthManager(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}

GuestFileManager GuestOperationsManager::get_fileManager() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("fileManager", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return GuestFileManager();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return GuestFileManager(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}

GuestProcessManager GuestOperationsManager::get_processManager() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("processManager", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return GuestProcessManager();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return GuestProcessManager(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}


const char* GuestProcessManager::type = "GuestProcessManager";
GuestProcessManager::GuestProcessManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
GuestProcessManager::~GuestProcessManager() {}

std::vector<vw1__GuestProcessInfo> GuestProcessManager::ListProcessesInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, int sizepids, __int64* pids)
{
	vw1__ListProcessesInGuestRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.auth = auth;
	req.__sizepids = sizepids;
	req.pids = pids;

	// call_defs ListProcessesInGuest to post the request to ESX server or virtual center
	_vw1__ListProcessesInGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ListProcessesInGuest))
	{
		std::vector<vw1__GuestProcessInfo> vw1__guestprocessinfos;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__GuestProcessInfo tmp(*rsp.returnval[i]);
			vw1__guestprocessinfos.push_back(tmp);
		}
		return vw1__guestprocessinfos;
	}

	throw get_last_error();
}

std::vector<std::string> GuestProcessManager::ReadEnvironmentVariableInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, int sizenames, char** names)
{
	vw1__ReadEnvironmentVariableInGuestRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.auth = auth;
	req.__sizenames = sizenames;
	req.names = names;

	// call_defs ReadEnvironmentVariableInGuest to post the request to ESX server or virtual center
	_vw1__ReadEnvironmentVariableInGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReadEnvironmentVariableInGuest))
	{
		std::vector<std::string> strings;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			std::string tmp(rsp.returnval[i]);
			strings.push_back(tmp);
		}
		return strings;
	}

	throw get_last_error();
}

__int64 GuestProcessManager::StartProgramInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, vw1__GuestProgramSpec*  spec)
{
	vw1__StartProgramInGuestRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.auth = auth;
	req.spec = spec;

	// call_defs StartProgramInGuest to post the request to ESX server or virtual center
	_vw1__StartProgramInGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::StartProgramInGuest))
	{
		return static_cast<__int64>(rsp.returnval);
	}

	throw get_last_error();
}

void GuestProcessManager::TerminateProcessInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, __int64 pid)
{
	vw1__TerminateProcessInGuestRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.auth = auth;
	req.pid = pid;

	// call_defs TerminateProcessInGuest to post the request to ESX server or virtual center
	_vw1__TerminateProcessInGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::TerminateProcessInGuest))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

const char* HistoryCollector::type = "HistoryCollector";
HistoryCollector::HistoryCollector(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HistoryCollector::~HistoryCollector() {}

void HistoryCollector::DestroyCollector()
{
	vw1__DestroyCollectorRequestType req;
	req._USCOREthis = _mor;

	// call_defs DestroyCollector to post the request to ESX server or virtual center
	_vw1__DestroyCollectorResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DestroyCollector))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HistoryCollector::ResetCollector()
{
	vw1__ResetCollectorRequestType req;
	req._USCOREthis = _mor;

	// call_defs ResetCollector to post the request to ESX server or virtual center
	_vw1__ResetCollectorResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ResetCollector))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HistoryCollector::RewindCollector()
{
	vw1__RewindCollectorRequestType req;
	req._USCOREthis = _mor;

	// call_defs RewindCollector to post the request to ESX server or virtual center
	_vw1__RewindCollectorResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RewindCollector))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HistoryCollector::SetCollectorPageSize(int maxCount)
{
	vw1__SetCollectorPageSizeRequestType req;
	req._USCOREthis = _mor;
	req.maxCount = maxCount;

	// call_defs SetCollectorPageSize to post the request to ESX server or virtual center
	_vw1__SetCollectorPageSizeResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SetCollectorPageSize))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
anyType HistoryCollector::get_filter() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("filter", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return anyType();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<anyType*>(it->val);
}


const char* HostAuthenticationManager::type = "HostAuthenticationManager";
HostAuthenticationManager::HostAuthenticationManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HostAuthenticationManager::~HostAuthenticationManager() {}
vw1__HostAuthenticationManagerInfo HostAuthenticationManager::get_info() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("info", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostAuthenticationManagerInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostAuthenticationManagerInfo*>(it->val);
}

std::vector<HostAuthenticationStore> HostAuthenticationManager::get_supportedStore() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("supportedStore", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<HostAuthenticationStore>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<HostAuthenticationStore> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(HostAuthenticationStore(amo->ManagedObjectReference[i]));
	return ret;
}


const char* HostAuthenticationStore::type = "HostAuthenticationStore";
HostAuthenticationStore::HostAuthenticationStore(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HostAuthenticationStore::~HostAuthenticationStore() {}
vw1__HostAuthenticationStoreInfo HostAuthenticationStore::get_info() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("info", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostAuthenticationStoreInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostAuthenticationStoreInfo*>(it->val);
}


const char* HostAutoStartManager::type = "HostAutoStartManager";
HostAutoStartManager::HostAutoStartManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HostAutoStartManager::~HostAutoStartManager() {}

void HostAutoStartManager::AutoStartPowerOff()
{
	vw1__AutoStartPowerOffRequestType req;
	req._USCOREthis = _mor;

	// call_defs AutoStartPowerOff to post the request to ESX server or virtual center
	_vw1__AutoStartPowerOffResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AutoStartPowerOff))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostAutoStartManager::AutoStartPowerOn()
{
	vw1__AutoStartPowerOnRequestType req;
	req._USCOREthis = _mor;

	// call_defs AutoStartPowerOn to post the request to ESX server or virtual center
	_vw1__AutoStartPowerOnResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AutoStartPowerOn))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostAutoStartManager::ReconfigureAutostart(vw1__HostAutoStartManagerConfig*  spec)
{
	vw1__ReconfigureAutostartRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;

	// call_defs ReconfigureAutostart to post the request to ESX server or virtual center
	_vw1__ReconfigureAutostartResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReconfigureAutostart))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__HostAutoStartManagerConfig HostAutoStartManager::get_config() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("config", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostAutoStartManagerConfig();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostAutoStartManagerConfig*>(it->val);
}


const char* HostBootDeviceSystem::type = "HostBootDeviceSystem";
HostBootDeviceSystem::HostBootDeviceSystem(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HostBootDeviceSystem::~HostBootDeviceSystem() {}

vw1__HostBootDeviceInfo HostBootDeviceSystem::QueryBootDevices()
{
	vw1__QueryBootDevicesRequestType req;
	req._USCOREthis = _mor;

	// call_defs QueryBootDevices to post the request to ESX server or virtual center
	_vw1__QueryBootDevicesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryBootDevices))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

void HostBootDeviceSystem::UpdateBootDevice(char* key)
{
	vw1__UpdateBootDeviceRequestType req;
	req._USCOREthis = _mor;
	req.key = key;

	// call_defs UpdateBootDevice to post the request to ESX server or virtual center
	_vw1__UpdateBootDeviceResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateBootDevice))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

const char* HostCacheConfigurationManager::type = "HostCacheConfigurationManager";
HostCacheConfigurationManager::HostCacheConfigurationManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HostCacheConfigurationManager::~HostCacheConfigurationManager() {}

Task HostCacheConfigurationManager::ConfigureHostCache_Task(vw1__HostCacheConfigurationSpec*  spec)
{
	vw1__ConfigureHostCacheRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;

	// call_defs ConfigureHostCache_USCORETask to post the request to ESX server or virtual center
	_vw1__ConfigureHostCache_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ConfigureHostCache_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}
std::vector<vw1__HostCacheConfigurationInfo> HostCacheConfigurationManager::get_cacheConfigurationInfo() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("cacheConfigurationInfo", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__HostCacheConfigurationInfo>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfHostCacheConfigurationInfo* amo = dynamic_cast<vw1__ArrayOfHostCacheConfigurationInfo*>(it->val);
	std::vector<vw1__HostCacheConfigurationInfo> ret;
	for (int i = 0; i < amo->__sizeHostCacheConfigurationInfo; ++i)
		ret.push_back(*amo->HostCacheConfigurationInfo[i]);
	return ret;
}


const char* HostCpuSchedulerSystem::type = "HostCpuSchedulerSystem";
HostCpuSchedulerSystem::HostCpuSchedulerSystem(vw1__ManagedObjectReference* mor) : ExtensibleManagedObject(mor) {}
HostCpuSchedulerSystem::~HostCpuSchedulerSystem() {}

void HostCpuSchedulerSystem::DisableHyperThreading()
{
	vw1__DisableHyperThreadingRequestType req;
	req._USCOREthis = _mor;

	// call_defs DisableHyperThreading to post the request to ESX server or virtual center
	_vw1__DisableHyperThreadingResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DisableHyperThreading))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostCpuSchedulerSystem::EnableHyperThreading()
{
	vw1__EnableHyperThreadingRequestType req;
	req._USCOREthis = _mor;

	// call_defs EnableHyperThreading to post the request to ESX server or virtual center
	_vw1__EnableHyperThreadingResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::EnableHyperThreading))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__HostHyperThreadScheduleInfo HostCpuSchedulerSystem::get_hyperthreadInfo() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("hyperthreadInfo", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostHyperThreadScheduleInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostHyperThreadScheduleInfo*>(it->val);
}


const char* HostDatastoreBrowser::type = "HostDatastoreBrowser";
HostDatastoreBrowser::HostDatastoreBrowser(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HostDatastoreBrowser::~HostDatastoreBrowser() {}

void HostDatastoreBrowser::DeleteFile(char* datastorePath)
{
	vw1__DeleteFileRequestType req;
	req._USCOREthis = _mor;
	req.datastorePath = datastorePath;

	// call_defs DeleteFile to post the request to ESX server or virtual center
	_vw1__DeleteFileResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DeleteFile))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task HostDatastoreBrowser::SearchDatastore_Task(char* datastorePath, vw1__HostDatastoreBrowserSearchSpec*  searchSpec)
{
	vw1__SearchDatastoreRequestType req;
	req._USCOREthis = _mor;
	req.datastorePath = datastorePath;
	req.searchSpec = searchSpec;

	// call_defs SearchDatastore_USCORETask to post the request to ESX server or virtual center
	_vw1__SearchDatastore_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SearchDatastore_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task HostDatastoreBrowser::SearchDatastoreSubFolders_Task(char* datastorePath, vw1__HostDatastoreBrowserSearchSpec*  searchSpec)
{
	vw1__SearchDatastoreSubFoldersRequestType req;
	req._USCOREthis = _mor;
	req.datastorePath = datastorePath;
	req.searchSpec = searchSpec;

	// call_defs SearchDatastoreSubFolders_USCORETask to post the request to ESX server or virtual center
	_vw1__SearchDatastoreSubFolders_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SearchDatastoreSubFolders_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}
std::vector<Datastore> HostDatastoreBrowser::get_datastore() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("datastore", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<Datastore>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<Datastore> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(Datastore(amo->ManagedObjectReference[i]));
	return ret;
}

std::vector<vw1__FileQuery> HostDatastoreBrowser::get_supportedType() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("supportedType", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__FileQuery>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfFileQuery* amo = dynamic_cast<vw1__ArrayOfFileQuery*>(it->val);
	std::vector<vw1__FileQuery> ret;
	for (int i = 0; i < amo->__sizeFileQuery; ++i)
		ret.push_back(*amo->FileQuery[i]);
	return ret;
}


const char* HostDatastoreSystem::type = "HostDatastoreSystem";
HostDatastoreSystem::HostDatastoreSystem(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HostDatastoreSystem::~HostDatastoreSystem() {}

void HostDatastoreSystem::ConfigureDatastorePrincipal(char* userName, char* password)
{
	vw1__ConfigureDatastorePrincipalRequestType req;
	req._USCOREthis = _mor;
	req.userName = userName;
	req.password = password;

	// call_defs ConfigureDatastorePrincipal to post the request to ESX server or virtual center
	_vw1__ConfigureDatastorePrincipalResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ConfigureDatastorePrincipal))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Datastore HostDatastoreSystem::CreateLocalDatastore(char* name, char* path)
{
	vw1__CreateLocalDatastoreRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.path = path;

	// call_defs CreateLocalDatastore to post the request to ESX server or virtual center
	_vw1__CreateLocalDatastoreResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateLocalDatastore))
	{
		return Datastore(rsp.returnval);
	}

	throw get_last_error();
}

Datastore HostDatastoreSystem::CreateNasDatastore(vw1__HostNasVolumeSpec*  spec)
{
	vw1__CreateNasDatastoreRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;

	// call_defs CreateNasDatastore to post the request to ESX server or virtual center
	_vw1__CreateNasDatastoreResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateNasDatastore))
	{
		return Datastore(rsp.returnval);
	}

	throw get_last_error();
}

Datastore HostDatastoreSystem::CreateVmfsDatastore(vw1__VmfsDatastoreCreateSpec*  spec)
{
	vw1__CreateVmfsDatastoreRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;

	// call_defs CreateVmfsDatastore to post the request to ESX server or virtual center
	_vw1__CreateVmfsDatastoreResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateVmfsDatastore))
	{
		return Datastore(rsp.returnval);
	}

	throw get_last_error();
}

Datastore HostDatastoreSystem::ExpandVmfsDatastore(Datastore datastore, vw1__VmfsDatastoreExpandSpec*  spec)
{
	vw1__ExpandVmfsDatastoreRequestType req;
	req._USCOREthis = _mor;
	req.datastore = datastore;
	req.spec = spec;

	// call_defs ExpandVmfsDatastore to post the request to ESX server or virtual center
	_vw1__ExpandVmfsDatastoreResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ExpandVmfsDatastore))
	{
		return Datastore(rsp.returnval);
	}

	throw get_last_error();
}

Datastore HostDatastoreSystem::ExtendVmfsDatastore(Datastore datastore, vw1__VmfsDatastoreExtendSpec*  spec)
{
	vw1__ExtendVmfsDatastoreRequestType req;
	req._USCOREthis = _mor;
	req.datastore = datastore;
	req.spec = spec;

	// call_defs ExtendVmfsDatastore to post the request to ESX server or virtual center
	_vw1__ExtendVmfsDatastoreResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ExtendVmfsDatastore))
	{
		return Datastore(rsp.returnval);
	}

	throw get_last_error();
}

std::vector<vw1__HostScsiDisk> HostDatastoreSystem::QueryAvailableDisksForVmfs(Datastore datastore)
{
	vw1__QueryAvailableDisksForVmfsRequestType req;
	req._USCOREthis = _mor;
	req.datastore = datastore;

	// call_defs QueryAvailableDisksForVmfs to post the request to ESX server or virtual center
	_vw1__QueryAvailableDisksForVmfsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryAvailableDisksForVmfs))
	{
		std::vector<vw1__HostScsiDisk> vw1__hostscsidisks;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__HostScsiDisk tmp(*rsp.returnval[i]);
			vw1__hostscsidisks.push_back(tmp);
		}
		return vw1__hostscsidisks;
	}

	throw get_last_error();
}

std::vector<vw1__HostUnresolvedVmfsVolume> HostDatastoreSystem::QueryUnresolvedVmfsVolumes()
{
	vw1__QueryUnresolvedVmfsVolumesRequestType req;
	req._USCOREthis = _mor;

	// call_defs QueryUnresolvedVmfsVolumes to post the request to ESX server or virtual center
	_vw1__QueryUnresolvedVmfsVolumesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryUnresolvedVmfsVolumes))
	{
		std::vector<vw1__HostUnresolvedVmfsVolume> vw1__hostunresolvedvmfsvolumes;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__HostUnresolvedVmfsVolume tmp(*rsp.returnval[i]);
			vw1__hostunresolvedvmfsvolumes.push_back(tmp);
		}
		return vw1__hostunresolvedvmfsvolumes;
	}

	throw get_last_error();
}

std::vector<vw1__VmfsDatastoreOption> HostDatastoreSystem::QueryVmfsDatastoreCreateOptions(char* devicePath, int*  vmfsMajorVersion)
{
	vw1__QueryVmfsDatastoreCreateOptionsRequestType req;
	req._USCOREthis = _mor;
	req.devicePath = devicePath;
	req.vmfsMajorVersion = vmfsMajorVersion;

	// call_defs QueryVmfsDatastoreCreateOptions to post the request to ESX server or virtual center
	_vw1__QueryVmfsDatastoreCreateOptionsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryVmfsDatastoreCreateOptions))
	{
		std::vector<vw1__VmfsDatastoreOption> vw1__vmfsdatastoreoptions;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__VmfsDatastoreOption tmp(*rsp.returnval[i]);
			vw1__vmfsdatastoreoptions.push_back(tmp);
		}
		return vw1__vmfsdatastoreoptions;
	}

	throw get_last_error();
}

std::vector<vw1__VmfsDatastoreOption> HostDatastoreSystem::QueryVmfsDatastoreExpandOptions(Datastore datastore)
{
	vw1__QueryVmfsDatastoreExpandOptionsRequestType req;
	req._USCOREthis = _mor;
	req.datastore = datastore;

	// call_defs QueryVmfsDatastoreExpandOptions to post the request to ESX server or virtual center
	_vw1__QueryVmfsDatastoreExpandOptionsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryVmfsDatastoreExpandOptions))
	{
		std::vector<vw1__VmfsDatastoreOption> vw1__vmfsdatastoreoptions;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__VmfsDatastoreOption tmp(*rsp.returnval[i]);
			vw1__vmfsdatastoreoptions.push_back(tmp);
		}
		return vw1__vmfsdatastoreoptions;
	}

	throw get_last_error();
}

std::vector<vw1__VmfsDatastoreOption> HostDatastoreSystem::QueryVmfsDatastoreExtendOptions(Datastore datastore, char* devicePath, bool*  suppressExpandCandidates)
{
	vw1__QueryVmfsDatastoreExtendOptionsRequestType req;
	req._USCOREthis = _mor;
	req.datastore = datastore;
	req.devicePath = devicePath;
	req.suppressExpandCandidates = suppressExpandCandidates;

	// call_defs QueryVmfsDatastoreExtendOptions to post the request to ESX server or virtual center
	_vw1__QueryVmfsDatastoreExtendOptionsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryVmfsDatastoreExtendOptions))
	{
		std::vector<vw1__VmfsDatastoreOption> vw1__vmfsdatastoreoptions;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__VmfsDatastoreOption tmp(*rsp.returnval[i]);
			vw1__vmfsdatastoreoptions.push_back(tmp);
		}
		return vw1__vmfsdatastoreoptions;
	}

	throw get_last_error();
}

void HostDatastoreSystem::RemoveDatastore(Datastore datastore)
{
	vw1__RemoveDatastoreRequestType req;
	req._USCOREthis = _mor;
	req.datastore = datastore;

	// call_defs RemoveDatastore to post the request to ESX server or virtual center
	_vw1__RemoveDatastoreResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemoveDatastore))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task HostDatastoreSystem::ResignatureUnresolvedVmfsVolume_Task(vw1__HostUnresolvedVmfsResignatureSpec*  resolutionSpec)
{
	vw1__ResignatureUnresolvedVmfsVolumeRequestType req;
	req._USCOREthis = _mor;
	req.resolutionSpec = resolutionSpec;

	// call_defs ResignatureUnresolvedVmfsVolume_USCORETask to post the request to ESX server or virtual center
	_vw1__ResignatureUnresolvedVmfsVolume_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ResignatureUnresolvedVmfsVolume_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void HostDatastoreSystem::UpdateLocalSwapDatastore(Datastore datastore)
{
	vw1__UpdateLocalSwapDatastoreRequestType req;
	req._USCOREthis = _mor;
	req.datastore = datastore;

	// call_defs UpdateLocalSwapDatastore to post the request to ESX server or virtual center
	_vw1__UpdateLocalSwapDatastoreResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateLocalSwapDatastore))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__HostDatastoreSystemCapabilities HostDatastoreSystem::get_capabilities() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("capabilities", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostDatastoreSystemCapabilities();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostDatastoreSystemCapabilities*>(it->val);
}

std::vector<Datastore> HostDatastoreSystem::get_datastore() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("datastore", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<Datastore>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<Datastore> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(Datastore(amo->ManagedObjectReference[i]));
	return ret;
}


const char* HostDateTimeSystem::type = "HostDateTimeSystem";
HostDateTimeSystem::HostDateTimeSystem(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HostDateTimeSystem::~HostDateTimeSystem() {}

std::vector<vw1__HostDateTimeSystemTimeZone> HostDateTimeSystem::QueryAvailableTimeZones()
{
	vw1__QueryAvailableTimeZonesRequestType req;
	req._USCOREthis = _mor;

	// call_defs QueryAvailableTimeZones to post the request to ESX server or virtual center
	_vw1__QueryAvailableTimeZonesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryAvailableTimeZones))
	{
		std::vector<vw1__HostDateTimeSystemTimeZone> vw1__hostdatetimesystemtimezones;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__HostDateTimeSystemTimeZone tmp(*rsp.returnval[i]);
			vw1__hostdatetimesystemtimezones.push_back(tmp);
		}
		return vw1__hostdatetimesystemtimezones;
	}

	throw get_last_error();
}

time_t HostDateTimeSystem::QueryDateTime()
{
	vw1__QueryDateTimeRequestType req;
	req._USCOREthis = _mor;

	// call_defs QueryDateTime to post the request to ESX server or virtual center
	_vw1__QueryDateTimeResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryDateTime))
	{
		return static_cast<time_t>(rsp.returnval);
	}

	throw get_last_error();
}

void HostDateTimeSystem::RefreshDateTimeSystem()
{
	vw1__RefreshDateTimeSystemRequestType req;
	req._USCOREthis = _mor;

	// call_defs RefreshDateTimeSystem to post the request to ESX server or virtual center
	_vw1__RefreshDateTimeSystemResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RefreshDateTimeSystem))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostDateTimeSystem::UpdateDateTime(time_t dateTime)
{
	vw1__UpdateDateTimeRequestType req;
	req._USCOREthis = _mor;
	req.dateTime = dateTime;

	// call_defs UpdateDateTime to post the request to ESX server or virtual center
	_vw1__UpdateDateTimeResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateDateTime))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostDateTimeSystem::UpdateDateTimeConfig(vw1__HostDateTimeConfig*  config)
{
	vw1__UpdateDateTimeConfigRequestType req;
	req._USCOREthis = _mor;
	req.config = config;

	// call_defs UpdateDateTimeConfig to post the request to ESX server or virtual center
	_vw1__UpdateDateTimeConfigResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateDateTimeConfig))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__HostDateTimeInfo HostDateTimeSystem::get_dateTimeInfo() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("dateTimeInfo", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostDateTimeInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostDateTimeInfo*>(it->val);
}


const char* HostDiagnosticSystem::type = "HostDiagnosticSystem";
HostDiagnosticSystem::HostDiagnosticSystem(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HostDiagnosticSystem::~HostDiagnosticSystem() {}

void HostDiagnosticSystem::CreateDiagnosticPartition(vw1__HostDiagnosticPartitionCreateSpec*  spec)
{
	vw1__CreateDiagnosticPartitionRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;

	// call_defs CreateDiagnosticPartition to post the request to ESX server or virtual center
	_vw1__CreateDiagnosticPartitionResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateDiagnosticPartition))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

std::vector<vw1__HostDiagnosticPartition> HostDiagnosticSystem::QueryAvailablePartition()
{
	vw1__QueryAvailablePartitionRequestType req;
	req._USCOREthis = _mor;

	// call_defs QueryAvailablePartition to post the request to ESX server or virtual center
	_vw1__QueryAvailablePartitionResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryAvailablePartition))
	{
		std::vector<vw1__HostDiagnosticPartition> vw1__hostdiagnosticpartitions;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__HostDiagnosticPartition tmp(*rsp.returnval[i]);
			vw1__hostdiagnosticpartitions.push_back(tmp);
		}
		return vw1__hostdiagnosticpartitions;
	}

	throw get_last_error();
}

vw1__HostDiagnosticPartitionCreateDescription HostDiagnosticSystem::QueryPartitionCreateDesc(char* diskUuid, char* diagnosticType)
{
	vw1__QueryPartitionCreateDescRequestType req;
	req._USCOREthis = _mor;
	req.diskUuid = diskUuid;
	req.diagnosticType = diagnosticType;

	// call_defs QueryPartitionCreateDesc to post the request to ESX server or virtual center
	_vw1__QueryPartitionCreateDescResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryPartitionCreateDesc))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

std::vector<vw1__HostDiagnosticPartitionCreateOption> HostDiagnosticSystem::QueryPartitionCreateOptions(char* storageType, char* diagnosticType)
{
	vw1__QueryPartitionCreateOptionsRequestType req;
	req._USCOREthis = _mor;
	req.storageType = storageType;
	req.diagnosticType = diagnosticType;

	// call_defs QueryPartitionCreateOptions to post the request to ESX server or virtual center
	_vw1__QueryPartitionCreateOptionsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryPartitionCreateOptions))
	{
		std::vector<vw1__HostDiagnosticPartitionCreateOption> vw1__hostdiagnosticpartitioncreateoptions;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__HostDiagnosticPartitionCreateOption tmp(*rsp.returnval[i]);
			vw1__hostdiagnosticpartitioncreateoptions.push_back(tmp);
		}
		return vw1__hostdiagnosticpartitioncreateoptions;
	}

	throw get_last_error();
}

void HostDiagnosticSystem::SelectActivePartition(vw1__HostScsiDiskPartition*  partition)
{
	vw1__SelectActivePartitionRequestType req;
	req._USCOREthis = _mor;
	req.partition = partition;

	// call_defs SelectActivePartition to post the request to ESX server or virtual center
	_vw1__SelectActivePartitionResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SelectActivePartition))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__HostDiagnosticPartition HostDiagnosticSystem::get_activePartition() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("activePartition", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostDiagnosticPartition();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostDiagnosticPartition*>(it->val);
}


const char* HostDirectoryStore::type = "HostDirectoryStore";
HostDirectoryStore::HostDirectoryStore(vw1__ManagedObjectReference* mor) : HostAuthenticationStore(mor) {}
HostDirectoryStore::~HostDirectoryStore() {}

const char* HostEsxAgentHostManager::type = "HostEsxAgentHostManager";
HostEsxAgentHostManager::HostEsxAgentHostManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HostEsxAgentHostManager::~HostEsxAgentHostManager() {}

void HostEsxAgentHostManager::EsxAgentHostManagerUpdateConfig(vw1__HostEsxAgentHostManagerConfigInfo*  configInfo)
{
	vw1__EsxAgentHostManagerUpdateConfigRequestType req;
	req._USCOREthis = _mor;
	req.configInfo = configInfo;

	// call_defs EsxAgentHostManagerUpdateConfig to post the request to ESX server or virtual center
	_vw1__EsxAgentHostManagerUpdateConfigResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::EsxAgentHostManagerUpdateConfig))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__HostEsxAgentHostManagerConfigInfo HostEsxAgentHostManager::get_configInfo() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("configInfo", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostEsxAgentHostManagerConfigInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostEsxAgentHostManagerConfigInfo*>(it->val);
}


const char* HostFirewallSystem::type = "HostFirewallSystem";
HostFirewallSystem::HostFirewallSystem(vw1__ManagedObjectReference* mor) : ExtensibleManagedObject(mor) {}
HostFirewallSystem::~HostFirewallSystem() {}

void HostFirewallSystem::DisableRuleset(char* id)
{
	vw1__DisableRulesetRequestType req;
	req._USCOREthis = _mor;
	req.id = id;

	// call_defs DisableRuleset to post the request to ESX server or virtual center
	_vw1__DisableRulesetResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DisableRuleset))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostFirewallSystem::EnableRuleset(char* id)
{
	vw1__EnableRulesetRequestType req;
	req._USCOREthis = _mor;
	req.id = id;

	// call_defs EnableRuleset to post the request to ESX server or virtual center
	_vw1__EnableRulesetResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::EnableRuleset))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostFirewallSystem::RefreshFirewall()
{
	vw1__RefreshFirewallRequestType req;
	req._USCOREthis = _mor;

	// call_defs RefreshFirewall to post the request to ESX server or virtual center
	_vw1__RefreshFirewallResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RefreshFirewall))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostFirewallSystem::UpdateDefaultPolicy(vw1__HostFirewallDefaultPolicy*  defaultPolicy)
{
	vw1__UpdateDefaultPolicyRequestType req;
	req._USCOREthis = _mor;
	req.defaultPolicy = defaultPolicy;

	// call_defs UpdateDefaultPolicy to post the request to ESX server or virtual center
	_vw1__UpdateDefaultPolicyResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateDefaultPolicy))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostFirewallSystem::UpdateRuleset(char* id, vw1__HostFirewallRulesetRulesetSpec*  spec)
{
	vw1__UpdateRulesetRequestType req;
	req._USCOREthis = _mor;
	req.id = id;
	req.spec = spec;

	// call_defs UpdateRuleset to post the request to ESX server or virtual center
	_vw1__UpdateRulesetResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateRuleset))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__HostFirewallInfo HostFirewallSystem::get_firewallInfo() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("firewallInfo", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostFirewallInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostFirewallInfo*>(it->val);
}


const char* HostFirmwareSystem::type = "HostFirmwareSystem";
HostFirmwareSystem::HostFirmwareSystem(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HostFirmwareSystem::~HostFirmwareSystem() {}

std::string HostFirmwareSystem::BackupFirmwareConfiguration()
{
	vw1__BackupFirmwareConfigurationRequestType req;
	req._USCOREthis = _mor;

	// call_defs BackupFirmwareConfiguration to post the request to ESX server or virtual center
	_vw1__BackupFirmwareConfigurationResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::BackupFirmwareConfiguration))
	{
		return static_cast<std::string>(rsp.returnval);
	}

	throw get_last_error();
}

std::string HostFirmwareSystem::QueryFirmwareConfigUploadURL()
{
	vw1__QueryFirmwareConfigUploadURLRequestType req;
	req._USCOREthis = _mor;

	// call_defs QueryFirmwareConfigUploadURL to post the request to ESX server or virtual center
	_vw1__QueryFirmwareConfigUploadURLResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryFirmwareConfigUploadURL))
	{
		return static_cast<std::string>(rsp.returnval);
	}

	throw get_last_error();
}

void HostFirmwareSystem::ResetFirmwareToFactoryDefaults()
{
	vw1__ResetFirmwareToFactoryDefaultsRequestType req;
	req._USCOREthis = _mor;

	// call_defs ResetFirmwareToFactoryDefaults to post the request to ESX server or virtual center
	_vw1__ResetFirmwareToFactoryDefaultsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ResetFirmwareToFactoryDefaults))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostFirmwareSystem::RestoreFirmwareConfiguration(bool force)
{
	vw1__RestoreFirmwareConfigurationRequestType req;
	req._USCOREthis = _mor;
	req.force = force;

	// call_defs RestoreFirmwareConfiguration to post the request to ESX server or virtual center
	_vw1__RestoreFirmwareConfigurationResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RestoreFirmwareConfiguration))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

const char* HostHealthStatusSystem::type = "HostHealthStatusSystem";
HostHealthStatusSystem::HostHealthStatusSystem(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HostHealthStatusSystem::~HostHealthStatusSystem() {}

void HostHealthStatusSystem::RefreshHealthStatusSystem()
{
	vw1__RefreshHealthStatusSystemRequestType req;
	req._USCOREthis = _mor;

	// call_defs RefreshHealthStatusSystem to post the request to ESX server or virtual center
	_vw1__RefreshHealthStatusSystemResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RefreshHealthStatusSystem))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostHealthStatusSystem::ResetSystemHealthInfo()
{
	vw1__ResetSystemHealthInfoRequestType req;
	req._USCOREthis = _mor;

	// call_defs ResetSystemHealthInfo to post the request to ESX server or virtual center
	_vw1__ResetSystemHealthInfoResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ResetSystemHealthInfo))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__HealthSystemRuntime HostHealthStatusSystem::get_runtime() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("runtime", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HealthSystemRuntime();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HealthSystemRuntime*>(it->val);
}


const char* HostImageConfigManager::type = "HostImageConfigManager";
HostImageConfigManager::HostImageConfigManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HostImageConfigManager::~HostImageConfigManager() {}

std::string HostImageConfigManager::HostImageConfigGetAcceptance()
{
	vw1__HostImageConfigGetAcceptanceRequestType req;
	req._USCOREthis = _mor;

	// call_defs HostImageConfigGetAcceptance to post the request to ESX server or virtual center
	_vw1__HostImageConfigGetAcceptanceResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::HostImageConfigGetAcceptance))
	{
		return static_cast<std::string>(rsp.returnval);
	}

	throw get_last_error();
}

vw1__HostImageProfileSummary HostImageConfigManager::HostImageConfigGetProfile()
{
	vw1__HostImageConfigGetProfileRequestType req;
	req._USCOREthis = _mor;

	// call_defs HostImageConfigGetProfile to post the request to ESX server or virtual center
	_vw1__HostImageConfigGetProfileResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::HostImageConfigGetProfile))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

void HostImageConfigManager::UpdateHostImageAcceptanceLevel(char* newAcceptanceLevel)
{
	vw1__UpdateHostImageAcceptanceLevelRequestType req;
	req._USCOREthis = _mor;
	req.newAcceptanceLevel = newAcceptanceLevel;

	// call_defs UpdateHostImageAcceptanceLevel to post the request to ESX server or virtual center
	_vw1__UpdateHostImageAcceptanceLevelResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateHostImageAcceptanceLevel))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

const char* HostKernelModuleSystem::type = "HostKernelModuleSystem";
HostKernelModuleSystem::HostKernelModuleSystem(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HostKernelModuleSystem::~HostKernelModuleSystem() {}

std::string HostKernelModuleSystem::QueryConfiguredModuleOptionString(char* name)
{
	vw1__QueryConfiguredModuleOptionStringRequestType req;
	req._USCOREthis = _mor;
	req.name = name;

	// call_defs QueryConfiguredModuleOptionString to post the request to ESX server or virtual center
	_vw1__QueryConfiguredModuleOptionStringResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryConfiguredModuleOptionString))
	{
		return static_cast<std::string>(rsp.returnval);
	}

	throw get_last_error();
}

std::vector<vw1__KernelModuleInfo> HostKernelModuleSystem::QueryModules()
{
	vw1__QueryModulesRequestType req;
	req._USCOREthis = _mor;

	// call_defs QueryModules to post the request to ESX server or virtual center
	_vw1__QueryModulesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryModules))
	{
		std::vector<vw1__KernelModuleInfo> vw1__kernelmoduleinfos;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__KernelModuleInfo tmp(*rsp.returnval[i]);
			vw1__kernelmoduleinfos.push_back(tmp);
		}
		return vw1__kernelmoduleinfos;
	}

	throw get_last_error();
}

void HostKernelModuleSystem::UpdateModuleOptionString(char* name, char* options)
{
	vw1__UpdateModuleOptionStringRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.options = options;

	// call_defs UpdateModuleOptionString to post the request to ESX server or virtual center
	_vw1__UpdateModuleOptionStringResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateModuleOptionString))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

const char* HostLocalAccountManager::type = "HostLocalAccountManager";
HostLocalAccountManager::HostLocalAccountManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HostLocalAccountManager::~HostLocalAccountManager() {}

void HostLocalAccountManager::AssignUserToGroup(char* user, char* group)
{
	vw1__AssignUserToGroupRequestType req;
	req._USCOREthis = _mor;
	req.user = user;
	req.group = group;

	// call_defs AssignUserToGroup to post the request to ESX server or virtual center
	_vw1__AssignUserToGroupResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AssignUserToGroup))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostLocalAccountManager::CreateGroup(vw1__HostAccountSpec*  group)
{
	vw1__CreateGroupRequestType req;
	req._USCOREthis = _mor;
	req.group = group;

	// call_defs CreateGroup to post the request to ESX server or virtual center
	_vw1__CreateGroupResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateGroup))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostLocalAccountManager::CreateUser(vw1__HostAccountSpec*  user)
{
	vw1__CreateUserRequestType req;
	req._USCOREthis = _mor;
	req.user = user;

	// call_defs CreateUser to post the request to ESX server or virtual center
	_vw1__CreateUserResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateUser))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostLocalAccountManager::RemoveGroup(char* groupName)
{
	vw1__RemoveGroupRequestType req;
	req._USCOREthis = _mor;
	req.groupName = groupName;

	// call_defs RemoveGroup to post the request to ESX server or virtual center
	_vw1__RemoveGroupResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemoveGroup))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostLocalAccountManager::RemoveUser(char* userName)
{
	vw1__RemoveUserRequestType req;
	req._USCOREthis = _mor;
	req.userName = userName;

	// call_defs RemoveUser to post the request to ESX server or virtual center
	_vw1__RemoveUserResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemoveUser))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostLocalAccountManager::UnassignUserFromGroup(char* user, char* group)
{
	vw1__UnassignUserFromGroupRequestType req;
	req._USCOREthis = _mor;
	req.user = user;
	req.group = group;

	// call_defs UnassignUserFromGroup to post the request to ESX server or virtual center
	_vw1__UnassignUserFromGroupResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UnassignUserFromGroup))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostLocalAccountManager::UpdateUser(vw1__HostAccountSpec*  user)
{
	vw1__UpdateUserRequestType req;
	req._USCOREthis = _mor;
	req.user = user;

	// call_defs UpdateUser to post the request to ESX server or virtual center
	_vw1__UpdateUserResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateUser))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

const char* HostLocalAuthentication::type = "HostLocalAuthentication";
HostLocalAuthentication::HostLocalAuthentication(vw1__ManagedObjectReference* mor) : HostAuthenticationStore(mor) {}
HostLocalAuthentication::~HostLocalAuthentication() {}

const char* HostMemorySystem::type = "HostMemorySystem";
HostMemorySystem::HostMemorySystem(vw1__ManagedObjectReference* mor) : ExtensibleManagedObject(mor) {}
HostMemorySystem::~HostMemorySystem() {}

void HostMemorySystem::ReconfigureServiceConsoleReservation(__int64 cfgBytes)
{
	vw1__ReconfigureServiceConsoleReservationRequestType req;
	req._USCOREthis = _mor;
	req.cfgBytes = cfgBytes;

	// call_defs ReconfigureServiceConsoleReservation to post the request to ESX server or virtual center
	_vw1__ReconfigureServiceConsoleReservationResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReconfigureServiceConsoleReservation))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostMemorySystem::ReconfigureVirtualMachineReservation(vw1__VirtualMachineMemoryReservationSpec*  spec)
{
	vw1__ReconfigureVirtualMachineReservationRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;

	// call_defs ReconfigureVirtualMachineReservation to post the request to ESX server or virtual center
	_vw1__ReconfigureVirtualMachineReservationResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReconfigureVirtualMachineReservation))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__ServiceConsoleReservationInfo HostMemorySystem::get_consoleReservationInfo() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("consoleReservationInfo", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__ServiceConsoleReservationInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__ServiceConsoleReservationInfo*>(it->val);
}

vw1__VirtualMachineMemoryReservationInfo HostMemorySystem::get_virtualMachineReservationInfo() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("virtualMachineReservationInfo", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__VirtualMachineMemoryReservationInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__VirtualMachineMemoryReservationInfo*>(it->val);
}


const char* HostNetworkSystem::type = "HostNetworkSystem";
HostNetworkSystem::HostNetworkSystem(vw1__ManagedObjectReference* mor) : ExtensibleManagedObject(mor) {}
HostNetworkSystem::~HostNetworkSystem() {}

void HostNetworkSystem::AddPortGroup(vw1__HostPortGroupSpec*  portgrp)
{
	vw1__AddPortGroupRequestType req;
	req._USCOREthis = _mor;
	req.portgrp = portgrp;

	// call_defs AddPortGroup to post the request to ESX server or virtual center
	_vw1__AddPortGroupResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AddPortGroup))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

std::string HostNetworkSystem::AddServiceConsoleVirtualNic(char* portgroup, vw1__HostVirtualNicSpec*  nic)
{
	vw1__AddServiceConsoleVirtualNicRequestType req;
	req._USCOREthis = _mor;
	req.portgroup = portgroup;
	req.nic = nic;

	// call_defs AddServiceConsoleVirtualNic to post the request to ESX server or virtual center
	_vw1__AddServiceConsoleVirtualNicResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AddServiceConsoleVirtualNic))
	{
		return static_cast<std::string>(rsp.returnval);
	}

	throw get_last_error();
}

std::string HostNetworkSystem::AddVirtualNic(char* portgroup, vw1__HostVirtualNicSpec*  nic)
{
	vw1__AddVirtualNicRequestType req;
	req._USCOREthis = _mor;
	req.portgroup = portgroup;
	req.nic = nic;

	// call_defs AddVirtualNic to post the request to ESX server or virtual center
	_vw1__AddVirtualNicResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AddVirtualNic))
	{
		return static_cast<std::string>(rsp.returnval);
	}

	throw get_last_error();
}

void HostNetworkSystem::AddVirtualSwitch(char* vswitchName, vw1__HostVirtualSwitchSpec*  spec)
{
	vw1__AddVirtualSwitchRequestType req;
	req._USCOREthis = _mor;
	req.vswitchName = vswitchName;
	req.spec = spec;

	// call_defs AddVirtualSwitch to post the request to ESX server or virtual center
	_vw1__AddVirtualSwitchResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AddVirtualSwitch))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

std::vector<vw1__PhysicalNicHintInfo> HostNetworkSystem::QueryNetworkHint(int sizedevice, char** device)
{
	vw1__QueryNetworkHintRequestType req;
	req._USCOREthis = _mor;
	req.__sizedevice = sizedevice;
	req.device = device;

	// call_defs QueryNetworkHint to post the request to ESX server or virtual center
	_vw1__QueryNetworkHintResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryNetworkHint))
	{
		std::vector<vw1__PhysicalNicHintInfo> vw1__physicalnichintinfos;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__PhysicalNicHintInfo tmp(*rsp.returnval[i]);
			vw1__physicalnichintinfos.push_back(tmp);
		}
		return vw1__physicalnichintinfos;
	}

	throw get_last_error();
}

void HostNetworkSystem::RefreshNetworkSystem()
{
	vw1__RefreshNetworkSystemRequestType req;
	req._USCOREthis = _mor;

	// call_defs RefreshNetworkSystem to post the request to ESX server or virtual center
	_vw1__RefreshNetworkSystemResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RefreshNetworkSystem))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostNetworkSystem::RemovePortGroup(char* pgName)
{
	vw1__RemovePortGroupRequestType req;
	req._USCOREthis = _mor;
	req.pgName = pgName;

	// call_defs RemovePortGroup to post the request to ESX server or virtual center
	_vw1__RemovePortGroupResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemovePortGroup))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostNetworkSystem::RemoveServiceConsoleVirtualNic(char* device)
{
	vw1__RemoveServiceConsoleVirtualNicRequestType req;
	req._USCOREthis = _mor;
	req.device = device;

	// call_defs RemoveServiceConsoleVirtualNic to post the request to ESX server or virtual center
	_vw1__RemoveServiceConsoleVirtualNicResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemoveServiceConsoleVirtualNic))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostNetworkSystem::RemoveVirtualNic(char* device)
{
	vw1__RemoveVirtualNicRequestType req;
	req._USCOREthis = _mor;
	req.device = device;

	// call_defs RemoveVirtualNic to post the request to ESX server or virtual center
	_vw1__RemoveVirtualNicResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemoveVirtualNic))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostNetworkSystem::RemoveVirtualSwitch(char* vswitchName)
{
	vw1__RemoveVirtualSwitchRequestType req;
	req._USCOREthis = _mor;
	req.vswitchName = vswitchName;

	// call_defs RemoveVirtualSwitch to post the request to ESX server or virtual center
	_vw1__RemoveVirtualSwitchResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemoveVirtualSwitch))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostNetworkSystem::RestartServiceConsoleVirtualNic(char* device)
{
	vw1__RestartServiceConsoleVirtualNicRequestType req;
	req._USCOREthis = _mor;
	req.device = device;

	// call_defs RestartServiceConsoleVirtualNic to post the request to ESX server or virtual center
	_vw1__RestartServiceConsoleVirtualNicResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RestartServiceConsoleVirtualNic))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostNetworkSystem::UpdateConsoleIpRouteConfig(vw1__HostIpRouteConfig*  config)
{
	vw1__UpdateConsoleIpRouteConfigRequestType req;
	req._USCOREthis = _mor;
	req.config = config;

	// call_defs UpdateConsoleIpRouteConfig to post the request to ESX server or virtual center
	_vw1__UpdateConsoleIpRouteConfigResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateConsoleIpRouteConfig))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostNetworkSystem::UpdateDnsConfig(vw1__HostDnsConfig*  config)
{
	vw1__UpdateDnsConfigRequestType req;
	req._USCOREthis = _mor;
	req.config = config;

	// call_defs UpdateDnsConfig to post the request to ESX server or virtual center
	_vw1__UpdateDnsConfigResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateDnsConfig))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostNetworkSystem::UpdateIpRouteConfig(vw1__HostIpRouteConfig*  config)
{
	vw1__UpdateIpRouteConfigRequestType req;
	req._USCOREthis = _mor;
	req.config = config;

	// call_defs UpdateIpRouteConfig to post the request to ESX server or virtual center
	_vw1__UpdateIpRouteConfigResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateIpRouteConfig))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostNetworkSystem::UpdateIpRouteTableConfig(vw1__HostIpRouteTableConfig*  config)
{
	vw1__UpdateIpRouteTableConfigRequestType req;
	req._USCOREthis = _mor;
	req.config = config;

	// call_defs UpdateIpRouteTableConfig to post the request to ESX server or virtual center
	_vw1__UpdateIpRouteTableConfigResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateIpRouteTableConfig))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

vw1__HostNetworkConfigResult HostNetworkSystem::UpdateNetworkConfig(vw1__HostNetworkConfig*  config, char* changeMode)
{
	vw1__UpdateNetworkConfigRequestType req;
	req._USCOREthis = _mor;
	req.config = config;
	req.changeMode = changeMode;

	// call_defs UpdateNetworkConfig to post the request to ESX server or virtual center
	_vw1__UpdateNetworkConfigResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateNetworkConfig))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

void HostNetworkSystem::UpdatePhysicalNicLinkSpeed(char* device, vw1__PhysicalNicLinkInfo*  linkSpeed)
{
	vw1__UpdatePhysicalNicLinkSpeedRequestType req;
	req._USCOREthis = _mor;
	req.device = device;
	req.linkSpeed = linkSpeed;

	// call_defs UpdatePhysicalNicLinkSpeed to post the request to ESX server or virtual center
	_vw1__UpdatePhysicalNicLinkSpeedResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdatePhysicalNicLinkSpeed))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostNetworkSystem::UpdatePortGroup(char* pgName, vw1__HostPortGroupSpec*  portgrp)
{
	vw1__UpdatePortGroupRequestType req;
	req._USCOREthis = _mor;
	req.pgName = pgName;
	req.portgrp = portgrp;

	// call_defs UpdatePortGroup to post the request to ESX server or virtual center
	_vw1__UpdatePortGroupResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdatePortGroup))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostNetworkSystem::UpdateServiceConsoleVirtualNic(char* device, vw1__HostVirtualNicSpec*  nic)
{
	vw1__UpdateServiceConsoleVirtualNicRequestType req;
	req._USCOREthis = _mor;
	req.device = device;
	req.nic = nic;

	// call_defs UpdateServiceConsoleVirtualNic to post the request to ESX server or virtual center
	_vw1__UpdateServiceConsoleVirtualNicResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateServiceConsoleVirtualNic))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostNetworkSystem::UpdateVirtualNic(char* device, vw1__HostVirtualNicSpec*  nic)
{
	vw1__UpdateVirtualNicRequestType req;
	req._USCOREthis = _mor;
	req.device = device;
	req.nic = nic;

	// call_defs UpdateVirtualNic to post the request to ESX server or virtual center
	_vw1__UpdateVirtualNicResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateVirtualNic))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostNetworkSystem::UpdateVirtualSwitch(char* vswitchName, vw1__HostVirtualSwitchSpec*  spec)
{
	vw1__UpdateVirtualSwitchRequestType req;
	req._USCOREthis = _mor;
	req.vswitchName = vswitchName;
	req.spec = spec;

	// call_defs UpdateVirtualSwitch to post the request to ESX server or virtual center
	_vw1__UpdateVirtualSwitchResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateVirtualSwitch))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__HostNetCapabilities HostNetworkSystem::get_capabilities() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("capabilities", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostNetCapabilities();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostNetCapabilities*>(it->val);
}

vw1__HostIpRouteConfig HostNetworkSystem::get_consoleIpRouteConfig() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("consoleIpRouteConfig", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostIpRouteConfig();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostIpRouteConfig*>(it->val);
}

vw1__HostDnsConfig HostNetworkSystem::get_dnsConfig() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("dnsConfig", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostDnsConfig();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostDnsConfig*>(it->val);
}

vw1__HostIpRouteConfig HostNetworkSystem::get_ipRouteConfig() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("ipRouteConfig", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostIpRouteConfig();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostIpRouteConfig*>(it->val);
}

vw1__HostNetworkConfig HostNetworkSystem::get_networkConfig() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("networkConfig", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostNetworkConfig();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostNetworkConfig*>(it->val);
}

vw1__HostNetworkInfo HostNetworkSystem::get_networkInfo() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("networkInfo", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostNetworkInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostNetworkInfo*>(it->val);
}

vw1__HostNetOffloadCapabilities HostNetworkSystem::get_offloadCapabilities() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("offloadCapabilities", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostNetOffloadCapabilities();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostNetOffloadCapabilities*>(it->val);
}


const char* HostPatchManager::type = "HostPatchManager";
HostPatchManager::HostPatchManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HostPatchManager::~HostPatchManager() {}

Task HostPatchManager::CheckHostPatch_Task(int sizemetaUrls, char** metaUrls, int sizebundleUrls, char** bundleUrls, vw1__HostPatchManagerPatchManagerOperationSpec*  spec)
{
	vw1__CheckHostPatchRequestType req;
	req._USCOREthis = _mor;
	req.__sizemetaUrls = sizemetaUrls;
	req.metaUrls = metaUrls;
	req.__sizebundleUrls = sizebundleUrls;
	req.bundleUrls = bundleUrls;
	req.spec = spec;

	// call_defs CheckHostPatch_USCORETask to post the request to ESX server or virtual center
	_vw1__CheckHostPatch_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CheckHostPatch_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task HostPatchManager::InstallHostPatch_Task(vw1__HostPatchManagerLocator*  repository, char* updateID, bool*  force)
{
	vw1__InstallHostPatchRequestType req;
	req._USCOREthis = _mor;
	req.repository = repository;
	req.updateID = updateID;
	req.force = force;

	// call_defs InstallHostPatch_USCORETask to post the request to ESX server or virtual center
	_vw1__InstallHostPatch_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::InstallHostPatch_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task HostPatchManager::InstallHostPatchV2_Task(int sizemetaUrls, char** metaUrls, int sizebundleUrls, char** bundleUrls, int sizevibUrls, char** vibUrls, vw1__HostPatchManagerPatchManagerOperationSpec*  spec)
{
	vw1__InstallHostPatchV2RequestType req;
	req._USCOREthis = _mor;
	req.__sizemetaUrls = sizemetaUrls;
	req.metaUrls = metaUrls;
	req.__sizebundleUrls = sizebundleUrls;
	req.bundleUrls = bundleUrls;
	req.__sizevibUrls = sizevibUrls;
	req.vibUrls = vibUrls;
	req.spec = spec;

	// call_defs InstallHostPatchV2_USCORETask to post the request to ESX server or virtual center
	_vw1__InstallHostPatchV2_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::InstallHostPatchV2_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task HostPatchManager::QueryHostPatch_Task(vw1__HostPatchManagerPatchManagerOperationSpec*  spec)
{
	vw1__QueryHostPatchRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;

	// call_defs QueryHostPatch_USCORETask to post the request to ESX server or virtual center
	_vw1__QueryHostPatch_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryHostPatch_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task HostPatchManager::ScanHostPatch_Task(vw1__HostPatchManagerLocator*  repository, int sizeupdateID, char** updateID)
{
	vw1__ScanHostPatchRequestType req;
	req._USCOREthis = _mor;
	req.repository = repository;
	req.__sizeupdateID = sizeupdateID;
	req.updateID = updateID;

	// call_defs ScanHostPatch_USCORETask to post the request to ESX server or virtual center
	_vw1__ScanHostPatch_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ScanHostPatch_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task HostPatchManager::ScanHostPatchV2_Task(int sizemetaUrls, char** metaUrls, int sizebundleUrls, char** bundleUrls, vw1__HostPatchManagerPatchManagerOperationSpec*  spec)
{
	vw1__ScanHostPatchV2RequestType req;
	req._USCOREthis = _mor;
	req.__sizemetaUrls = sizemetaUrls;
	req.metaUrls = metaUrls;
	req.__sizebundleUrls = sizebundleUrls;
	req.bundleUrls = bundleUrls;
	req.spec = spec;

	// call_defs ScanHostPatchV2_USCORETask to post the request to ESX server or virtual center
	_vw1__ScanHostPatchV2_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ScanHostPatchV2_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task HostPatchManager::StageHostPatch_Task(int sizemetaUrls, char** metaUrls, int sizebundleUrls, char** bundleUrls, int sizevibUrls, char** vibUrls, vw1__HostPatchManagerPatchManagerOperationSpec*  spec)
{
	vw1__StageHostPatchRequestType req;
	req._USCOREthis = _mor;
	req.__sizemetaUrls = sizemetaUrls;
	req.metaUrls = metaUrls;
	req.__sizebundleUrls = sizebundleUrls;
	req.bundleUrls = bundleUrls;
	req.__sizevibUrls = sizevibUrls;
	req.vibUrls = vibUrls;
	req.spec = spec;

	// call_defs StageHostPatch_USCORETask to post the request to ESX server or virtual center
	_vw1__StageHostPatch_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::StageHostPatch_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task HostPatchManager::UninstallHostPatch_Task(int sizebulletinIds, char** bulletinIds, vw1__HostPatchManagerPatchManagerOperationSpec*  spec)
{
	vw1__UninstallHostPatchRequestType req;
	req._USCOREthis = _mor;
	req.__sizebulletinIds = sizebulletinIds;
	req.bulletinIds = bulletinIds;
	req.spec = spec;

	// call_defs UninstallHostPatch_USCORETask to post the request to ESX server or virtual center
	_vw1__UninstallHostPatch_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UninstallHostPatch_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

const char* HostPciPassthruSystem::type = "HostPciPassthruSystem";
HostPciPassthruSystem::HostPciPassthruSystem(vw1__ManagedObjectReference* mor) : ExtensibleManagedObject(mor) {}
HostPciPassthruSystem::~HostPciPassthruSystem() {}

void HostPciPassthruSystem::Refresh()
{
	vw1__RefreshRequestType req;
	req._USCOREthis = _mor;

	// call_defs Refresh to post the request to ESX server or virtual center
	_vw1__RefreshResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::Refresh))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostPciPassthruSystem::UpdatePassthruConfig(int sizeconfig, vw1__HostPciPassthruConfig**  config)
{
	vw1__UpdatePassthruConfigRequestType req;
	req._USCOREthis = _mor;
	req.__sizeconfig = sizeconfig;
	req.config = config;

	// call_defs UpdatePassthruConfig to post the request to ESX server or virtual center
	_vw1__UpdatePassthruConfigResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdatePassthruConfig))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
std::vector<vw1__HostPciPassthruInfo> HostPciPassthruSystem::get_pciPassthruInfo() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("pciPassthruInfo", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__HostPciPassthruInfo>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfHostPciPassthruInfo* amo = dynamic_cast<vw1__ArrayOfHostPciPassthruInfo*>(it->val);
	std::vector<vw1__HostPciPassthruInfo> ret;
	for (int i = 0; i < amo->__sizeHostPciPassthruInfo; ++i)
		ret.push_back(*amo->HostPciPassthruInfo[i]);
	return ret;
}


const char* HostPowerSystem::type = "HostPowerSystem";
HostPowerSystem::HostPowerSystem(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HostPowerSystem::~HostPowerSystem() {}

void HostPowerSystem::ConfigurePowerPolicy(int key)
{
	vw1__ConfigurePowerPolicyRequestType req;
	req._USCOREthis = _mor;
	req.key = key;

	// call_defs ConfigurePowerPolicy to post the request to ESX server or virtual center
	_vw1__ConfigurePowerPolicyResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ConfigurePowerPolicy))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__PowerSystemCapability HostPowerSystem::get_capability() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("capability", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__PowerSystemCapability();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__PowerSystemCapability*>(it->val);
}

vw1__PowerSystemInfo HostPowerSystem::get_info() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("info", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__PowerSystemInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__PowerSystemInfo*>(it->val);
}


const char* HostServiceSystem::type = "HostServiceSystem";
HostServiceSystem::HostServiceSystem(vw1__ManagedObjectReference* mor) : ExtensibleManagedObject(mor) {}
HostServiceSystem::~HostServiceSystem() {}

void HostServiceSystem::RefreshServices()
{
	vw1__RefreshServicesRequestType req;
	req._USCOREthis = _mor;

	// call_defs RefreshServices to post the request to ESX server or virtual center
	_vw1__RefreshServicesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RefreshServices))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostServiceSystem::RestartService(char* id)
{
	vw1__RestartServiceRequestType req;
	req._USCOREthis = _mor;
	req.id = id;

	// call_defs RestartService to post the request to ESX server or virtual center
	_vw1__RestartServiceResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RestartService))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostServiceSystem::StartService(char* id)
{
	vw1__StartServiceRequestType req;
	req._USCOREthis = _mor;
	req.id = id;

	// call_defs StartService to post the request to ESX server or virtual center
	_vw1__StartServiceResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::StartService))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostServiceSystem::StopService(char* id)
{
	vw1__StopServiceRequestType req;
	req._USCOREthis = _mor;
	req.id = id;

	// call_defs StopService to post the request to ESX server or virtual center
	_vw1__StopServiceResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::StopService))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostServiceSystem::UninstallService(char* id)
{
	vw1__UninstallServiceRequestType req;
	req._USCOREthis = _mor;
	req.id = id;

	// call_defs UninstallService to post the request to ESX server or virtual center
	_vw1__UninstallServiceResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UninstallService))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostServiceSystem::UpdateServicePolicy(char* id, char* policy)
{
	vw1__UpdateServicePolicyRequestType req;
	req._USCOREthis = _mor;
	req.id = id;
	req.policy = policy;

	// call_defs UpdateServicePolicy to post the request to ESX server or virtual center
	_vw1__UpdateServicePolicyResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateServicePolicy))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__HostServiceInfo HostServiceSystem::get_serviceInfo() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("serviceInfo", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostServiceInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostServiceInfo*>(it->val);
}


const char* HostSnmpSystem::type = "HostSnmpSystem";
HostSnmpSystem::HostSnmpSystem(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HostSnmpSystem::~HostSnmpSystem() {}

void HostSnmpSystem::ReconfigureSnmpAgent(vw1__HostSnmpConfigSpec*  spec)
{
	vw1__ReconfigureSnmpAgentRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;

	// call_defs ReconfigureSnmpAgent to post the request to ESX server or virtual center
	_vw1__ReconfigureSnmpAgentResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReconfigureSnmpAgent))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostSnmpSystem::SendTestNotification()
{
	vw1__SendTestNotificationRequestType req;
	req._USCOREthis = _mor;

	// call_defs SendTestNotification to post the request to ESX server or virtual center
	_vw1__SendTestNotificationResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SendTestNotification))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__HostSnmpConfigSpec HostSnmpSystem::get_configuration() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("configuration", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostSnmpConfigSpec();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostSnmpConfigSpec*>(it->val);
}

vw1__HostSnmpSystemAgentLimits HostSnmpSystem::get_limits() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("limits", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostSnmpSystemAgentLimits();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostSnmpSystemAgentLimits*>(it->val);
}


const char* HostStorageSystem::type = "HostStorageSystem";
HostStorageSystem::HostStorageSystem(vw1__ManagedObjectReference* mor) : ExtensibleManagedObject(mor) {}
HostStorageSystem::~HostStorageSystem() {}

void HostStorageSystem::AddInternetScsiSendTargets(char* iScsiHbaDevice, int sizetargets, vw1__HostInternetScsiHbaSendTarget**  targets)
{
	vw1__AddInternetScsiSendTargetsRequestType req;
	req._USCOREthis = _mor;
	req.iScsiHbaDevice = iScsiHbaDevice;
	req.__sizetargets = sizetargets;
	req.targets = targets;

	// call_defs AddInternetScsiSendTargets to post the request to ESX server or virtual center
	_vw1__AddInternetScsiSendTargetsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AddInternetScsiSendTargets))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::AddInternetScsiStaticTargets(char* iScsiHbaDevice, int sizetargets, vw1__HostInternetScsiHbaStaticTarget**  targets)
{
	vw1__AddInternetScsiStaticTargetsRequestType req;
	req._USCOREthis = _mor;
	req.iScsiHbaDevice = iScsiHbaDevice;
	req.__sizetargets = sizetargets;
	req.targets = targets;

	// call_defs AddInternetScsiStaticTargets to post the request to ESX server or virtual center
	_vw1__AddInternetScsiStaticTargetsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AddInternetScsiStaticTargets))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::AttachScsiLun(char* lunUuid)
{
	vw1__AttachScsiLunRequestType req;
	req._USCOREthis = _mor;
	req.lunUuid = lunUuid;

	// call_defs AttachScsiLun to post the request to ESX server or virtual center
	_vw1__AttachScsiLunResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AttachScsiLun))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::AttachVmfsExtent(char* vmfsPath, vw1__HostScsiDiskPartition*  extent)
{
	vw1__AttachVmfsExtentRequestType req;
	req._USCOREthis = _mor;
	req.vmfsPath = vmfsPath;
	req.extent = extent;

	// call_defs AttachVmfsExtent to post the request to ESX server or virtual center
	_vw1__AttachVmfsExtentResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AttachVmfsExtent))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

vw1__HostDiskPartitionInfo HostStorageSystem::ComputeDiskPartitionInfo(char* devicePath, vw1__HostDiskPartitionLayout*  layout, char* partitionFormat)
{
	vw1__ComputeDiskPartitionInfoRequestType req;
	req._USCOREthis = _mor;
	req.devicePath = devicePath;
	req.layout = layout;
	req.partitionFormat = partitionFormat;

	// call_defs ComputeDiskPartitionInfo to post the request to ESX server or virtual center
	_vw1__ComputeDiskPartitionInfoResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ComputeDiskPartitionInfo))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__HostDiskPartitionInfo HostStorageSystem::ComputeDiskPartitionInfoForResize(vw1__HostScsiDiskPartition*  partition, vw1__HostDiskPartitionBlockRange*  blockRange, char* partitionFormat)
{
	vw1__ComputeDiskPartitionInfoForResizeRequestType req;
	req._USCOREthis = _mor;
	req.partition = partition;
	req.blockRange = blockRange;
	req.partitionFormat = partitionFormat;

	// call_defs ComputeDiskPartitionInfoForResize to post the request to ESX server or virtual center
	_vw1__ComputeDiskPartitionInfoForResizeResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ComputeDiskPartitionInfoForResize))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

void HostStorageSystem::DetachScsiLun(char* lunUuid)
{
	vw1__DetachScsiLunRequestType req;
	req._USCOREthis = _mor;
	req.lunUuid = lunUuid;

	// call_defs DetachScsiLun to post the request to ESX server or virtual center
	_vw1__DetachScsiLunResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DetachScsiLun))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::DisableMultipathPath(char* pathName)
{
	vw1__DisableMultipathPathRequestType req;
	req._USCOREthis = _mor;
	req.pathName = pathName;

	// call_defs DisableMultipathPath to post the request to ESX server or virtual center
	_vw1__DisableMultipathPathResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DisableMultipathPath))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::DiscoverFcoeHbas(vw1__FcoeConfigFcoeSpecification*  fcoeSpec)
{
	vw1__DiscoverFcoeHbasRequestType req;
	req._USCOREthis = _mor;
	req.fcoeSpec = fcoeSpec;

	// call_defs DiscoverFcoeHbas to post the request to ESX server or virtual center
	_vw1__DiscoverFcoeHbasResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DiscoverFcoeHbas))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::EnableMultipathPath(char* pathName)
{
	vw1__EnableMultipathPathRequestType req;
	req._USCOREthis = _mor;
	req.pathName = pathName;

	// call_defs EnableMultipathPath to post the request to ESX server or virtual center
	_vw1__EnableMultipathPathResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::EnableMultipathPath))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::ExpandVmfsExtent(char* vmfsPath, vw1__HostScsiDiskPartition*  extent)
{
	vw1__ExpandVmfsExtentRequestType req;
	req._USCOREthis = _mor;
	req.vmfsPath = vmfsPath;
	req.extent = extent;

	// call_defs ExpandVmfsExtent to post the request to ESX server or virtual center
	_vw1__ExpandVmfsExtentResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ExpandVmfsExtent))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

vw1__HostVmfsVolume HostStorageSystem::FormatVmfs(vw1__HostVmfsSpec*  createSpec)
{
	vw1__FormatVmfsRequestType req;
	req._USCOREthis = _mor;
	req.createSpec = createSpec;

	// call_defs FormatVmfs to post the request to ESX server or virtual center
	_vw1__FormatVmfsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::FormatVmfs))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

void HostStorageSystem::MarkForRemoval(char* hbaName, bool remove)
{
	vw1__MarkForRemovalRequestType req;
	req._USCOREthis = _mor;
	req.hbaName = hbaName;
	req.remove = remove;

	// call_defs MarkForRemoval to post the request to ESX server or virtual center
	_vw1__MarkForRemovalResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MarkForRemoval))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::MountVmfsVolume(char* vmfsUuid)
{
	vw1__MountVmfsVolumeRequestType req;
	req._USCOREthis = _mor;
	req.vmfsUuid = vmfsUuid;

	// call_defs MountVmfsVolume to post the request to ESX server or virtual center
	_vw1__MountVmfsVolumeResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MountVmfsVolume))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

std::vector<vw1__HostPathSelectionPolicyOption> HostStorageSystem::QueryPathSelectionPolicyOptions()
{
	vw1__QueryPathSelectionPolicyOptionsRequestType req;
	req._USCOREthis = _mor;

	// call_defs QueryPathSelectionPolicyOptions to post the request to ESX server or virtual center
	_vw1__QueryPathSelectionPolicyOptionsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryPathSelectionPolicyOptions))
	{
		std::vector<vw1__HostPathSelectionPolicyOption> vw1__hostpathselectionpolicyoptions;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__HostPathSelectionPolicyOption tmp(*rsp.returnval[i]);
			vw1__hostpathselectionpolicyoptions.push_back(tmp);
		}
		return vw1__hostpathselectionpolicyoptions;
	}

	throw get_last_error();
}

std::vector<vw1__HostStorageArrayTypePolicyOption> HostStorageSystem::QueryStorageArrayTypePolicyOptions()
{
	vw1__QueryStorageArrayTypePolicyOptionsRequestType req;
	req._USCOREthis = _mor;

	// call_defs QueryStorageArrayTypePolicyOptions to post the request to ESX server or virtual center
	_vw1__QueryStorageArrayTypePolicyOptionsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryStorageArrayTypePolicyOptions))
	{
		std::vector<vw1__HostStorageArrayTypePolicyOption> vw1__hoststoragearraytypepolicyoptions;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__HostStorageArrayTypePolicyOption tmp(*rsp.returnval[i]);
			vw1__hoststoragearraytypepolicyoptions.push_back(tmp);
		}
		return vw1__hoststoragearraytypepolicyoptions;
	}

	throw get_last_error();
}

std::vector<vw1__HostUnresolvedVmfsVolume> HostStorageSystem::QueryUnresolvedVmfsVolume()
{
	vw1__QueryUnresolvedVmfsVolumeRequestType req;
	req._USCOREthis = _mor;

	// call_defs QueryUnresolvedVmfsVolume to post the request to ESX server or virtual center
	_vw1__QueryUnresolvedVmfsVolumeResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryUnresolvedVmfsVolume))
	{
		std::vector<vw1__HostUnresolvedVmfsVolume> vw1__hostunresolvedvmfsvolumes;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__HostUnresolvedVmfsVolume tmp(*rsp.returnval[i]);
			vw1__hostunresolvedvmfsvolumes.push_back(tmp);
		}
		return vw1__hostunresolvedvmfsvolumes;
	}

	throw get_last_error();
}

void HostStorageSystem::RefreshStorageSystem()
{
	vw1__RefreshStorageSystemRequestType req;
	req._USCOREthis = _mor;

	// call_defs RefreshStorageSystem to post the request to ESX server or virtual center
	_vw1__RefreshStorageSystemResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RefreshStorageSystem))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::RemoveInternetScsiSendTargets(char* iScsiHbaDevice, int sizetargets, vw1__HostInternetScsiHbaSendTarget**  targets)
{
	vw1__RemoveInternetScsiSendTargetsRequestType req;
	req._USCOREthis = _mor;
	req.iScsiHbaDevice = iScsiHbaDevice;
	req.__sizetargets = sizetargets;
	req.targets = targets;

	// call_defs RemoveInternetScsiSendTargets to post the request to ESX server or virtual center
	_vw1__RemoveInternetScsiSendTargetsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemoveInternetScsiSendTargets))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::RemoveInternetScsiStaticTargets(char* iScsiHbaDevice, int sizetargets, vw1__HostInternetScsiHbaStaticTarget**  targets)
{
	vw1__RemoveInternetScsiStaticTargetsRequestType req;
	req._USCOREthis = _mor;
	req.iScsiHbaDevice = iScsiHbaDevice;
	req.__sizetargets = sizetargets;
	req.targets = targets;

	// call_defs RemoveInternetScsiStaticTargets to post the request to ESX server or virtual center
	_vw1__RemoveInternetScsiStaticTargetsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemoveInternetScsiStaticTargets))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::RescanAllHba()
{
	vw1__RescanAllHbaRequestType req;
	req._USCOREthis = _mor;

	// call_defs RescanAllHba to post the request to ESX server or virtual center
	_vw1__RescanAllHbaResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RescanAllHba))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::RescanHba(char* hbaDevice)
{
	vw1__RescanHbaRequestType req;
	req._USCOREthis = _mor;
	req.hbaDevice = hbaDevice;

	// call_defs RescanHba to post the request to ESX server or virtual center
	_vw1__RescanHbaResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RescanHba))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::RescanVmfs()
{
	vw1__RescanVmfsRequestType req;
	req._USCOREthis = _mor;

	// call_defs RescanVmfs to post the request to ESX server or virtual center
	_vw1__RescanVmfsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RescanVmfs))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

std::vector<vw1__HostUnresolvedVmfsResolutionResult> HostStorageSystem::ResolveMultipleUnresolvedVmfsVolumes(int sizeresolutionSpec, vw1__HostUnresolvedVmfsResolutionSpec**  resolutionSpec)
{
	vw1__ResolveMultipleUnresolvedVmfsVolumesRequestType req;
	req._USCOREthis = _mor;
	req.__sizeresolutionSpec = sizeresolutionSpec;
	req.resolutionSpec = resolutionSpec;

	// call_defs ResolveMultipleUnresolvedVmfsVolumes to post the request to ESX server or virtual center
	_vw1__ResolveMultipleUnresolvedVmfsVolumesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ResolveMultipleUnresolvedVmfsVolumes))
	{
		std::vector<vw1__HostUnresolvedVmfsResolutionResult> vw1__hostunresolvedvmfsresolutionresults;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__HostUnresolvedVmfsResolutionResult tmp(*rsp.returnval[i]);
			vw1__hostunresolvedvmfsresolutionresults.push_back(tmp);
		}
		return vw1__hostunresolvedvmfsresolutionresults;
	}

	throw get_last_error();
}

std::vector<vw1__HostDiskPartitionInfo> HostStorageSystem::RetrieveDiskPartitionInfo(int sizedevicePath, char** devicePath)
{
	vw1__RetrieveDiskPartitionInfoRequestType req;
	req._USCOREthis = _mor;
	req.__sizedevicePath = sizedevicePath;
	req.devicePath = devicePath;

	// call_defs RetrieveDiskPartitionInfo to post the request to ESX server or virtual center
	_vw1__RetrieveDiskPartitionInfoResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RetrieveDiskPartitionInfo))
	{
		std::vector<vw1__HostDiskPartitionInfo> vw1__hostdiskpartitioninfos;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__HostDiskPartitionInfo tmp(*rsp.returnval[i]);
			vw1__hostdiskpartitioninfos.push_back(tmp);
		}
		return vw1__hostdiskpartitioninfos;
	}

	throw get_last_error();
}

void HostStorageSystem::SetMultipathLunPolicy(char* lunId, vw1__HostMultipathInfoLogicalUnitPolicy*  policy)
{
	vw1__SetMultipathLunPolicyRequestType req;
	req._USCOREthis = _mor;
	req.lunId = lunId;
	req.policy = policy;

	// call_defs SetMultipathLunPolicy to post the request to ESX server or virtual center
	_vw1__SetMultipathLunPolicyResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SetMultipathLunPolicy))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::UnmountForceMountedVmfsVolume(char* vmfsUuid)
{
	vw1__UnmountForceMountedVmfsVolumeRequestType req;
	req._USCOREthis = _mor;
	req.vmfsUuid = vmfsUuid;

	// call_defs UnmountForceMountedVmfsVolume to post the request to ESX server or virtual center
	_vw1__UnmountForceMountedVmfsVolumeResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UnmountForceMountedVmfsVolume))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::UnmountVmfsVolume(char* vmfsUuid)
{
	vw1__UnmountVmfsVolumeRequestType req;
	req._USCOREthis = _mor;
	req.vmfsUuid = vmfsUuid;

	// call_defs UnmountVmfsVolume to post the request to ESX server or virtual center
	_vw1__UnmountVmfsVolumeResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UnmountVmfsVolume))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::UpdateDiskPartitions(char* devicePath, vw1__HostDiskPartitionSpec*  spec)
{
	vw1__UpdateDiskPartitionsRequestType req;
	req._USCOREthis = _mor;
	req.devicePath = devicePath;
	req.spec = spec;

	// call_defs UpdateDiskPartitions to post the request to ESX server or virtual center
	_vw1__UpdateDiskPartitionsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateDiskPartitions))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::UpdateInternetScsiAdvancedOptions(char* iScsiHbaDevice, vw1__HostInternetScsiHbaTargetSet*  targetSet, int sizeoptions, vw1__HostInternetScsiHbaParamValue**  options)
{
	vw1__UpdateInternetScsiAdvancedOptionsRequestType req;
	req._USCOREthis = _mor;
	req.iScsiHbaDevice = iScsiHbaDevice;
	req.targetSet = targetSet;
	req.__sizeoptions = sizeoptions;
	req.options = options;

	// call_defs UpdateInternetScsiAdvancedOptions to post the request to ESX server or virtual center
	_vw1__UpdateInternetScsiAdvancedOptionsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateInternetScsiAdvancedOptions))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::UpdateInternetScsiAlias(char* iScsiHbaDevice, char* iScsiAlias)
{
	vw1__UpdateInternetScsiAliasRequestType req;
	req._USCOREthis = _mor;
	req.iScsiHbaDevice = iScsiHbaDevice;
	req.iScsiAlias = iScsiAlias;

	// call_defs UpdateInternetScsiAlias to post the request to ESX server or virtual center
	_vw1__UpdateInternetScsiAliasResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateInternetScsiAlias))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::UpdateInternetScsiAuthenticationProperties(char* iScsiHbaDevice, vw1__HostInternetScsiHbaAuthenticationProperties*  authenticationProperties, vw1__HostInternetScsiHbaTargetSet*  targetSet)
{
	vw1__UpdateInternetScsiAuthenticationPropertiesRequestType req;
	req._USCOREthis = _mor;
	req.iScsiHbaDevice = iScsiHbaDevice;
	req.authenticationProperties = authenticationProperties;
	req.targetSet = targetSet;

	// call_defs UpdateInternetScsiAuthenticationProperties to post the request to ESX server or virtual center
	_vw1__UpdateInternetScsiAuthenticationPropertiesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateInternetScsiAuthenticationProperties))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::UpdateInternetScsiDigestProperties(char* iScsiHbaDevice, vw1__HostInternetScsiHbaTargetSet*  targetSet, vw1__HostInternetScsiHbaDigestProperties*  digestProperties)
{
	vw1__UpdateInternetScsiDigestPropertiesRequestType req;
	req._USCOREthis = _mor;
	req.iScsiHbaDevice = iScsiHbaDevice;
	req.targetSet = targetSet;
	req.digestProperties = digestProperties;

	// call_defs UpdateInternetScsiDigestProperties to post the request to ESX server or virtual center
	_vw1__UpdateInternetScsiDigestPropertiesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateInternetScsiDigestProperties))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::UpdateInternetScsiDiscoveryProperties(char* iScsiHbaDevice, vw1__HostInternetScsiHbaDiscoveryProperties*  discoveryProperties)
{
	vw1__UpdateInternetScsiDiscoveryPropertiesRequestType req;
	req._USCOREthis = _mor;
	req.iScsiHbaDevice = iScsiHbaDevice;
	req.discoveryProperties = discoveryProperties;

	// call_defs UpdateInternetScsiDiscoveryProperties to post the request to ESX server or virtual center
	_vw1__UpdateInternetScsiDiscoveryPropertiesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateInternetScsiDiscoveryProperties))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::UpdateInternetScsiIPProperties(char* iScsiHbaDevice, vw1__HostInternetScsiHbaIPProperties*  ipProperties)
{
	vw1__UpdateInternetScsiIPPropertiesRequestType req;
	req._USCOREthis = _mor;
	req.iScsiHbaDevice = iScsiHbaDevice;
	req.ipProperties = ipProperties;

	// call_defs UpdateInternetScsiIPProperties to post the request to ESX server or virtual center
	_vw1__UpdateInternetScsiIPPropertiesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateInternetScsiIPProperties))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::UpdateInternetScsiName(char* iScsiHbaDevice, char* iScsiName)
{
	vw1__UpdateInternetScsiNameRequestType req;
	req._USCOREthis = _mor;
	req.iScsiHbaDevice = iScsiHbaDevice;
	req.iScsiName = iScsiName;

	// call_defs UpdateInternetScsiName to post the request to ESX server or virtual center
	_vw1__UpdateInternetScsiNameResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateInternetScsiName))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::UpdateScsiLunDisplayName(char* lunUuid, char* displayName)
{
	vw1__UpdateScsiLunDisplayNameRequestType req;
	req._USCOREthis = _mor;
	req.lunUuid = lunUuid;
	req.displayName = displayName;

	// call_defs UpdateScsiLunDisplayName to post the request to ESX server or virtual center
	_vw1__UpdateScsiLunDisplayNameResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateScsiLunDisplayName))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::UpdateSoftwareInternetScsiEnabled(bool enabled)
{
	vw1__UpdateSoftwareInternetScsiEnabledRequestType req;
	req._USCOREthis = _mor;
	req.enabled = enabled;

	// call_defs UpdateSoftwareInternetScsiEnabled to post the request to ESX server or virtual center
	_vw1__UpdateSoftwareInternetScsiEnabledResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateSoftwareInternetScsiEnabled))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::UpgradeVmfs(char* vmfsPath)
{
	vw1__UpgradeVmfsRequestType req;
	req._USCOREthis = _mor;
	req.vmfsPath = vmfsPath;

	// call_defs UpgradeVmfs to post the request to ESX server or virtual center
	_vw1__UpgradeVmfsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpgradeVmfs))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostStorageSystem::UpgradeVmLayout()
{
	vw1__UpgradeVmLayoutRequestType req;
	req._USCOREthis = _mor;

	// call_defs UpgradeVmLayout to post the request to ESX server or virtual center
	_vw1__UpgradeVmLayoutResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpgradeVmLayout))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__HostFileSystemVolumeInfo HostStorageSystem::get_fileSystemVolumeInfo() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("fileSystemVolumeInfo", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostFileSystemVolumeInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostFileSystemVolumeInfo*>(it->val);
}

vw1__HostMultipathStateInfo HostStorageSystem::get_multipathStateInfo() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("multipathStateInfo", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostMultipathStateInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostMultipathStateInfo*>(it->val);
}

vw1__HostStorageDeviceInfo HostStorageSystem::get_storageDeviceInfo() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("storageDeviceInfo", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostStorageDeviceInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostStorageDeviceInfo*>(it->val);
}

std::vector<std::string> HostStorageSystem::get_systemFile() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("systemFile", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<std::string>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfString* amo = dynamic_cast<vw1__ArrayOfString*>(it->val);
	std::vector<std::string> ret;
	for (int i = 0; i < amo->__sizestring; ++i)
		ret.push_back(amo->string[i]);
	return ret;
}


const char* HostVMotionSystem::type = "HostVMotionSystem";
HostVMotionSystem::HostVMotionSystem(vw1__ManagedObjectReference* mor) : ExtensibleManagedObject(mor) {}
HostVMotionSystem::~HostVMotionSystem() {}

void HostVMotionSystem::DeselectVnic()
{
	vw1__DeselectVnicRequestType req;
	req._USCOREthis = _mor;

	// call_defs DeselectVnic to post the request to ESX server or virtual center
	_vw1__DeselectVnicResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DeselectVnic))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostVMotionSystem::SelectVnic(char* device)
{
	vw1__SelectVnicRequestType req;
	req._USCOREthis = _mor;
	req.device = device;

	// call_defs SelectVnic to post the request to ESX server or virtual center
	_vw1__SelectVnicResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SelectVnic))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostVMotionSystem::UpdateIpConfig(vw1__HostIpConfig*  ipConfig)
{
	vw1__UpdateIpConfigRequestType req;
	req._USCOREthis = _mor;
	req.ipConfig = ipConfig;

	// call_defs UpdateIpConfig to post the request to ESX server or virtual center
	_vw1__UpdateIpConfigResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateIpConfig))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__HostIpConfig HostVMotionSystem::get_ipConfig() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("ipConfig", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostIpConfig();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostIpConfig*>(it->val);
}

vw1__HostVMotionNetConfig HostVMotionSystem::get_netConfig() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("netConfig", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostVMotionNetConfig();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostVMotionNetConfig*>(it->val);
}


const char* HostVirtualNicManager::type = "HostVirtualNicManager";
HostVirtualNicManager::HostVirtualNicManager(vw1__ManagedObjectReference* mor) : ExtensibleManagedObject(mor) {}
HostVirtualNicManager::~HostVirtualNicManager() {}

void HostVirtualNicManager::DeselectVnicForNicType(char* nicType, char* device)
{
	vw1__DeselectVnicForNicTypeRequestType req;
	req._USCOREthis = _mor;
	req.nicType = nicType;
	req.device = device;

	// call_defs DeselectVnicForNicType to post the request to ESX server or virtual center
	_vw1__DeselectVnicForNicTypeResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DeselectVnicForNicType))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

vw1__VirtualNicManagerNetConfig HostVirtualNicManager::QueryNetConfig(char* nicType)
{
	vw1__QueryNetConfigRequestType req;
	req._USCOREthis = _mor;
	req.nicType = nicType;

	// call_defs QueryNetConfig to post the request to ESX server or virtual center
	_vw1__QueryNetConfigResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryNetConfig))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

void HostVirtualNicManager::SelectVnicForNicType(char* nicType, char* device)
{
	vw1__SelectVnicForNicTypeRequestType req;
	req._USCOREthis = _mor;
	req.nicType = nicType;
	req.device = device;

	// call_defs SelectVnicForNicType to post the request to ESX server or virtual center
	_vw1__SelectVnicForNicTypeResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SelectVnicForNicType))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__HostVirtualNicManagerInfo HostVirtualNicManager::get_info() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("info", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostVirtualNicManagerInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostVirtualNicManagerInfo*>(it->val);
}


const char* HttpNfcLease::type = "HttpNfcLease";
HttpNfcLease::HttpNfcLease(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
HttpNfcLease::~HttpNfcLease() {}

void HttpNfcLease::HttpNfcLeaseAbort(vw1__LocalizedMethodFault*  fault)
{
	vw1__HttpNfcLeaseAbortRequestType req;
	req._USCOREthis = _mor;
	req.fault = fault;

	// call_defs HttpNfcLeaseAbort to post the request to ESX server or virtual center
	_vw1__HttpNfcLeaseAbortResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::HttpNfcLeaseAbort))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HttpNfcLease::HttpNfcLeaseComplete()
{
	vw1__HttpNfcLeaseCompleteRequestType req;
	req._USCOREthis = _mor;

	// call_defs HttpNfcLeaseComplete to post the request to ESX server or virtual center
	_vw1__HttpNfcLeaseCompleteResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::HttpNfcLeaseComplete))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

std::vector<vw1__HttpNfcLeaseManifestEntry> HttpNfcLease::HttpNfcLeaseGetManifest()
{
	vw1__HttpNfcLeaseGetManifestRequestType req;
	req._USCOREthis = _mor;

	// call_defs HttpNfcLeaseGetManifest to post the request to ESX server or virtual center
	_vw1__HttpNfcLeaseGetManifestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::HttpNfcLeaseGetManifest))
	{
		std::vector<vw1__HttpNfcLeaseManifestEntry> vw1__httpnfcleasemanifestentrys;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__HttpNfcLeaseManifestEntry tmp(*rsp.returnval[i]);
			vw1__httpnfcleasemanifestentrys.push_back(tmp);
		}
		return vw1__httpnfcleasemanifestentrys;
	}

	throw get_last_error();
}

void HttpNfcLease::HttpNfcLeaseProgress(int percent)
{
	vw1__HttpNfcLeaseProgressRequestType req;
	req._USCOREthis = _mor;
	req.percent = percent;

	// call_defs HttpNfcLeaseProgress to post the request to ESX server or virtual center
	_vw1__HttpNfcLeaseProgressResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::HttpNfcLeaseProgress))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__LocalizedMethodFault HttpNfcLease::get_error() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("error", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__LocalizedMethodFault();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__LocalizedMethodFault*>(it->val);
}

vw1__HttpNfcLeaseInfo HttpNfcLease::get_info() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("info", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HttpNfcLeaseInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HttpNfcLeaseInfo*>(it->val);
}

int HttpNfcLease::get_initializeProgress() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("initializeProgress", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return int();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<xsd__int*>(it->val)->__item;
}

vw1__HttpNfcLeaseState HttpNfcLease::get_state() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("state", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HttpNfcLeaseState();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<vw1__HttpNfcLeaseState_*>(it->val)->__item;
}


const char* IpPoolManager::type = "IpPoolManager";
IpPoolManager::IpPoolManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
IpPoolManager::~IpPoolManager() {}

int IpPoolManager::CreateIpPool(Datacenter dc, vw1__IpPool*  pool)
{
	vw1__CreateIpPoolRequestType req;
	req._USCOREthis = _mor;
	req.dc = dc;
	req.pool = pool;

	// call_defs CreateIpPool to post the request to ESX server or virtual center
	_vw1__CreateIpPoolResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateIpPool))
	{
		return static_cast<int>(rsp.returnval);
	}

	throw get_last_error();
}

void IpPoolManager::DestroyIpPool(Datacenter dc, int id, bool force)
{
	vw1__DestroyIpPoolRequestType req;
	req._USCOREthis = _mor;
	req.dc = dc;
	req.id = id;
	req.force = force;

	// call_defs DestroyIpPool to post the request to ESX server or virtual center
	_vw1__DestroyIpPoolResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DestroyIpPool))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

std::vector<vw1__IpPool> IpPoolManager::QueryIpPools(Datacenter dc)
{
	vw1__QueryIpPoolsRequestType req;
	req._USCOREthis = _mor;
	req.dc = dc;

	// call_defs QueryIpPools to post the request to ESX server or virtual center
	_vw1__QueryIpPoolsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryIpPools))
	{
		std::vector<vw1__IpPool> vw1__ippools;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__IpPool tmp(*rsp.returnval[i]);
			vw1__ippools.push_back(tmp);
		}
		return vw1__ippools;
	}

	throw get_last_error();
}

void IpPoolManager::UpdateIpPool(Datacenter dc, vw1__IpPool*  pool)
{
	vw1__UpdateIpPoolRequestType req;
	req._USCOREthis = _mor;
	req.dc = dc;
	req.pool = pool;

	// call_defs UpdateIpPool to post the request to ESX server or virtual center
	_vw1__UpdateIpPoolResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateIpPool))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

const char* IscsiManager::type = "IscsiManager";
IscsiManager::IscsiManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
IscsiManager::~IscsiManager() {}

void IscsiManager::BindVnic(char* iScsiHbaName, char* vnicDevice)
{
	vw1__BindVnicRequestType req;
	req._USCOREthis = _mor;
	req.iScsiHbaName = iScsiHbaName;
	req.vnicDevice = vnicDevice;

	// call_defs BindVnic to post the request to ESX server or virtual center
	_vw1__BindVnicResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::BindVnic))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

std::vector<vw1__IscsiPortInfo> IscsiManager::QueryBoundVnics(char* iScsiHbaName)
{
	vw1__QueryBoundVnicsRequestType req;
	req._USCOREthis = _mor;
	req.iScsiHbaName = iScsiHbaName;

	// call_defs QueryBoundVnics to post the request to ESX server or virtual center
	_vw1__QueryBoundVnicsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryBoundVnics))
	{
		std::vector<vw1__IscsiPortInfo> vw1__iscsiportinfos;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__IscsiPortInfo tmp(*rsp.returnval[i]);
			vw1__iscsiportinfos.push_back(tmp);
		}
		return vw1__iscsiportinfos;
	}

	throw get_last_error();
}

std::vector<vw1__IscsiPortInfo> IscsiManager::QueryCandidateNics(char* iScsiHbaName)
{
	vw1__QueryCandidateNicsRequestType req;
	req._USCOREthis = _mor;
	req.iScsiHbaName = iScsiHbaName;

	// call_defs QueryCandidateNics to post the request to ESX server or virtual center
	_vw1__QueryCandidateNicsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryCandidateNics))
	{
		std::vector<vw1__IscsiPortInfo> vw1__iscsiportinfos;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__IscsiPortInfo tmp(*rsp.returnval[i]);
			vw1__iscsiportinfos.push_back(tmp);
		}
		return vw1__iscsiportinfos;
	}

	throw get_last_error();
}

vw1__IscsiMigrationDependency IscsiManager::QueryMigrationDependencies(int sizepnicDevice, char** pnicDevice)
{
	vw1__QueryMigrationDependenciesRequestType req;
	req._USCOREthis = _mor;
	req.__sizepnicDevice = sizepnicDevice;
	req.pnicDevice = pnicDevice;

	// call_defs QueryMigrationDependencies to post the request to ESX server or virtual center
	_vw1__QueryMigrationDependenciesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryMigrationDependencies))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__IscsiStatus IscsiManager::QueryPnicStatus(char* pnicDevice)
{
	vw1__QueryPnicStatusRequestType req;
	req._USCOREthis = _mor;
	req.pnicDevice = pnicDevice;

	// call_defs QueryPnicStatus to post the request to ESX server or virtual center
	_vw1__QueryPnicStatusResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryPnicStatus))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__IscsiStatus IscsiManager::QueryVnicStatus(char* vnicDevice)
{
	vw1__QueryVnicStatusRequestType req;
	req._USCOREthis = _mor;
	req.vnicDevice = vnicDevice;

	// call_defs QueryVnicStatus to post the request to ESX server or virtual center
	_vw1__QueryVnicStatusResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryVnicStatus))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

void IscsiManager::UnbindVnic(char* iScsiHbaName, char* vnicDevice, bool force)
{
	vw1__UnbindVnicRequestType req;
	req._USCOREthis = _mor;
	req.iScsiHbaName = iScsiHbaName;
	req.vnicDevice = vnicDevice;
	req.force = force;

	// call_defs UnbindVnic to post the request to ESX server or virtual center
	_vw1__UnbindVnicResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UnbindVnic))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

const char* LicenseAssignmentManager::type = "LicenseAssignmentManager";
LicenseAssignmentManager::LicenseAssignmentManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
LicenseAssignmentManager::~LicenseAssignmentManager() {}

std::vector<vw1__LicenseAssignmentManagerLicenseAssignment> LicenseAssignmentManager::QueryAssignedLicenses(char* entityId)
{
	vw1__QueryAssignedLicensesRequestType req;
	req._USCOREthis = _mor;
	req.entityId = entityId;

	// call_defs QueryAssignedLicenses to post the request to ESX server or virtual center
	_vw1__QueryAssignedLicensesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryAssignedLicenses))
	{
		std::vector<vw1__LicenseAssignmentManagerLicenseAssignment> vw1__licenseassignmentmanagerlicenseassignments;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__LicenseAssignmentManagerLicenseAssignment tmp(*rsp.returnval[i]);
			vw1__licenseassignmentmanagerlicenseassignments.push_back(tmp);
		}
		return vw1__licenseassignmentmanagerlicenseassignments;
	}

	throw get_last_error();
}

void LicenseAssignmentManager::RemoveAssignedLicense(char* entityId)
{
	vw1__RemoveAssignedLicenseRequestType req;
	req._USCOREthis = _mor;
	req.entityId = entityId;

	// call_defs RemoveAssignedLicense to post the request to ESX server or virtual center
	_vw1__RemoveAssignedLicenseResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemoveAssignedLicense))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

vw1__LicenseManagerLicenseInfo LicenseAssignmentManager::UpdateAssignedLicense(char* entity, char* licenseKey, char* entityDisplayName)
{
	vw1__UpdateAssignedLicenseRequestType req;
	req._USCOREthis = _mor;
	req.entity = entity;
	req.licenseKey = licenseKey;
	req.entityDisplayName = entityDisplayName;

	// call_defs UpdateAssignedLicense to post the request to ESX server or virtual center
	_vw1__UpdateAssignedLicenseResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateAssignedLicense))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

const char* LicenseManager::type = "LicenseManager";
LicenseManager::LicenseManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
LicenseManager::~LicenseManager() {}

vw1__LicenseManagerLicenseInfo LicenseManager::AddLicense(char* licenseKey, int sizelabels, vw1__KeyValue**  labels)
{
	vw1__AddLicenseRequestType req;
	req._USCOREthis = _mor;
	req.licenseKey = licenseKey;
	req.__sizelabels = sizelabels;
	req.labels = labels;

	// call_defs AddLicense to post the request to ESX server or virtual center
	_vw1__AddLicenseResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AddLicense))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

bool LicenseManager::CheckLicenseFeature(HostSystem host, char* featureKey)
{
	vw1__CheckLicenseFeatureRequestType req;
	req._USCOREthis = _mor;
	req.host = host;
	req.featureKey = featureKey;

	// call_defs CheckLicenseFeature to post the request to ESX server or virtual center
	_vw1__CheckLicenseFeatureResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CheckLicenseFeature))
	{
		return static_cast<bool>(rsp.returnval);
	}

	throw get_last_error();
}

void LicenseManager::ConfigureLicenseSource(HostSystem host, vw1__LicenseSource*  licenseSource)
{
	vw1__ConfigureLicenseSourceRequestType req;
	req._USCOREthis = _mor;
	req.host = host;
	req.licenseSource = licenseSource;

	// call_defs ConfigureLicenseSource to post the request to ESX server or virtual center
	_vw1__ConfigureLicenseSourceResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ConfigureLicenseSource))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

vw1__LicenseManagerLicenseInfo LicenseManager::DecodeLicense(char* licenseKey)
{
	vw1__DecodeLicenseRequestType req;
	req._USCOREthis = _mor;
	req.licenseKey = licenseKey;

	// call_defs DecodeLicense to post the request to ESX server or virtual center
	_vw1__DecodeLicenseResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DecodeLicense))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

bool LicenseManager::DisableFeature(HostSystem host, char* featureKey)
{
	vw1__DisableFeatureRequestType req;
	req._USCOREthis = _mor;
	req.host = host;
	req.featureKey = featureKey;

	// call_defs DisableFeature to post the request to ESX server or virtual center
	_vw1__DisableFeatureResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DisableFeature))
	{
		return static_cast<bool>(rsp.returnval);
	}

	throw get_last_error();
}

bool LicenseManager::EnableFeature(HostSystem host, char* featureKey)
{
	vw1__EnableFeatureRequestType req;
	req._USCOREthis = _mor;
	req.host = host;
	req.featureKey = featureKey;

	// call_defs EnableFeature to post the request to ESX server or virtual center
	_vw1__EnableFeatureResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::EnableFeature))
	{
		return static_cast<bool>(rsp.returnval);
	}

	throw get_last_error();
}

std::vector<vw1__LicenseAvailabilityInfo> LicenseManager::QueryLicenseSourceAvailability(HostSystem host)
{
	vw1__QueryLicenseSourceAvailabilityRequestType req;
	req._USCOREthis = _mor;
	req.host = host;

	// call_defs QueryLicenseSourceAvailability to post the request to ESX server or virtual center
	_vw1__QueryLicenseSourceAvailabilityResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryLicenseSourceAvailability))
	{
		std::vector<vw1__LicenseAvailabilityInfo> vw1__licenseavailabilityinfos;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__LicenseAvailabilityInfo tmp(*rsp.returnval[i]);
			vw1__licenseavailabilityinfos.push_back(tmp);
		}
		return vw1__licenseavailabilityinfos;
	}

	throw get_last_error();
}

vw1__LicenseUsageInfo LicenseManager::QueryLicenseUsage(HostSystem host)
{
	vw1__QueryLicenseUsageRequestType req;
	req._USCOREthis = _mor;
	req.host = host;

	// call_defs QueryLicenseUsage to post the request to ESX server or virtual center
	_vw1__QueryLicenseUsageResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryLicenseUsage))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

std::vector<vw1__LicenseFeatureInfo> LicenseManager::QuerySupportedFeatures(HostSystem host)
{
	vw1__QuerySupportedFeaturesRequestType req;
	req._USCOREthis = _mor;
	req.host = host;

	// call_defs QuerySupportedFeatures to post the request to ESX server or virtual center
	_vw1__QuerySupportedFeaturesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QuerySupportedFeatures))
	{
		std::vector<vw1__LicenseFeatureInfo> vw1__licensefeatureinfos;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__LicenseFeatureInfo tmp(*rsp.returnval[i]);
			vw1__licensefeatureinfos.push_back(tmp);
		}
		return vw1__licensefeatureinfos;
	}

	throw get_last_error();
}

void LicenseManager::RemoveLicense(char* licenseKey)
{
	vw1__RemoveLicenseRequestType req;
	req._USCOREthis = _mor;
	req.licenseKey = licenseKey;

	// call_defs RemoveLicense to post the request to ESX server or virtual center
	_vw1__RemoveLicenseResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemoveLicense))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void LicenseManager::RemoveLicenseLabel(char* licenseKey, char* labelKey)
{
	vw1__RemoveLicenseLabelRequestType req;
	req._USCOREthis = _mor;
	req.licenseKey = licenseKey;
	req.labelKey = labelKey;

	// call_defs RemoveLicenseLabel to post the request to ESX server or virtual center
	_vw1__RemoveLicenseLabelResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemoveLicenseLabel))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void LicenseManager::SetLicenseEdition(HostSystem host, char* featureKey)
{
	vw1__SetLicenseEditionRequestType req;
	req._USCOREthis = _mor;
	req.host = host;
	req.featureKey = featureKey;

	// call_defs SetLicenseEdition to post the request to ESX server or virtual center
	_vw1__SetLicenseEditionResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SetLicenseEdition))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

vw1__LicenseManagerLicenseInfo LicenseManager::UpdateLicense(char* licenseKey, int sizelabels, vw1__KeyValue**  labels)
{
	vw1__UpdateLicenseRequestType req;
	req._USCOREthis = _mor;
	req.licenseKey = licenseKey;
	req.__sizelabels = sizelabels;
	req.labels = labels;

	// call_defs UpdateLicense to post the request to ESX server or virtual center
	_vw1__UpdateLicenseResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateLicense))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

void LicenseManager::UpdateLicenseLabel(char* licenseKey, char* labelKey, char* labelValue)
{
	vw1__UpdateLicenseLabelRequestType req;
	req._USCOREthis = _mor;
	req.licenseKey = licenseKey;
	req.labelKey = labelKey;
	req.labelValue = labelValue;

	// call_defs UpdateLicenseLabel to post the request to ESX server or virtual center
	_vw1__UpdateLicenseLabelResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateLicenseLabel))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__LicenseDiagnostics LicenseManager::get_diagnostics() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("diagnostics", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__LicenseDiagnostics();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__LicenseDiagnostics*>(it->val);
}

vw1__LicenseManagerEvaluationInfo LicenseManager::get_evaluation() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("evaluation", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__LicenseManagerEvaluationInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__LicenseManagerEvaluationInfo*>(it->val);
}

std::vector<vw1__LicenseFeatureInfo> LicenseManager::get_featureInfo() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("featureInfo", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__LicenseFeatureInfo>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfLicenseFeatureInfo* amo = dynamic_cast<vw1__ArrayOfLicenseFeatureInfo*>(it->val);
	std::vector<vw1__LicenseFeatureInfo> ret;
	for (int i = 0; i < amo->__sizeLicenseFeatureInfo; ++i)
		ret.push_back(*amo->LicenseFeatureInfo[i]);
	return ret;
}

LicenseAssignmentManager LicenseManager::get_licenseAssignmentManager() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("licenseAssignmentManager", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return LicenseAssignmentManager();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return LicenseAssignmentManager(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}

std::string LicenseManager::get_licensedEdition() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("licensedEdition", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::string();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<xsd__string*>(it->val)->__item;
}

std::vector<vw1__LicenseManagerLicenseInfo> LicenseManager::get_licenses() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("licenses", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__LicenseManagerLicenseInfo>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfLicenseManagerLicenseInfo* amo = dynamic_cast<vw1__ArrayOfLicenseManagerLicenseInfo*>(it->val);
	std::vector<vw1__LicenseManagerLicenseInfo> ret;
	for (int i = 0; i < amo->__sizeLicenseManagerLicenseInfo; ++i)
		ret.push_back(*amo->LicenseManagerLicenseInfo[i]);
	return ret;
}

vw1__LicenseSource LicenseManager::get_source() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("source", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__LicenseSource();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__LicenseSource*>(it->val);
}

bool LicenseManager::get_sourceAvailable() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("sourceAvailable", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return bool();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<xsd__boolean*>(it->val)->__item;
}


const char* LocalizationManager::type = "LocalizationManager";
LocalizationManager::LocalizationManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
LocalizationManager::~LocalizationManager() {}
std::vector<vw1__LocalizationManagerMessageCatalog> LocalizationManager::get_catalog() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("catalog", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__LocalizationManagerMessageCatalog>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfLocalizationManagerMessageCatalog* amo = dynamic_cast<vw1__ArrayOfLocalizationManagerMessageCatalog*>(it->val);
	std::vector<vw1__LocalizationManagerMessageCatalog> ret;
	for (int i = 0; i < amo->__sizeLocalizationManagerMessageCatalog; ++i)
		ret.push_back(*amo->LocalizationManagerMessageCatalog[i]);
	return ret;
}


const char* ManagedEntity::type = "ManagedEntity";
ManagedEntity::ManagedEntity(vw1__ManagedObjectReference* mor) : ExtensibleManagedObject(mor) {}
ManagedEntity::~ManagedEntity() {}

Task ManagedEntity::Destroy_Task()
{
	vw1__DestroyRequestType req;
	req._USCOREthis = _mor;

	// call_defs Destroy_USCORETask to post the request to ESX server or virtual center
	_vw1__Destroy_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::Destroy_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void ManagedEntity::Reload()
{
	vw1__ReloadRequestType req;
	req._USCOREthis = _mor;

	// call_defs Reload to post the request to ESX server or virtual center
	_vw1__ReloadResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::Reload))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task ManagedEntity::Rename_Task(char* newName)
{
	vw1__RenameRequestType req;
	req._USCOREthis = _mor;
	req.newName = newName;

	// call_defs Rename_USCORETask to post the request to ESX server or virtual center
	_vw1__Rename_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::Rename_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}
bool ManagedEntity::get_alarmActionsEnabled() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("alarmActionsEnabled", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return bool();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<xsd__boolean*>(it->val)->__item;
}

std::vector<vw1__Event> ManagedEntity::get_configIssue() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("configIssue", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__Event>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfEvent* amo = dynamic_cast<vw1__ArrayOfEvent*>(it->val);
	std::vector<vw1__Event> ret;
	for (int i = 0; i < amo->__sizeEvent; ++i)
		ret.push_back(*amo->Event[i]);
	return ret;
}

vw1__ManagedEntityStatus ManagedEntity::get_configStatus() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("configStatus", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__ManagedEntityStatus();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<vw1__ManagedEntityStatus_*>(it->val)->__item;
}

std::vector<vw1__CustomFieldValue> ManagedEntity::get_customValue() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("customValue", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__CustomFieldValue>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfCustomFieldValue* amo = dynamic_cast<vw1__ArrayOfCustomFieldValue*>(it->val);
	std::vector<vw1__CustomFieldValue> ret;
	for (int i = 0; i < amo->__sizeCustomFieldValue; ++i)
		ret.push_back(*amo->CustomFieldValue[i]);
	return ret;
}

std::vector<vw1__AlarmState> ManagedEntity::get_declaredAlarmState() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("declaredAlarmState", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__AlarmState>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfAlarmState* amo = dynamic_cast<vw1__ArrayOfAlarmState*>(it->val);
	std::vector<vw1__AlarmState> ret;
	for (int i = 0; i < amo->__sizeAlarmState; ++i)
		ret.push_back(*amo->AlarmState[i]);
	return ret;
}

std::vector<std::string> ManagedEntity::get_disabledMethod() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("disabledMethod", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<std::string>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfString* amo = dynamic_cast<vw1__ArrayOfString*>(it->val);
	std::vector<std::string> ret;
	for (int i = 0; i < amo->__sizestring; ++i)
		ret.push_back(amo->string[i]);
	return ret;
}

std::vector<int> ManagedEntity::get_effectiveRole() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("effectiveRole", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<int>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfInt* amo = dynamic_cast<vw1__ArrayOfInt*>(it->val);
	std::vector<int> ret;
	for (int i = 0; i < amo->__sizeint_; ++i)
		ret.push_back(amo->int_[i]);
	return ret;
}

std::string ManagedEntity::get_name() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("name", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::string();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<xsd__string*>(it->val)->__item;
}

vw1__ManagedEntityStatus ManagedEntity::get_overallStatus() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("overallStatus", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__ManagedEntityStatus();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<vw1__ManagedEntityStatus_*>(it->val)->__item;
}

ManagedEntity ManagedEntity::get_parent() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("parent", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return ManagedEntity();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return ManagedEntity(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}

std::vector<vw1__Permission> ManagedEntity::get_permission() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("permission", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__Permission>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfPermission* amo = dynamic_cast<vw1__ArrayOfPermission*>(it->val);
	std::vector<vw1__Permission> ret;
	for (int i = 0; i < amo->__sizePermission; ++i)
		ret.push_back(*amo->Permission[i]);
	return ret;
}

std::vector<Task> ManagedEntity::get_recentTask() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("recentTask", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<Task>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<Task> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(Task(amo->ManagedObjectReference[i]));
	return ret;
}

std::vector<vw1__Tag> ManagedEntity::get_tag() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("tag", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__Tag>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfTag* amo = dynamic_cast<vw1__ArrayOfTag*>(it->val);
	std::vector<vw1__Tag> ret;
	for (int i = 0; i < amo->__sizeTag; ++i)
		ret.push_back(*amo->Tag[i]);
	return ret;
}

std::vector<vw1__AlarmState> ManagedEntity::get_triggeredAlarmState() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("triggeredAlarmState", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__AlarmState>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfAlarmState* amo = dynamic_cast<vw1__ArrayOfAlarmState*>(it->val);
	std::vector<vw1__AlarmState> ret;
	for (int i = 0; i < amo->__sizeAlarmState; ++i)
		ret.push_back(*amo->AlarmState[i]);
	return ret;
}


const char* Network::type = "Network";
Network::Network(vw1__ManagedObjectReference* mor) : ManagedEntity(mor) {}
Network::~Network() {}

void Network::DestroyNetwork()
{
	vw1__DestroyNetworkRequestType req;
	req._USCOREthis = _mor;

	// call_defs DestroyNetwork to post the request to ESX server or virtual center
	_vw1__DestroyNetworkResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DestroyNetwork))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
std::vector<HostSystem> Network::get_host() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("host", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<HostSystem>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<HostSystem> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(HostSystem(amo->ManagedObjectReference[i]));
	return ret;
}

std::string Network::get_name() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("name", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::string();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<xsd__string*>(it->val)->__item;
}

vw1__NetworkSummary Network::get_summary() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("summary", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__NetworkSummary();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__NetworkSummary*>(it->val);
}

std::vector<VirtualMachine> Network::get_vm() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("vm", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<VirtualMachine>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<VirtualMachine> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(VirtualMachine(amo->ManagedObjectReference[i]));
	return ret;
}


const char* OptionManager::type = "OptionManager";
OptionManager::OptionManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
OptionManager::~OptionManager() {}

std::vector<vw1__OptionValue> OptionManager::QueryOptions(char* name)
{
	vw1__QueryOptionsRequestType req;
	req._USCOREthis = _mor;
	req.name = name;

	// call_defs QueryOptions to post the request to ESX server or virtual center
	_vw1__QueryOptionsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryOptions))
	{
		std::vector<vw1__OptionValue> vw1__optionvalues;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__OptionValue tmp(*rsp.returnval[i]);
			vw1__optionvalues.push_back(tmp);
		}
		return vw1__optionvalues;
	}

	throw get_last_error();
}

void OptionManager::UpdateOptions(int sizechangedValue, vw1__OptionValue**  changedValue)
{
	vw1__UpdateOptionsRequestType req;
	req._USCOREthis = _mor;
	req.__sizechangedValue = sizechangedValue;
	req.changedValue = changedValue;

	// call_defs UpdateOptions to post the request to ESX server or virtual center
	_vw1__UpdateOptionsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateOptions))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
std::vector<vw1__OptionValue> OptionManager::get_setting() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("setting", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__OptionValue>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfOptionValue* amo = dynamic_cast<vw1__ArrayOfOptionValue*>(it->val);
	std::vector<vw1__OptionValue> ret;
	for (int i = 0; i < amo->__sizeOptionValue; ++i)
		ret.push_back(*amo->OptionValue[i]);
	return ret;
}

std::vector<vw1__OptionDef> OptionManager::get_supportedOption() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("supportedOption", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__OptionDef>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfOptionDef* amo = dynamic_cast<vw1__ArrayOfOptionDef*>(it->val);
	std::vector<vw1__OptionDef> ret;
	for (int i = 0; i < amo->__sizeOptionDef; ++i)
		ret.push_back(*amo->OptionDef[i]);
	return ret;
}


const char* OvfManager::type = "OvfManager";
OvfManager::OvfManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
OvfManager::~OvfManager() {}

vw1__OvfCreateDescriptorResult OvfManager::CreateDescriptor(ManagedEntity obj, vw1__OvfCreateDescriptorParams*  cdp)
{
	vw1__CreateDescriptorRequestType req;
	req._USCOREthis = _mor;
	req.obj = obj;
	req.cdp = cdp;

	// call_defs CreateDescriptor to post the request to ESX server or virtual center
	_vw1__CreateDescriptorResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateDescriptor))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__OvfCreateImportSpecResult OvfManager::CreateImportSpec(char* ovfDescriptor, ResourcePool resourcePool, Datastore datastore, vw1__OvfCreateImportSpecParams*  cisp)
{
	vw1__CreateImportSpecRequestType req;
	req._USCOREthis = _mor;
	req.ovfDescriptor = ovfDescriptor;
	req.resourcePool = resourcePool;
	req.datastore = datastore;
	req.cisp = cisp;

	// call_defs CreateImportSpec to post the request to ESX server or virtual center
	_vw1__CreateImportSpecResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateImportSpec))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__OvfParseDescriptorResult OvfManager::ParseDescriptor(char* ovfDescriptor, vw1__OvfParseDescriptorParams*  pdp)
{
	vw1__ParseDescriptorRequestType req;
	req._USCOREthis = _mor;
	req.ovfDescriptor = ovfDescriptor;
	req.pdp = pdp;

	// call_defs ParseDescriptor to post the request to ESX server or virtual center
	_vw1__ParseDescriptorResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ParseDescriptor))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__OvfValidateHostResult OvfManager::ValidateHost(char* ovfDescriptor, HostSystem host, vw1__OvfValidateHostParams*  vhp)
{
	vw1__ValidateHostRequestType req;
	req._USCOREthis = _mor;
	req.ovfDescriptor = ovfDescriptor;
	req.host = host;
	req.vhp = vhp;

	// call_defs ValidateHost to post the request to ESX server or virtual center
	_vw1__ValidateHostResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ValidateHost))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

const char* PerformanceManager::type = "PerformanceManager";
PerformanceManager::PerformanceManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
PerformanceManager::~PerformanceManager() {}

void PerformanceManager::CreatePerfInterval(vw1__PerfInterval*  intervalId)
{
	vw1__CreatePerfIntervalRequestType req;
	req._USCOREthis = _mor;
	req.intervalId = intervalId;

	// call_defs CreatePerfInterval to post the request to ESX server or virtual center
	_vw1__CreatePerfIntervalResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreatePerfInterval))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

std::vector<vw1__PerfMetricId> PerformanceManager::QueryAvailablePerfMetric(vw1__ManagedObjectReference*  entity, time_t*  beginTime, time_t*  endTime, int*  intervalId)
{
	vw1__QueryAvailablePerfMetricRequestType req;
	req._USCOREthis = _mor;
	req.entity = entity;
	req.beginTime = beginTime;
	req.endTime = endTime;
	req.intervalId = intervalId;

	// call_defs QueryAvailablePerfMetric to post the request to ESX server or virtual center
	_vw1__QueryAvailablePerfMetricResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryAvailablePerfMetric))
	{
		std::vector<vw1__PerfMetricId> vw1__perfmetricids;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__PerfMetricId tmp(*rsp.returnval[i]);
			vw1__perfmetricids.push_back(tmp);
		}
		return vw1__perfmetricids;
	}

	throw get_last_error();
}

std::vector<vw1__PerfEntityMetricBase> PerformanceManager::QueryPerf(int sizequerySpec, vw1__PerfQuerySpec**  querySpec)
{
	vw1__QueryPerfRequestType req;
	req._USCOREthis = _mor;
	req.__sizequerySpec = sizequerySpec;
	req.querySpec = querySpec;

	// call_defs QueryPerf to post the request to ESX server or virtual center
	_vw1__QueryPerfResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryPerf))
	{
		std::vector<vw1__PerfEntityMetricBase> vw1__perfentitymetricbases;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__PerfEntityMetricBase tmp(*rsp.returnval[i]);
			vw1__perfentitymetricbases.push_back(tmp);
		}
		return vw1__perfentitymetricbases;
	}

	throw get_last_error();
}

vw1__PerfCompositeMetric PerformanceManager::QueryPerfComposite(vw1__PerfQuerySpec*  querySpec)
{
	vw1__QueryPerfCompositeRequestType req;
	req._USCOREthis = _mor;
	req.querySpec = querySpec;

	// call_defs QueryPerfComposite to post the request to ESX server or virtual center
	_vw1__QueryPerfCompositeResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryPerfComposite))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

std::vector<vw1__PerfCounterInfo> PerformanceManager::QueryPerfCounter(int sizecounterId, int* counterId)
{
	vw1__QueryPerfCounterRequestType req;
	req._USCOREthis = _mor;
	req.__sizecounterId = sizecounterId;
	req.counterId = counterId;

	// call_defs QueryPerfCounter to post the request to ESX server or virtual center
	_vw1__QueryPerfCounterResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryPerfCounter))
	{
		std::vector<vw1__PerfCounterInfo> vw1__perfcounterinfos;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__PerfCounterInfo tmp(*rsp.returnval[i]);
			vw1__perfcounterinfos.push_back(tmp);
		}
		return vw1__perfcounterinfos;
	}

	throw get_last_error();
}

std::vector<vw1__PerfCounterInfo> PerformanceManager::QueryPerfCounterByLevel(int level)
{
	vw1__QueryPerfCounterByLevelRequestType req;
	req._USCOREthis = _mor;
	req.level = level;

	// call_defs QueryPerfCounterByLevel to post the request to ESX server or virtual center
	_vw1__QueryPerfCounterByLevelResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryPerfCounterByLevel))
	{
		std::vector<vw1__PerfCounterInfo> vw1__perfcounterinfos;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__PerfCounterInfo tmp(*rsp.returnval[i]);
			vw1__perfcounterinfos.push_back(tmp);
		}
		return vw1__perfcounterinfos;
	}

	throw get_last_error();
}

vw1__PerfProviderSummary PerformanceManager::QueryPerfProviderSummary(vw1__ManagedObjectReference*  entity)
{
	vw1__QueryPerfProviderSummaryRequestType req;
	req._USCOREthis = _mor;
	req.entity = entity;

	// call_defs QueryPerfProviderSummary to post the request to ESX server or virtual center
	_vw1__QueryPerfProviderSummaryResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryPerfProviderSummary))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

void PerformanceManager::RemovePerfInterval(int samplePeriod)
{
	vw1__RemovePerfIntervalRequestType req;
	req._USCOREthis = _mor;
	req.samplePeriod = samplePeriod;

	// call_defs RemovePerfInterval to post the request to ESX server or virtual center
	_vw1__RemovePerfIntervalResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemovePerfInterval))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void PerformanceManager::ResetCounterLevelMapping(int sizecounters, int* counters)
{
	vw1__ResetCounterLevelMappingRequestType req;
	req._USCOREthis = _mor;
	req.__sizecounters = sizecounters;
	req.counters = counters;

	// call_defs ResetCounterLevelMapping to post the request to ESX server or virtual center
	_vw1__ResetCounterLevelMappingResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ResetCounterLevelMapping))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void PerformanceManager::UpdateCounterLevelMapping(int sizecounterLevelMap, vw1__PerformanceManagerCounterLevelMapping**  counterLevelMap)
{
	vw1__UpdateCounterLevelMappingRequestType req;
	req._USCOREthis = _mor;
	req.__sizecounterLevelMap = sizecounterLevelMap;
	req.counterLevelMap = counterLevelMap;

	// call_defs UpdateCounterLevelMapping to post the request to ESX server or virtual center
	_vw1__UpdateCounterLevelMappingResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateCounterLevelMapping))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void PerformanceManager::UpdatePerfInterval(vw1__PerfInterval*  interval)
{
	vw1__UpdatePerfIntervalRequestType req;
	req._USCOREthis = _mor;
	req.interval = interval;

	// call_defs UpdatePerfInterval to post the request to ESX server or virtual center
	_vw1__UpdatePerfIntervalResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdatePerfInterval))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__PerformanceDescription PerformanceManager::get_description() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("description", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__PerformanceDescription();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__PerformanceDescription*>(it->val);
}

std::vector<vw1__PerfInterval> PerformanceManager::get_historicalInterval() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("historicalInterval", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__PerfInterval>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfPerfInterval* amo = dynamic_cast<vw1__ArrayOfPerfInterval*>(it->val);
	std::vector<vw1__PerfInterval> ret;
	for (int i = 0; i < amo->__sizePerfInterval; ++i)
		ret.push_back(*amo->PerfInterval[i]);
	return ret;
}

std::vector<vw1__PerfCounterInfo> PerformanceManager::get_perfCounter() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("perfCounter", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__PerfCounterInfo>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfPerfCounterInfo* amo = dynamic_cast<vw1__ArrayOfPerfCounterInfo*>(it->val);
	std::vector<vw1__PerfCounterInfo> ret;
	for (int i = 0; i < amo->__sizePerfCounterInfo; ++i)
		ret.push_back(*amo->PerfCounterInfo[i]);
	return ret;
}


const char* Profile::type = "Profile";
Profile::Profile(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
Profile::~Profile() {}

void Profile::AssociateProfile(int sizeentity, ManagedEntity* entity)
{
	vw1__AssociateProfileRequestType req;
	req._USCOREthis = _mor;
	req.__sizeentity = sizeentity;
	vw1__ManagedObjectReference ** pentity = new vw1__ManagedObjectReference *[sizeentity];
	for (int i = 0; i < sizeentity; ++i)
		pentity[i] = entity[i];
	req.entity = pentity;

	// call_defs AssociateProfile to post the request to ESX server or virtual center
	_vw1__AssociateProfileResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AssociateProfile))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task Profile::CheckProfileCompliance_Task(int sizeentity, ManagedEntity* entity)
{
	vw1__CheckProfileComplianceRequestType req;
	req._USCOREthis = _mor;
	req.__sizeentity = sizeentity;
	vw1__ManagedObjectReference ** pentity = new vw1__ManagedObjectReference *[sizeentity];
	for (int i = 0; i < sizeentity; ++i)
		pentity[i] = entity[i];
	req.entity = pentity;

	// call_defs CheckProfileCompliance_USCORETask to post the request to ESX server or virtual center
	_vw1__CheckProfileCompliance_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CheckProfileCompliance_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void Profile::DestroyProfile()
{
	vw1__DestroyProfileRequestType req;
	req._USCOREthis = _mor;

	// call_defs DestroyProfile to post the request to ESX server or virtual center
	_vw1__DestroyProfileResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DestroyProfile))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void Profile::DissociateProfile(int sizeentity, ManagedEntity* entity)
{
	vw1__DissociateProfileRequestType req;
	req._USCOREthis = _mor;
	req.__sizeentity = sizeentity;
	vw1__ManagedObjectReference ** pentity = new vw1__ManagedObjectReference *[sizeentity];
	for (int i = 0; i < sizeentity; ++i)
		pentity[i] = entity[i];
	req.entity = pentity;

	// call_defs DissociateProfile to post the request to ESX server or virtual center
	_vw1__DissociateProfileResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DissociateProfile))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

std::string Profile::ExportProfile()
{
	vw1__ExportProfileRequestType req;
	req._USCOREthis = _mor;

	// call_defs ExportProfile to post the request to ESX server or virtual center
	_vw1__ExportProfileResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ExportProfile))
	{
		return static_cast<std::string>(rsp.returnval);
	}

	throw get_last_error();
}

vw1__ProfileDescription Profile::RetrieveDescription()
{
	vw1__RetrieveDescriptionRequestType req;
	req._USCOREthis = _mor;

	// call_defs RetrieveDescription to post the request to ESX server or virtual center
	_vw1__RetrieveDescriptionResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RetrieveDescription))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}
std::string Profile::get_complianceStatus() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("complianceStatus", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::string();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<xsd__string*>(it->val)->__item;
}

vw1__ProfileConfigInfo Profile::get_config() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("config", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__ProfileConfigInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__ProfileConfigInfo*>(it->val);
}

time_t Profile::get_createdTime() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("createdTime", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return time_t();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<xsd__dateTime*>(it->val)->__item;
}

vw1__ProfileDescription Profile::get_description() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("description", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__ProfileDescription();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__ProfileDescription*>(it->val);
}

std::vector<ManagedEntity> Profile::get_entity() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("entity", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<ManagedEntity>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<ManagedEntity> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(ManagedEntity(amo->ManagedObjectReference[i]));
	return ret;
}

time_t Profile::get_modifiedTime() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("modifiedTime", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return time_t();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<xsd__dateTime*>(it->val)->__item;
}

std::string Profile::get_name() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("name", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::string();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<xsd__string*>(it->val)->__item;
}


const char* ProfileComplianceManager::type = "ProfileComplianceManager";
ProfileComplianceManager::ProfileComplianceManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
ProfileComplianceManager::~ProfileComplianceManager() {}

Task ProfileComplianceManager::CheckCompliance_Task(int sizeprofile, Profile* profile, int sizeentity, ManagedEntity* entity)
{
	vw1__CheckComplianceRequestType req;
	req._USCOREthis = _mor;
	req.__sizeprofile = sizeprofile;
	vw1__ManagedObjectReference ** pprofile = new vw1__ManagedObjectReference *[sizeprofile];
	for (int i = 0; i < sizeprofile; ++i)
		pprofile[i] = profile[i];
	req.profile = pprofile;
	req.__sizeentity = sizeentity;
	vw1__ManagedObjectReference ** pentity = new vw1__ManagedObjectReference *[sizeentity];
	for (int i = 0; i < sizeentity; ++i)
		pentity[i] = entity[i];
	req.entity = pentity;

	// call_defs CheckCompliance_USCORETask to post the request to ESX server or virtual center
	_vw1__CheckCompliance_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CheckCompliance_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void ProfileComplianceManager::ClearComplianceStatus(int sizeprofile, Profile* profile, int sizeentity, ManagedEntity* entity)
{
	vw1__ClearComplianceStatusRequestType req;
	req._USCOREthis = _mor;
	req.__sizeprofile = sizeprofile;
	vw1__ManagedObjectReference ** pprofile = new vw1__ManagedObjectReference *[sizeprofile];
	for (int i = 0; i < sizeprofile; ++i)
		pprofile[i] = profile[i];
	req.profile = pprofile;
	req.__sizeentity = sizeentity;
	vw1__ManagedObjectReference ** pentity = new vw1__ManagedObjectReference *[sizeentity];
	for (int i = 0; i < sizeentity; ++i)
		pentity[i] = entity[i];
	req.entity = pentity;

	// call_defs ClearComplianceStatus to post the request to ESX server or virtual center
	_vw1__ClearComplianceStatusResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ClearComplianceStatus))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

std::vector<vw1__ComplianceResult> ProfileComplianceManager::QueryComplianceStatus(int sizeprofile, Profile* profile, int sizeentity, ManagedEntity* entity)
{
	vw1__QueryComplianceStatusRequestType req;
	req._USCOREthis = _mor;
	req.__sizeprofile = sizeprofile;
	vw1__ManagedObjectReference ** pprofile = new vw1__ManagedObjectReference *[sizeprofile];
	for (int i = 0; i < sizeprofile; ++i)
		pprofile[i] = profile[i];
	req.profile = pprofile;
	req.__sizeentity = sizeentity;
	vw1__ManagedObjectReference ** pentity = new vw1__ManagedObjectReference *[sizeentity];
	for (int i = 0; i < sizeentity; ++i)
		pentity[i] = entity[i];
	req.entity = pentity;

	// call_defs QueryComplianceStatus to post the request to ESX server or virtual center
	_vw1__QueryComplianceStatusResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryComplianceStatus))
	{
		std::vector<vw1__ComplianceResult> vw1__complianceresults;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__ComplianceResult tmp(*rsp.returnval[i]);
			vw1__complianceresults.push_back(tmp);
		}
		return vw1__complianceresults;
	}

	throw get_last_error();
}

std::vector<vw1__ProfileExpressionMetadata> ProfileComplianceManager::QueryExpressionMetadata(int sizeexpressionName, char** expressionName, Profile profile)
{
	vw1__QueryExpressionMetadataRequestType req;
	req._USCOREthis = _mor;
	req.__sizeexpressionName = sizeexpressionName;
	req.expressionName = expressionName;
	req.profile = profile;

	// call_defs QueryExpressionMetadata to post the request to ESX server or virtual center
	_vw1__QueryExpressionMetadataResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryExpressionMetadata))
	{
		std::vector<vw1__ProfileExpressionMetadata> vw1__profileexpressionmetadatas;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__ProfileExpressionMetadata tmp(*rsp.returnval[i]);
			vw1__profileexpressionmetadatas.push_back(tmp);
		}
		return vw1__profileexpressionmetadatas;
	}

	throw get_last_error();
}

const char* ProfileManager::type = "ProfileManager";
ProfileManager::ProfileManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
ProfileManager::~ProfileManager() {}

Profile ProfileManager::CreateProfile(vw1__ProfileCreateSpec*  createSpec)
{
	vw1__CreateProfileRequestType req;
	req._USCOREthis = _mor;
	req.createSpec = createSpec;

	// call_defs CreateProfile to post the request to ESX server or virtual center
	_vw1__CreateProfileResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateProfile))
	{
		return Profile(rsp.returnval);
	}

	throw get_last_error();
}

std::vector<Profile> ProfileManager::FindAssociatedProfile(ManagedEntity entity)
{
	vw1__FindAssociatedProfileRequestType req;
	req._USCOREthis = _mor;
	req.entity = entity;

	// call_defs FindAssociatedProfile to post the request to ESX server or virtual center
	_vw1__FindAssociatedProfileResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::FindAssociatedProfile))
	{
		std::vector<Profile> profiles;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			Profile tmp(rsp.returnval[i]);
			profiles.push_back(tmp);
		}
		return profiles;
	}

	throw get_last_error();
}

std::vector<vw1__ProfilePolicyMetadata> ProfileManager::QueryPolicyMetadata(int sizepolicyName, char** policyName, Profile profile)
{
	vw1__QueryPolicyMetadataRequestType req;
	req._USCOREthis = _mor;
	req.__sizepolicyName = sizepolicyName;
	req.policyName = policyName;
	req.profile = profile;

	// call_defs QueryPolicyMetadata to post the request to ESX server or virtual center
	_vw1__QueryPolicyMetadataResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryPolicyMetadata))
	{
		std::vector<vw1__ProfilePolicyMetadata> vw1__profilepolicymetadatas;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__ProfilePolicyMetadata tmp(*rsp.returnval[i]);
			vw1__profilepolicymetadatas.push_back(tmp);
		}
		return vw1__profilepolicymetadatas;
	}

	throw get_last_error();
}
std::vector<Profile> ProfileManager::get_profile() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("profile", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<Profile>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<Profile> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(Profile(amo->ManagedObjectReference[i]));
	return ret;
}


const char* PropertyCollector::type = "PropertyCollector";
PropertyCollector::PropertyCollector(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
PropertyCollector::~PropertyCollector() {}

void PropertyCollector::CancelRetrievePropertiesEx(char* token)
{
	vw1__CancelRetrievePropertiesExRequestType req;
	req._USCOREthis = _mor;
	req.token = token;

	// call_defs CancelRetrievePropertiesEx to post the request to ESX server or virtual center
	_vw1__CancelRetrievePropertiesExResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CancelRetrievePropertiesEx))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void PropertyCollector::CancelWaitForUpdates()
{
	vw1__CancelWaitForUpdatesRequestType req;
	req._USCOREthis = _mor;

	// call_defs CancelWaitForUpdates to post the request to ESX server or virtual center
	_vw1__CancelWaitForUpdatesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CancelWaitForUpdates))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

vw1__UpdateSet PropertyCollector::CheckForUpdates(char* version)
{
	vw1__CheckForUpdatesRequestType req;
	req._USCOREthis = _mor;
	req.version = version;

	// call_defs CheckForUpdates to post the request to ESX server or virtual center
	_vw1__CheckForUpdatesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CheckForUpdates))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__RetrieveResult PropertyCollector::ContinueRetrievePropertiesEx(char* token)
{
	vw1__ContinueRetrievePropertiesExRequestType req;
	req._USCOREthis = _mor;
	req.token = token;

	// call_defs ContinueRetrievePropertiesEx to post the request to ESX server or virtual center
	_vw1__ContinueRetrievePropertiesExResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ContinueRetrievePropertiesEx))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

PropertyFilter PropertyCollector::CreateFilter(vw1__PropertyFilterSpec*  spec, bool partialUpdates)
{
	vw1__CreateFilterRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;
	req.partialUpdates = partialUpdates;

	// call_defs CreateFilter to post the request to ESX server or virtual center
	_vw1__CreateFilterResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateFilter))
	{
		return PropertyFilter(rsp.returnval);
	}

	throw get_last_error();
}

PropertyCollector PropertyCollector::CreatePropertyCollector()
{
	vw1__CreatePropertyCollectorRequestType req;
	req._USCOREthis = _mor;

	// call_defs CreatePropertyCollector to post the request to ESX server or virtual center
	_vw1__CreatePropertyCollectorResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreatePropertyCollector))
	{
		return PropertyCollector(rsp.returnval);
	}

	throw get_last_error();
}

void PropertyCollector::DestroyPropertyCollector()
{
	vw1__DestroyPropertyCollectorRequestType req;
	req._USCOREthis = _mor;

	// call_defs DestroyPropertyCollector to post the request to ESX server or virtual center
	_vw1__DestroyPropertyCollectorResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DestroyPropertyCollector))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

std::vector<vw1__ObjectContent> PropertyCollector::RetrieveProperties(int sizespecSet, vw1__PropertyFilterSpec**  specSet)
{
	vw1__RetrievePropertiesRequestType req;
	req._USCOREthis = _mor;
	req.__sizespecSet = sizespecSet;
	req.specSet = specSet;

	// call_defs RetrieveProperties to post the request to ESX server or virtual center
	_vw1__RetrievePropertiesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RetrieveProperties))
	{
		std::vector<vw1__ObjectContent> vw1__objectcontents;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__ObjectContent tmp(*rsp.returnval[i]);
			vw1__objectcontents.push_back(tmp);
		}
		return vw1__objectcontents;
	}

	throw get_last_error();
}

vw1__RetrieveResult PropertyCollector::RetrievePropertiesEx(int sizespecSet, vw1__PropertyFilterSpec**  specSet, vw1__RetrieveOptions*  options)
{
	vw1__RetrievePropertiesExRequestType req;
	req._USCOREthis = _mor;
	req.__sizespecSet = sizespecSet;
	req.specSet = specSet;
	req.options = options;

	// call_defs RetrievePropertiesEx to post the request to ESX server or virtual center
	_vw1__RetrievePropertiesExResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RetrievePropertiesEx))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__UpdateSet PropertyCollector::WaitForUpdates(char* version)
{
	vw1__WaitForUpdatesRequestType req;
	req._USCOREthis = _mor;
	req.version = version;

	// call_defs WaitForUpdates to post the request to ESX server or virtual center
	_vw1__WaitForUpdatesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::WaitForUpdates))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__UpdateSet PropertyCollector::WaitForUpdatesEx(char* version, vw1__WaitOptions*  options)
{
	vw1__WaitForUpdatesExRequestType req;
	req._USCOREthis = _mor;
	req.version = version;
	req.options = options;

	// call_defs WaitForUpdatesEx to post the request to ESX server or virtual center
	_vw1__WaitForUpdatesExResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::WaitForUpdatesEx))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}
std::vector<PropertyFilter> PropertyCollector::get_filter() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("filter", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<PropertyFilter>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<PropertyFilter> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(PropertyFilter(amo->ManagedObjectReference[i]));
	return ret;
}


const char* PropertyFilter::type = "PropertyFilter";
PropertyFilter::PropertyFilter(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
PropertyFilter::~PropertyFilter() {}

void PropertyFilter::DestroyPropertyFilter()
{
	vw1__DestroyPropertyFilterRequestType req;
	req._USCOREthis = _mor;

	// call_defs DestroyPropertyFilter to post the request to ESX server or virtual center
	_vw1__DestroyPropertyFilterResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DestroyPropertyFilter))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
bool PropertyFilter::get_partialUpdates() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("partialUpdates", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return bool();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<xsd__boolean*>(it->val)->__item;
}

vw1__PropertyFilterSpec PropertyFilter::get_spec() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("spec", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__PropertyFilterSpec();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__PropertyFilterSpec*>(it->val);
}


const char* ResourcePlanningManager::type = "ResourcePlanningManager";
ResourcePlanningManager::ResourcePlanningManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
ResourcePlanningManager::~ResourcePlanningManager() {}

vw1__DatabaseSizeEstimate ResourcePlanningManager::EstimateDatabaseSize(vw1__DatabaseSizeParam*  dbSizeParam)
{
	vw1__EstimateDatabaseSizeRequestType req;
	req._USCOREthis = _mor;
	req.dbSizeParam = dbSizeParam;

	// call_defs EstimateDatabaseSize to post the request to ESX server or virtual center
	_vw1__EstimateDatabaseSizeResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::EstimateDatabaseSize))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

const char* ResourcePool::type = "ResourcePool";
ResourcePool::ResourcePool(vw1__ManagedObjectReference* mor) : ManagedEntity(mor) {}
ResourcePool::~ResourcePool() {}

Task ResourcePool::CreateChildVM_Task(vw1__VirtualMachineConfigSpec*  config, HostSystem host)
{
	vw1__CreateChildVMRequestType req;
	req._USCOREthis = _mor;
	req.config = config;
	req.host = host;

	// call_defs CreateChildVM_USCORETask to post the request to ESX server or virtual center
	_vw1__CreateChildVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateChildVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

ResourcePool ResourcePool::CreateResourcePool(char* name, vw1__ResourceConfigSpec*  spec)
{
	vw1__CreateResourcePoolRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.spec = spec;

	// call_defs CreateResourcePool to post the request to ESX server or virtual center
	_vw1__CreateResourcePoolResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateResourcePool))
	{
		return ResourcePool(rsp.returnval);
	}

	throw get_last_error();
}

VirtualApp ResourcePool::CreateVApp(char* name, vw1__ResourceConfigSpec*  resSpec, vw1__VAppConfigSpec*  configSpec, Folder vmFolder)
{
	vw1__CreateVAppRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.resSpec = resSpec;
	req.configSpec = configSpec;
	req.vmFolder = vmFolder;

	// call_defs CreateVApp to post the request to ESX server or virtual center
	_vw1__CreateVAppResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateVApp))
	{
		return VirtualApp(rsp.returnval);
	}

	throw get_last_error();
}

void ResourcePool::DestroyChildren()
{
	vw1__DestroyChildrenRequestType req;
	req._USCOREthis = _mor;

	// call_defs DestroyChildren to post the request to ESX server or virtual center
	_vw1__DestroyChildrenResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DestroyChildren))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

HttpNfcLease ResourcePool::ImportVApp(vw1__ImportSpec*  spec, Folder folder, HostSystem host)
{
	vw1__ImportVAppRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;
	req.folder = folder;
	req.host = host;

	// call_defs ImportVApp to post the request to ESX server or virtual center
	_vw1__ImportVAppResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ImportVApp))
	{
		return HttpNfcLease(rsp.returnval);
	}

	throw get_last_error();
}

void ResourcePool::MoveIntoResourcePool(int sizelist, ManagedEntity* list)
{
	vw1__MoveIntoResourcePoolRequestType req;
	req._USCOREthis = _mor;
	req.__sizelist = sizelist;
	vw1__ManagedObjectReference ** plist = new vw1__ManagedObjectReference *[sizelist];
	for (int i = 0; i < sizelist; ++i)
		plist[i] = list[i];
	req.list = plist;

	// call_defs MoveIntoResourcePool to post the request to ESX server or virtual center
	_vw1__MoveIntoResourcePoolResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MoveIntoResourcePool))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

vw1__ResourceConfigOption ResourcePool::QueryResourceConfigOption()
{
	vw1__QueryResourceConfigOptionRequestType req;
	req._USCOREthis = _mor;

	// call_defs QueryResourceConfigOption to post the request to ESX server or virtual center
	_vw1__QueryResourceConfigOptionResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryResourceConfigOption))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

void ResourcePool::RefreshRuntime()
{
	vw1__RefreshRuntimeRequestType req;
	req._USCOREthis = _mor;

	// call_defs RefreshRuntime to post the request to ESX server or virtual center
	_vw1__RefreshRuntimeResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RefreshRuntime))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task ResourcePool::RegisterChildVM_Task(char* path, char* name, HostSystem host)
{
	vw1__RegisterChildVMRequestType req;
	req._USCOREthis = _mor;
	req.path = path;
	req.name = name;
	req.host = host;

	// call_defs RegisterChildVM_USCORETask to post the request to ESX server or virtual center
	_vw1__RegisterChildVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RegisterChildVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void ResourcePool::UpdateChildResourceConfiguration(int sizespec, vw1__ResourceConfigSpec**  spec)
{
	vw1__UpdateChildResourceConfigurationRequestType req;
	req._USCOREthis = _mor;
	req.__sizespec = sizespec;
	req.spec = spec;

	// call_defs UpdateChildResourceConfiguration to post the request to ESX server or virtual center
	_vw1__UpdateChildResourceConfigurationResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateChildResourceConfiguration))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void ResourcePool::UpdateConfig(char* name, vw1__ResourceConfigSpec*  config)
{
	vw1__UpdateConfigRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.config = config;

	// call_defs UpdateConfig to post the request to ESX server or virtual center
	_vw1__UpdateConfigResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateConfig))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
std::vector<vw1__ResourceConfigSpec> ResourcePool::get_childConfiguration() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("childConfiguration", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__ResourceConfigSpec>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfResourceConfigSpec* amo = dynamic_cast<vw1__ArrayOfResourceConfigSpec*>(it->val);
	std::vector<vw1__ResourceConfigSpec> ret;
	for (int i = 0; i < amo->__sizeResourceConfigSpec; ++i)
		ret.push_back(*amo->ResourceConfigSpec[i]);
	return ret;
}

vw1__ResourceConfigSpec ResourcePool::get_config() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("config", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__ResourceConfigSpec();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__ResourceConfigSpec*>(it->val);
}

ComputeResource ResourcePool::get_owner() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("owner", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return ComputeResource();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return ComputeResource(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}

std::vector<ResourcePool> ResourcePool::get_resourcePool() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("resourcePool", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<ResourcePool>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<ResourcePool> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(ResourcePool(amo->ManagedObjectReference[i]));
	return ret;
}

vw1__ResourcePoolRuntimeInfo ResourcePool::get_runtime() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("runtime", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__ResourcePoolRuntimeInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__ResourcePoolRuntimeInfo*>(it->val);
}

vw1__ResourcePoolSummary ResourcePool::get_summary() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("summary", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__ResourcePoolSummary();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__ResourcePoolSummary*>(it->val);
}

std::vector<VirtualMachine> ResourcePool::get_vm() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("vm", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<VirtualMachine>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<VirtualMachine> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(VirtualMachine(amo->ManagedObjectReference[i]));
	return ret;
}


const char* ScheduledTask::type = "ScheduledTask";
ScheduledTask::ScheduledTask(vw1__ManagedObjectReference* mor) : ExtensibleManagedObject(mor) {}
ScheduledTask::~ScheduledTask() {}

void ScheduledTask::ReconfigureScheduledTask(vw1__ScheduledTaskSpec*  spec)
{
	vw1__ReconfigureScheduledTaskRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;

	// call_defs ReconfigureScheduledTask to post the request to ESX server or virtual center
	_vw1__ReconfigureScheduledTaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReconfigureScheduledTask))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void ScheduledTask::RemoveScheduledTask()
{
	vw1__RemoveScheduledTaskRequestType req;
	req._USCOREthis = _mor;

	// call_defs RemoveScheduledTask to post the request to ESX server or virtual center
	_vw1__RemoveScheduledTaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemoveScheduledTask))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void ScheduledTask::RunScheduledTask()
{
	vw1__RunScheduledTaskRequestType req;
	req._USCOREthis = _mor;

	// call_defs RunScheduledTask to post the request to ESX server or virtual center
	_vw1__RunScheduledTaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RunScheduledTask))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__ScheduledTaskInfo ScheduledTask::get_info() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("info", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__ScheduledTaskInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__ScheduledTaskInfo*>(it->val);
}


const char* ScheduledTaskManager::type = "ScheduledTaskManager";
ScheduledTaskManager::ScheduledTaskManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
ScheduledTaskManager::~ScheduledTaskManager() {}

ScheduledTask ScheduledTaskManager::CreateObjectScheduledTask(vw1__ManagedObjectReference*  obj, vw1__ScheduledTaskSpec*  spec)
{
	vw1__CreateObjectScheduledTaskRequestType req;
	req._USCOREthis = _mor;
	req.obj = obj;
	req.spec = spec;

	// call_defs CreateObjectScheduledTask to post the request to ESX server or virtual center
	_vw1__CreateObjectScheduledTaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateObjectScheduledTask))
	{
		return ScheduledTask(rsp.returnval);
	}

	throw get_last_error();
}

ScheduledTask ScheduledTaskManager::CreateScheduledTask(ManagedEntity entity, vw1__ScheduledTaskSpec*  spec)
{
	vw1__CreateScheduledTaskRequestType req;
	req._USCOREthis = _mor;
	req.entity = entity;
	req.spec = spec;

	// call_defs CreateScheduledTask to post the request to ESX server or virtual center
	_vw1__CreateScheduledTaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateScheduledTask))
	{
		return ScheduledTask(rsp.returnval);
	}

	throw get_last_error();
}

std::vector<ScheduledTask> ScheduledTaskManager::RetrieveEntityScheduledTask(ManagedEntity entity)
{
	vw1__RetrieveEntityScheduledTaskRequestType req;
	req._USCOREthis = _mor;
	req.entity = entity;

	// call_defs RetrieveEntityScheduledTask to post the request to ESX server or virtual center
	_vw1__RetrieveEntityScheduledTaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RetrieveEntityScheduledTask))
	{
		std::vector<ScheduledTask> scheduledtasks;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			ScheduledTask tmp(rsp.returnval[i]);
			scheduledtasks.push_back(tmp);
		}
		return scheduledtasks;
	}

	throw get_last_error();
}

std::vector<ScheduledTask> ScheduledTaskManager::RetrieveObjectScheduledTask(vw1__ManagedObjectReference*  obj)
{
	vw1__RetrieveObjectScheduledTaskRequestType req;
	req._USCOREthis = _mor;
	req.obj = obj;

	// call_defs RetrieveObjectScheduledTask to post the request to ESX server or virtual center
	_vw1__RetrieveObjectScheduledTaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RetrieveObjectScheduledTask))
	{
		std::vector<ScheduledTask> scheduledtasks;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			ScheduledTask tmp(rsp.returnval[i]);
			scheduledtasks.push_back(tmp);
		}
		return scheduledtasks;
	}

	throw get_last_error();
}
vw1__ScheduledTaskDescription ScheduledTaskManager::get_description() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("description", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__ScheduledTaskDescription();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__ScheduledTaskDescription*>(it->val);
}

std::vector<ScheduledTask> ScheduledTaskManager::get_scheduledTask() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("scheduledTask", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<ScheduledTask>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<ScheduledTask> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(ScheduledTask(amo->ManagedObjectReference[i]));
	return ret;
}


const char* SearchIndex::type = "SearchIndex";
SearchIndex::SearchIndex(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
SearchIndex::~SearchIndex() {}

std::vector<ManagedEntity> SearchIndex::FindAllByDnsName(Datacenter datacenter, char* dnsName, bool vmSearch)
{
	vw1__FindAllByDnsNameRequestType req;
	req._USCOREthis = _mor;
	req.datacenter = datacenter;
	req.dnsName = dnsName;
	req.vmSearch = vmSearch;

	// call_defs FindAllByDnsName to post the request to ESX server or virtual center
	_vw1__FindAllByDnsNameResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::FindAllByDnsName))
	{
		std::vector<ManagedEntity> managedentitys;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			ManagedEntity tmp(rsp.returnval[i]);
			managedentitys.push_back(tmp);
		}
		return managedentitys;
	}

	throw get_last_error();
}

std::vector<ManagedEntity> SearchIndex::FindAllByIp(Datacenter datacenter, char* ip, bool vmSearch)
{
	vw1__FindAllByIpRequestType req;
	req._USCOREthis = _mor;
	req.datacenter = datacenter;
	req.ip = ip;
	req.vmSearch = vmSearch;

	// call_defs FindAllByIp to post the request to ESX server or virtual center
	_vw1__FindAllByIpResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::FindAllByIp))
	{
		std::vector<ManagedEntity> managedentitys;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			ManagedEntity tmp(rsp.returnval[i]);
			managedentitys.push_back(tmp);
		}
		return managedentitys;
	}

	throw get_last_error();
}

std::vector<ManagedEntity> SearchIndex::FindAllByUuid(Datacenter datacenter, char* uuid, bool vmSearch, bool*  instanceUuid)
{
	vw1__FindAllByUuidRequestType req;
	req._USCOREthis = _mor;
	req.datacenter = datacenter;
	req.uuid = uuid;
	req.vmSearch = vmSearch;
	req.instanceUuid = instanceUuid;

	// call_defs FindAllByUuid to post the request to ESX server or virtual center
	_vw1__FindAllByUuidResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::FindAllByUuid))
	{
		std::vector<ManagedEntity> managedentitys;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			ManagedEntity tmp(rsp.returnval[i]);
			managedentitys.push_back(tmp);
		}
		return managedentitys;
	}

	throw get_last_error();
}

VirtualMachine SearchIndex::FindByDatastorePath(Datacenter datacenter, char* path)
{
	vw1__FindByDatastorePathRequestType req;
	req._USCOREthis = _mor;
	req.datacenter = datacenter;
	req.path = path;

	// call_defs FindByDatastorePath to post the request to ESX server or virtual center
	_vw1__FindByDatastorePathResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::FindByDatastorePath))
	{
		return VirtualMachine(rsp.returnval);
	}

	throw get_last_error();
}

ManagedEntity SearchIndex::FindByDnsName(Datacenter datacenter, char* dnsName, bool vmSearch)
{
	vw1__FindByDnsNameRequestType req;
	req._USCOREthis = _mor;
	req.datacenter = datacenter;
	req.dnsName = dnsName;
	req.vmSearch = vmSearch;

	// call_defs FindByDnsName to post the request to ESX server or virtual center
	_vw1__FindByDnsNameResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::FindByDnsName))
	{
		return ManagedEntity(rsp.returnval);
	}

	throw get_last_error();
}

ManagedEntity SearchIndex::FindByInventoryPath(char* inventoryPath)
{
	vw1__FindByInventoryPathRequestType req;
	req._USCOREthis = _mor;
	req.inventoryPath = inventoryPath;

	// call_defs FindByInventoryPath to post the request to ESX server or virtual center
	_vw1__FindByInventoryPathResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::FindByInventoryPath))
	{
		return ManagedEntity(rsp.returnval);
	}

	throw get_last_error();
}

ManagedEntity SearchIndex::FindByIp(Datacenter datacenter, char* ip, bool vmSearch)
{
	vw1__FindByIpRequestType req;
	req._USCOREthis = _mor;
	req.datacenter = datacenter;
	req.ip = ip;
	req.vmSearch = vmSearch;

	// call_defs FindByIp to post the request to ESX server or virtual center
	_vw1__FindByIpResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::FindByIp))
	{
		return ManagedEntity(rsp.returnval);
	}

	throw get_last_error();
}

ManagedEntity SearchIndex::FindByUuid(Datacenter datacenter, char* uuid, bool vmSearch, bool*  instanceUuid)
{
	vw1__FindByUuidRequestType req;
	req._USCOREthis = _mor;
	req.datacenter = datacenter;
	req.uuid = uuid;
	req.vmSearch = vmSearch;
	req.instanceUuid = instanceUuid;

	// call_defs FindByUuid to post the request to ESX server or virtual center
	_vw1__FindByUuidResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::FindByUuid))
	{
		return ManagedEntity(rsp.returnval);
	}

	throw get_last_error();
}

ManagedEntity SearchIndex::FindChild(ManagedEntity entity, char* name)
{
	vw1__FindChildRequestType req;
	req._USCOREthis = _mor;
	req.entity = entity;
	req.name = name;

	// call_defs FindChild to post the request to ESX server or virtual center
	_vw1__FindChildResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::FindChild))
	{
		return ManagedEntity(rsp.returnval);
	}

	throw get_last_error();
}

const char* ServiceInstance::type = "ServiceInstance";
ServiceInstance::ServiceInstance(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
ServiceInstance::~ServiceInstance() {}

time_t ServiceInstance::CurrentTime()
{
	vw1__CurrentTimeRequestType req;
	req._USCOREthis = _mor;

	// call_defs CurrentTime to post the request to ESX server or virtual center
	_vw1__CurrentTimeResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CurrentTime))
	{
		return static_cast<time_t>(rsp.returnval);
	}

	throw get_last_error();
}

std::vector<vw1__HostVMotionCompatibility> ServiceInstance::QueryVMotionCompatibility(VirtualMachine vm, int sizehost, HostSystem* host, int sizecompatibility, char** compatibility)
{
	vw1__QueryVMotionCompatibilityRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.__sizehost = sizehost;
	vw1__ManagedObjectReference ** phost = new vw1__ManagedObjectReference *[sizehost];
	for (int i = 0; i < sizehost; ++i)
		phost[i] = host[i];
	req.host = phost;
	req.__sizecompatibility = sizecompatibility;
	req.compatibility = compatibility;

	// call_defs QueryVMotionCompatibility to post the request to ESX server or virtual center
	_vw1__QueryVMotionCompatibilityResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryVMotionCompatibility))
	{
		std::vector<vw1__HostVMotionCompatibility> vw1__hostvmotioncompatibilitys;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__HostVMotionCompatibility tmp(*rsp.returnval[i]);
			vw1__hostvmotioncompatibilitys.push_back(tmp);
		}
		return vw1__hostvmotioncompatibilitys;
	}

	throw get_last_error();
}

std::vector<vw1__ProductComponentInfo> ServiceInstance::RetrieveProductComponents()
{
	vw1__RetrieveProductComponentsRequestType req;
	req._USCOREthis = _mor;

	// call_defs RetrieveProductComponents to post the request to ESX server or virtual center
	_vw1__RetrieveProductComponentsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RetrieveProductComponents))
	{
		std::vector<vw1__ProductComponentInfo> vw1__productcomponentinfos;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__ProductComponentInfo tmp(*rsp.returnval[i]);
			vw1__productcomponentinfos.push_back(tmp);
		}
		return vw1__productcomponentinfos;
	}

	throw get_last_error();
}

vw1__ServiceContent ServiceInstance::RetrieveServiceContent()
{
	vw1__RetrieveServiceContentRequestType req;
	req._USCOREthis = _mor;

	// call_defs RetrieveServiceContent to post the request to ESX server or virtual center
	_vw1__RetrieveServiceContentResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RetrieveServiceContent))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

std::vector<vw1__Event> ServiceInstance::ValidateMigration(int sizevm, VirtualMachine* vm, vw1__VirtualMachinePowerState*  state, int sizetestType, char** testType, ResourcePool pool, HostSystem host)
{
	vw1__ValidateMigrationRequestType req;
	req._USCOREthis = _mor;
	req.__sizevm = sizevm;
	vw1__ManagedObjectReference ** pvm = new vw1__ManagedObjectReference *[sizevm];
	for (int i = 0; i < sizevm; ++i)
		pvm[i] = vm[i];
	req.vm = pvm;
	req.state = state;
	req.__sizetestType = sizetestType;
	req.testType = testType;
	req.pool = pool;
	req.host = host;

	// call_defs ValidateMigration to post the request to ESX server or virtual center
	_vw1__ValidateMigrationResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ValidateMigration))
	{
		std::vector<vw1__Event> vw1__events;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__Event tmp(*rsp.returnval[i]);
			vw1__events.push_back(tmp);
		}
		return vw1__events;
	}

	throw get_last_error();
}
vw1__Capability ServiceInstance::get_capability() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("capability", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__Capability();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__Capability*>(it->val);
}

vw1__ServiceContent ServiceInstance::get_content() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("content", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__ServiceContent();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__ServiceContent*>(it->val);
}

time_t ServiceInstance::get_serverClock() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("serverClock", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return time_t();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<xsd__dateTime*>(it->val)->__item;
}


const char* SessionManager::type = "SessionManager";
SessionManager::SessionManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
SessionManager::~SessionManager() {}

std::string SessionManager::AcquireCloneTicket()
{
	vw1__AcquireCloneTicketRequestType req;
	req._USCOREthis = _mor;

	// call_defs AcquireCloneTicket to post the request to ESX server or virtual center
	_vw1__AcquireCloneTicketResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AcquireCloneTicket))
	{
		return static_cast<std::string>(rsp.returnval);
	}

	throw get_last_error();
}

vw1__SessionManagerGenericServiceTicket SessionManager::AcquireGenericServiceTicket(vw1__SessionManagerServiceRequestSpec*  spec)
{
	vw1__AcquireGenericServiceTicketRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;

	// call_defs AcquireGenericServiceTicket to post the request to ESX server or virtual center
	_vw1__AcquireGenericServiceTicketResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AcquireGenericServiceTicket))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__SessionManagerLocalTicket SessionManager::AcquireLocalTicket(char* userName)
{
	vw1__AcquireLocalTicketRequestType req;
	req._USCOREthis = _mor;
	req.userName = userName;

	// call_defs AcquireLocalTicket to post the request to ESX server or virtual center
	_vw1__AcquireLocalTicketResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AcquireLocalTicket))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__UserSession SessionManager::CloneSession(char* cloneTicket)
{
	vw1__CloneSessionRequestType req;
	req._USCOREthis = _mor;
	req.cloneTicket = cloneTicket;

	// call_defs CloneSession to post the request to ESX server or virtual center
	_vw1__CloneSessionResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CloneSession))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__UserSession SessionManager::ImpersonateUser(char* userName, char* locale)
{
	vw1__ImpersonateUserRequestType req;
	req._USCOREthis = _mor;
	req.userName = userName;
	req.locale = locale;

	// call_defs ImpersonateUser to post the request to ESX server or virtual center
	_vw1__ImpersonateUserResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ImpersonateUser))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__UserSession SessionManager::Login(char* userName, char* password, char* locale)
{
	vw1__LoginRequestType req;
	req._USCOREthis = _mor;
	req.userName = userName;
	req.password = password;
	req.locale = locale;

	// call_defs Login to post the request to ESX server or virtual center
	_vw1__LoginResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::Login))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__UserSession SessionManager::LoginBySSPI(char* base64Token, char* locale)
{
	vw1__LoginBySSPIRequestType req;
	req._USCOREthis = _mor;
	req.base64Token = base64Token;
	req.locale = locale;

	// call_defs LoginBySSPI to post the request to ESX server or virtual center
	_vw1__LoginBySSPIResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::LoginBySSPI))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__UserSession SessionManager::LoginExtensionByCertificate(char* extensionKey, char* locale)
{
	vw1__LoginExtensionByCertificateRequestType req;
	req._USCOREthis = _mor;
	req.extensionKey = extensionKey;
	req.locale = locale;

	// call_defs LoginExtensionByCertificate to post the request to ESX server or virtual center
	_vw1__LoginExtensionByCertificateResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::LoginExtensionByCertificate))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__UserSession SessionManager::LoginExtensionBySubjectName(char* extensionKey, char* locale)
{
	vw1__LoginExtensionBySubjectNameRequestType req;
	req._USCOREthis = _mor;
	req.extensionKey = extensionKey;
	req.locale = locale;

	// call_defs LoginExtensionBySubjectName to post the request to ESX server or virtual center
	_vw1__LoginExtensionBySubjectNameResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::LoginExtensionBySubjectName))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

void SessionManager::Logout()
{
	vw1__LogoutRequestType req;
	req._USCOREthis = _mor;

	// call_defs Logout to post the request to ESX server or virtual center
	_vw1__LogoutResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::Logout))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

bool SessionManager::SessionIsActive(char* sessionID, char* userName)
{
	vw1__SessionIsActiveRequestType req;
	req._USCOREthis = _mor;
	req.sessionID = sessionID;
	req.userName = userName;

	// call_defs SessionIsActive to post the request to ESX server or virtual center
	_vw1__SessionIsActiveResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SessionIsActive))
	{
		return static_cast<bool>(rsp.returnval);
	}

	throw get_last_error();
}

void SessionManager::SetLocale(char* locale)
{
	vw1__SetLocaleRequestType req;
	req._USCOREthis = _mor;
	req.locale = locale;

	// call_defs SetLocale to post the request to ESX server or virtual center
	_vw1__SetLocaleResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SetLocale))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void SessionManager::TerminateSession(int sizesessionId, char** sessionId)
{
	vw1__TerminateSessionRequestType req;
	req._USCOREthis = _mor;
	req.__sizesessionId = sizesessionId;
	req.sessionId = sessionId;

	// call_defs TerminateSession to post the request to ESX server or virtual center
	_vw1__TerminateSessionResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::TerminateSession))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void SessionManager::UpdateServiceMessage(char* message)
{
	vw1__UpdateServiceMessageRequestType req;
	req._USCOREthis = _mor;
	req.message = message;

	// call_defs UpdateServiceMessage to post the request to ESX server or virtual center
	_vw1__UpdateServiceMessageResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateServiceMessage))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__UserSession SessionManager::get_currentSession() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("currentSession", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__UserSession();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__UserSession*>(it->val);
}

std::string SessionManager::get_defaultLocale() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("defaultLocale", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::string();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<xsd__string*>(it->val)->__item;
}

std::string SessionManager::get_message() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("message", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::string();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<xsd__string*>(it->val)->__item;
}

std::vector<std::string> SessionManager::get_messageLocaleList() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("messageLocaleList", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<std::string>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfString* amo = dynamic_cast<vw1__ArrayOfString*>(it->val);
	std::vector<std::string> ret;
	for (int i = 0; i < amo->__sizestring; ++i)
		ret.push_back(amo->string[i]);
	return ret;
}

std::vector<vw1__UserSession> SessionManager::get_sessionList() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("sessionList", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__UserSession>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfUserSession* amo = dynamic_cast<vw1__ArrayOfUserSession*>(it->val);
	std::vector<vw1__UserSession> ret;
	for (int i = 0; i < amo->__sizeUserSession; ++i)
		ret.push_back(*amo->UserSession[i]);
	return ret;
}

std::vector<std::string> SessionManager::get_supportedLocaleList() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("supportedLocaleList", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<std::string>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfString* amo = dynamic_cast<vw1__ArrayOfString*>(it->val);
	std::vector<std::string> ret;
	for (int i = 0; i < amo->__sizestring; ++i)
		ret.push_back(amo->string[i]);
	return ret;
}


const char* StorageResourceManager::type = "StorageResourceManager";
StorageResourceManager::StorageResourceManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
StorageResourceManager::~StorageResourceManager() {}

Task StorageResourceManager::ApplyStorageDrsRecommendation_Task(int sizekey, char** key)
{
	vw1__ApplyStorageDrsRecommendationRequestType req;
	req._USCOREthis = _mor;
	req.__sizekey = sizekey;
	req.key = key;

	// call_defs ApplyStorageDrsRecommendation_USCORETask to post the request to ESX server or virtual center
	_vw1__ApplyStorageDrsRecommendation_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ApplyStorageDrsRecommendation_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task StorageResourceManager::ApplyStorageDrsRecommendationToPod_Task(StoragePod pod, char* key)
{
	vw1__ApplyStorageDrsRecommendationToPodRequestType req;
	req._USCOREthis = _mor;
	req.pod = pod;
	req.key = key;

	// call_defs ApplyStorageDrsRecommendationToPod_USCORETask to post the request to ESX server or virtual center
	_vw1__ApplyStorageDrsRecommendationToPod_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ApplyStorageDrsRecommendationToPod_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void StorageResourceManager::CancelStorageDrsRecommendation(int sizekey, char** key)
{
	vw1__CancelStorageDrsRecommendationRequestType req;
	req._USCOREthis = _mor;
	req.__sizekey = sizekey;
	req.key = key;

	// call_defs CancelStorageDrsRecommendation to post the request to ESX server or virtual center
	_vw1__CancelStorageDrsRecommendationResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CancelStorageDrsRecommendation))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task StorageResourceManager::ConfigureDatastoreIORM_Task(Datastore datastore, vw1__StorageIORMConfigSpec*  spec)
{
	vw1__ConfigureDatastoreIORMRequestType req;
	req._USCOREthis = _mor;
	req.datastore = datastore;
	req.spec = spec;

	// call_defs ConfigureDatastoreIORM_USCORETask to post the request to ESX server or virtual center
	_vw1__ConfigureDatastoreIORM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ConfigureDatastoreIORM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task StorageResourceManager::ConfigureStorageDrsForPod_Task(StoragePod pod, vw1__StorageDrsConfigSpec*  spec, bool modify)
{
	vw1__ConfigureStorageDrsForPodRequestType req;
	req._USCOREthis = _mor;
	req.pod = pod;
	req.spec = spec;
	req.modify = modify;

	// call_defs ConfigureStorageDrsForPod_USCORETask to post the request to ESX server or virtual center
	_vw1__ConfigureStorageDrsForPod_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ConfigureStorageDrsForPod_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

vw1__StorageIORMConfigOption StorageResourceManager::QueryIORMConfigOption(HostSystem host)
{
	vw1__QueryIORMConfigOptionRequestType req;
	req._USCOREthis = _mor;
	req.host = host;

	// call_defs QueryIORMConfigOption to post the request to ESX server or virtual center
	_vw1__QueryIORMConfigOptionResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryIORMConfigOption))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__StoragePlacementResult StorageResourceManager::RecommendDatastores(vw1__StoragePlacementSpec*  storageSpec)
{
	vw1__RecommendDatastoresRequestType req;
	req._USCOREthis = _mor;
	req.storageSpec = storageSpec;

	// call_defs RecommendDatastores to post the request to ESX server or virtual center
	_vw1__RecommendDatastoresResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RecommendDatastores))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

void StorageResourceManager::RefreshStorageDrsRecommendation(StoragePod pod)
{
	vw1__RefreshStorageDrsRecommendationRequestType req;
	req._USCOREthis = _mor;
	req.pod = pod;

	// call_defs RefreshStorageDrsRecommendation to post the request to ESX server or virtual center
	_vw1__RefreshStorageDrsRecommendationResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RefreshStorageDrsRecommendation))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

const char* Task::type = "Task";
Task::Task(vw1__ManagedObjectReference* mor) : ExtensibleManagedObject(mor) {}
Task::~Task() {}

void Task::CancelTask()
{
	vw1__CancelTaskRequestType req;
	req._USCOREthis = _mor;

	// call_defs CancelTask to post the request to ESX server or virtual center
	_vw1__CancelTaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CancelTask))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void Task::SetTaskDescription(vw1__LocalizableMessage*  description)
{
	vw1__SetTaskDescriptionRequestType req;
	req._USCOREthis = _mor;
	req.description = description;

	// call_defs SetTaskDescription to post the request to ESX server or virtual center
	_vw1__SetTaskDescriptionResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SetTaskDescription))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void Task::SetTaskState(vw1__TaskInfoState state, anyType*  result, vw1__LocalizedMethodFault*  fault)
{
	vw1__SetTaskStateRequestType req;
	req._USCOREthis = _mor;
	req.state = state;
	req.result = result;
	req.fault = fault;

	// call_defs SetTaskState to post the request to ESX server or virtual center
	_vw1__SetTaskStateResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SetTaskState))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void Task::UpdateProgress(int percentDone)
{
	vw1__UpdateProgressRequestType req;
	req._USCOREthis = _mor;
	req.percentDone = percentDone;

	// call_defs UpdateProgress to post the request to ESX server or virtual center
	_vw1__UpdateProgressResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateProgress))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__TaskInfo Task::get_info() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("info", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__TaskInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__TaskInfo*>(it->val);
}


const char* TaskHistoryCollector::type = "TaskHistoryCollector";
TaskHistoryCollector::TaskHistoryCollector(vw1__ManagedObjectReference* mor) : HistoryCollector(mor) {}
TaskHistoryCollector::~TaskHistoryCollector() {}

std::vector<vw1__TaskInfo> TaskHistoryCollector::ReadNextTasks(int maxCount)
{
	vw1__ReadNextTasksRequestType req;
	req._USCOREthis = _mor;
	req.maxCount = maxCount;

	// call_defs ReadNextTasks to post the request to ESX server or virtual center
	_vw1__ReadNextTasksResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReadNextTasks))
	{
		std::vector<vw1__TaskInfo> vw1__taskinfos;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__TaskInfo tmp(*rsp.returnval[i]);
			vw1__taskinfos.push_back(tmp);
		}
		return vw1__taskinfos;
	}

	throw get_last_error();
}

std::vector<vw1__TaskInfo> TaskHistoryCollector::ReadPreviousTasks(int maxCount)
{
	vw1__ReadPreviousTasksRequestType req;
	req._USCOREthis = _mor;
	req.maxCount = maxCount;

	// call_defs ReadPreviousTasks to post the request to ESX server or virtual center
	_vw1__ReadPreviousTasksResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReadPreviousTasks))
	{
		std::vector<vw1__TaskInfo> vw1__taskinfos;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__TaskInfo tmp(*rsp.returnval[i]);
			vw1__taskinfos.push_back(tmp);
		}
		return vw1__taskinfos;
	}

	throw get_last_error();
}
std::vector<vw1__TaskInfo> TaskHistoryCollector::get_latestPage() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("latestPage", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__TaskInfo>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfTaskInfo* amo = dynamic_cast<vw1__ArrayOfTaskInfo*>(it->val);
	std::vector<vw1__TaskInfo> ret;
	for (int i = 0; i < amo->__sizeTaskInfo; ++i)
		ret.push_back(*amo->TaskInfo[i]);
	return ret;
}


const char* TaskManager::type = "TaskManager";
TaskManager::TaskManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
TaskManager::~TaskManager() {}

TaskHistoryCollector TaskManager::CreateCollectorForTasks(vw1__TaskFilterSpec*  filter)
{
	vw1__CreateCollectorForTasksRequestType req;
	req._USCOREthis = _mor;
	req.filter = filter;

	// call_defs CreateCollectorForTasks to post the request to ESX server or virtual center
	_vw1__CreateCollectorForTasksResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateCollectorForTasks))
	{
		return TaskHistoryCollector(rsp.returnval);
	}

	throw get_last_error();
}

vw1__TaskInfo TaskManager::CreateTask(vw1__ManagedObjectReference*  obj, char* taskTypeId, char* initiatedBy, bool cancelable, char* parentTaskKey)
{
	vw1__CreateTaskRequestType req;
	req._USCOREthis = _mor;
	req.obj = obj;
	req.taskTypeId = taskTypeId;
	req.initiatedBy = initiatedBy;
	req.cancelable = cancelable;
	req.parentTaskKey = parentTaskKey;

	// call_defs CreateTask to post the request to ESX server or virtual center
	_vw1__CreateTaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateTask))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}
vw1__TaskDescription TaskManager::get_description() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("description", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__TaskDescription();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__TaskDescription*>(it->val);
}

int TaskManager::get_maxCollector() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("maxCollector", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return int();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<xsd__int*>(it->val)->__item;
}

std::vector<Task> TaskManager::get_recentTask() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("recentTask", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<Task>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<Task> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(Task(amo->ManagedObjectReference[i]));
	return ret;
}


const char* UserDirectory::type = "UserDirectory";
UserDirectory::UserDirectory(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
UserDirectory::~UserDirectory() {}

std::vector<vw1__UserSearchResult> UserDirectory::RetrieveUserGroups(char* domain, char* searchStr, char* belongsToGroup, char* belongsToUser, bool exactMatch, bool findUsers, bool findGroups)
{
	vw1__RetrieveUserGroupsRequestType req;
	req._USCOREthis = _mor;
	req.domain = domain;
	req.searchStr = searchStr;
	req.belongsToGroup = belongsToGroup;
	req.belongsToUser = belongsToUser;
	req.exactMatch = exactMatch;
	req.findUsers = findUsers;
	req.findGroups = findGroups;

	// call_defs RetrieveUserGroups to post the request to ESX server or virtual center
	_vw1__RetrieveUserGroupsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RetrieveUserGroups))
	{
		std::vector<vw1__UserSearchResult> vw1__usersearchresults;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__UserSearchResult tmp(*rsp.returnval[i]);
			vw1__usersearchresults.push_back(tmp);
		}
		return vw1__usersearchresults;
	}

	throw get_last_error();
}
std::vector<std::string> UserDirectory::get_domainList() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("domainList", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<std::string>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfString* amo = dynamic_cast<vw1__ArrayOfString*>(it->val);
	std::vector<std::string> ret;
	for (int i = 0; i < amo->__sizestring; ++i)
		ret.push_back(amo->string[i]);
	return ret;
}


const char* View::type = "View";
View::View(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
View::~View() {}

void View::DestroyView()
{
	vw1__DestroyViewRequestType req;
	req._USCOREthis = _mor;

	// call_defs DestroyView to post the request to ESX server or virtual center
	_vw1__DestroyViewResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DestroyView))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

const char* ViewManager::type = "ViewManager";
ViewManager::ViewManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
ViewManager::~ViewManager() {}

ContainerView ViewManager::CreateContainerView(ManagedEntity container, int sizetype, char** type, bool recursive)
{
	vw1__CreateContainerViewRequestType req;
	req._USCOREthis = _mor;
	req.container = container;
	req.__sizetype = sizetype;
	req.type = type;
	req.recursive = recursive;

	// call_defs CreateContainerView to post the request to ESX server or virtual center
	_vw1__CreateContainerViewResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateContainerView))
	{
		return ContainerView(rsp.returnval);
	}

	throw get_last_error();
}

InventoryView ViewManager::CreateInventoryView()
{
	vw1__CreateInventoryViewRequestType req;
	req._USCOREthis = _mor;

	// call_defs CreateInventoryView to post the request to ESX server or virtual center
	_vw1__CreateInventoryViewResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateInventoryView))
	{
		return InventoryView(rsp.returnval);
	}

	throw get_last_error();
}

ListView ViewManager::CreateListView(int sizeobj, vw1__ManagedObjectReference**  obj)
{
	vw1__CreateListViewRequestType req;
	req._USCOREthis = _mor;
	req.__sizeobj = sizeobj;
	vw1__ManagedObjectReference ** pobj = new vw1__ManagedObjectReference *[sizeobj];
	for (int i = 0; i < sizeobj; ++i)
		pobj[i] = obj[i];
	req.obj = pobj;

	// call_defs CreateListView to post the request to ESX server or virtual center
	_vw1__CreateListViewResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateListView))
	{
		return ListView(rsp.returnval);
	}

	throw get_last_error();
}

ListView ViewManager::CreateListViewFromView(View view)
{
	vw1__CreateListViewFromViewRequestType req;
	req._USCOREthis = _mor;
	req.view = view;

	// call_defs CreateListViewFromView to post the request to ESX server or virtual center
	_vw1__CreateListViewFromViewResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateListViewFromView))
	{
		return ListView(rsp.returnval);
	}

	throw get_last_error();
}
std::vector<View> ViewManager::get_viewList() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("viewList", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<View>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<View> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(View(amo->ManagedObjectReference[i]));
	return ret;
}


const char* VirtualApp::type = "VirtualApp";
VirtualApp::VirtualApp(vw1__ManagedObjectReference* mor) : ResourcePool(mor) {}
VirtualApp::~VirtualApp() {}

Task VirtualApp::CloneVApp_Task(char* name, ResourcePool target, vw1__VAppCloneSpec*  spec)
{
	vw1__CloneVAppRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.target = target;
	req.spec = spec;

	// call_defs CloneVApp_USCORETask to post the request to ESX server or virtual center
	_vw1__CloneVApp_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CloneVApp_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

HttpNfcLease VirtualApp::ExportVApp()
{
	vw1__ExportVAppRequestType req;
	req._USCOREthis = _mor;

	// call_defs ExportVApp to post the request to ESX server or virtual center
	_vw1__ExportVAppResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ExportVApp))
	{
		return HttpNfcLease(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualApp::PowerOffVApp_Task(bool force)
{
	vw1__PowerOffVAppRequestType req;
	req._USCOREthis = _mor;
	req.force = force;

	// call_defs PowerOffVApp_USCORETask to post the request to ESX server or virtual center
	_vw1__PowerOffVApp_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::PowerOffVApp_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualApp::PowerOnVApp_Task()
{
	vw1__PowerOnVAppRequestType req;
	req._USCOREthis = _mor;

	// call_defs PowerOnVApp_USCORETask to post the request to ESX server or virtual center
	_vw1__PowerOnVApp_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::PowerOnVApp_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualApp::SuspendVApp_Task()
{
	vw1__SuspendVAppRequestType req;
	req._USCOREthis = _mor;

	// call_defs SuspendVApp_USCORETask to post the request to ESX server or virtual center
	_vw1__SuspendVApp_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SuspendVApp_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualApp::unregisterVApp_Task()
{
	vw1__unregisterVAppRequestType req;
	req._USCOREthis = _mor;

	// call_defs unregisterVApp_USCORETask to post the request to ESX server or virtual center
	_vw1__unregisterVApp_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::unregisterVApp_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void VirtualApp::UpdateLinkedChildren(int sizeaddChangeSet, vw1__VirtualAppLinkInfo**  addChangeSet, int sizeremoveSet, ManagedEntity* removeSet)
{
	vw1__UpdateLinkedChildrenRequestType req;
	req._USCOREthis = _mor;
	req.__sizeaddChangeSet = sizeaddChangeSet;
	req.addChangeSet = addChangeSet;
	req.__sizeremoveSet = sizeremoveSet;
	vw1__ManagedObjectReference ** premoveSet = new vw1__ManagedObjectReference *[sizeremoveSet];
	for (int i = 0; i < sizeremoveSet; ++i)
		premoveSet[i] = removeSet[i];
	req.removeSet = premoveSet;

	// call_defs UpdateLinkedChildren to post the request to ESX server or virtual center
	_vw1__UpdateLinkedChildrenResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateLinkedChildren))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void VirtualApp::UpdateVAppConfig(vw1__VAppConfigSpec*  spec)
{
	vw1__UpdateVAppConfigRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;

	// call_defs UpdateVAppConfig to post the request to ESX server or virtual center
	_vw1__UpdateVAppConfigResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateVAppConfig))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
std::vector<vw1__VirtualAppLinkInfo> VirtualApp::get_childLink() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("childLink", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__VirtualAppLinkInfo>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfVirtualAppLinkInfo* amo = dynamic_cast<vw1__ArrayOfVirtualAppLinkInfo*>(it->val);
	std::vector<vw1__VirtualAppLinkInfo> ret;
	for (int i = 0; i < amo->__sizeVirtualAppLinkInfo; ++i)
		ret.push_back(*amo->VirtualAppLinkInfo[i]);
	return ret;
}

std::vector<Datastore> VirtualApp::get_datastore() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("datastore", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<Datastore>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<Datastore> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(Datastore(amo->ManagedObjectReference[i]));
	return ret;
}

std::vector<Network> VirtualApp::get_network() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("network", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<Network>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<Network> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(Network(amo->ManagedObjectReference[i]));
	return ret;
}

Folder VirtualApp::get_parentFolder() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("parentFolder", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return Folder();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return Folder(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}

ManagedEntity VirtualApp::get_parentVApp() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("parentVApp", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return ManagedEntity();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return ManagedEntity(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}

vw1__VAppConfigInfo VirtualApp::get_vAppConfig() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("vAppConfig", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__VAppConfigInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__VAppConfigInfo*>(it->val);
}


const char* VirtualDiskManager::type = "VirtualDiskManager";
VirtualDiskManager::VirtualDiskManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
VirtualDiskManager::~VirtualDiskManager() {}

Task VirtualDiskManager::CopyVirtualDisk_Task(char* sourceName, Datacenter sourceDatacenter, char* destName, Datacenter destDatacenter, vw1__VirtualDiskSpec*  destSpec, bool*  force)
{
	vw1__CopyVirtualDiskRequestType req;
	req._USCOREthis = _mor;
	req.sourceName = sourceName;
	req.sourceDatacenter = sourceDatacenter;
	req.destName = destName;
	req.destDatacenter = destDatacenter;
	req.destSpec = destSpec;
	req.force = force;

	// call_defs CopyVirtualDisk_USCORETask to post the request to ESX server or virtual center
	_vw1__CopyVirtualDisk_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CopyVirtualDisk_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualDiskManager::CreateVirtualDisk_Task(char* name, Datacenter datacenter, vw1__VirtualDiskSpec*  spec)
{
	vw1__CreateVirtualDiskRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.datacenter = datacenter;
	req.spec = spec;

	// call_defs CreateVirtualDisk_USCORETask to post the request to ESX server or virtual center
	_vw1__CreateVirtualDisk_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateVirtualDisk_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualDiskManager::DefragmentVirtualDisk_Task(char* name, Datacenter datacenter)
{
	vw1__DefragmentVirtualDiskRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.datacenter = datacenter;

	// call_defs DefragmentVirtualDisk_USCORETask to post the request to ESX server or virtual center
	_vw1__DefragmentVirtualDisk_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DefragmentVirtualDisk_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualDiskManager::DeleteVirtualDisk_Task(char* name, Datacenter datacenter)
{
	vw1__DeleteVirtualDiskRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.datacenter = datacenter;

	// call_defs DeleteVirtualDisk_USCORETask to post the request to ESX server or virtual center
	_vw1__DeleteVirtualDisk_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DeleteVirtualDisk_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualDiskManager::EagerZeroVirtualDisk_Task(char* name, Datacenter datacenter)
{
	vw1__EagerZeroVirtualDiskRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.datacenter = datacenter;

	// call_defs EagerZeroVirtualDisk_USCORETask to post the request to ESX server or virtual center
	_vw1__EagerZeroVirtualDisk_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::EagerZeroVirtualDisk_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualDiskManager::ExtendVirtualDisk_Task(char* name, Datacenter datacenter, __int64 newCapacityKb, bool*  eagerZero)
{
	vw1__ExtendVirtualDiskRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.datacenter = datacenter;
	req.newCapacityKb = newCapacityKb;
	req.eagerZero = eagerZero;

	// call_defs ExtendVirtualDisk_USCORETask to post the request to ESX server or virtual center
	_vw1__ExtendVirtualDisk_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ExtendVirtualDisk_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualDiskManager::InflateVirtualDisk_Task(char* name, Datacenter datacenter)
{
	vw1__InflateVirtualDiskRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.datacenter = datacenter;

	// call_defs InflateVirtualDisk_USCORETask to post the request to ESX server or virtual center
	_vw1__InflateVirtualDisk_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::InflateVirtualDisk_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualDiskManager::MoveVirtualDisk_Task(char* sourceName, Datacenter sourceDatacenter, char* destName, Datacenter destDatacenter, bool*  force)
{
	vw1__MoveVirtualDiskRequestType req;
	req._USCOREthis = _mor;
	req.sourceName = sourceName;
	req.sourceDatacenter = sourceDatacenter;
	req.destName = destName;
	req.destDatacenter = destDatacenter;
	req.force = force;

	// call_defs MoveVirtualDisk_USCORETask to post the request to ESX server or virtual center
	_vw1__MoveVirtualDisk_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MoveVirtualDisk_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

int VirtualDiskManager::QueryVirtualDiskFragmentation(char* name, Datacenter datacenter)
{
	vw1__QueryVirtualDiskFragmentationRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.datacenter = datacenter;

	// call_defs QueryVirtualDiskFragmentation to post the request to ESX server or virtual center
	_vw1__QueryVirtualDiskFragmentationResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryVirtualDiskFragmentation))
	{
		return static_cast<int>(rsp.returnval);
	}

	throw get_last_error();
}

vw1__HostDiskDimensionsChs VirtualDiskManager::QueryVirtualDiskGeometry(char* name, Datacenter datacenter)
{
	vw1__QueryVirtualDiskGeometryRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.datacenter = datacenter;

	// call_defs QueryVirtualDiskGeometry to post the request to ESX server or virtual center
	_vw1__QueryVirtualDiskGeometryResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryVirtualDiskGeometry))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

std::string VirtualDiskManager::QueryVirtualDiskUuid(char* name, Datacenter datacenter)
{
	vw1__QueryVirtualDiskUuidRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.datacenter = datacenter;

	// call_defs QueryVirtualDiskUuid to post the request to ESX server or virtual center
	_vw1__QueryVirtualDiskUuidResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryVirtualDiskUuid))
	{
		return static_cast<std::string>(rsp.returnval);
	}

	throw get_last_error();
}

void VirtualDiskManager::SetVirtualDiskUuid(char* name, Datacenter datacenter, char* uuid)
{
	vw1__SetVirtualDiskUuidRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.datacenter = datacenter;
	req.uuid = uuid;

	// call_defs SetVirtualDiskUuid to post the request to ESX server or virtual center
	_vw1__SetVirtualDiskUuidResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SetVirtualDiskUuid))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task VirtualDiskManager::ShrinkVirtualDisk_Task(char* name, Datacenter datacenter, bool*  copy)
{
	vw1__ShrinkVirtualDiskRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.datacenter = datacenter;
	req.copy = copy;

	// call_defs ShrinkVirtualDisk_USCORETask to post the request to ESX server or virtual center
	_vw1__ShrinkVirtualDisk_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ShrinkVirtualDisk_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualDiskManager::ZeroFillVirtualDisk_Task(char* name, Datacenter datacenter)
{
	vw1__ZeroFillVirtualDiskRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.datacenter = datacenter;

	// call_defs ZeroFillVirtualDisk_USCORETask to post the request to ESX server or virtual center
	_vw1__ZeroFillVirtualDisk_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ZeroFillVirtualDisk_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

const char* VirtualMachine::type = "VirtualMachine";
VirtualMachine::VirtualMachine(vw1__ManagedObjectReference* mor) : ManagedEntity(mor) {}
VirtualMachine::~VirtualMachine() {}

vw1__VirtualMachineMksTicket VirtualMachine::AcquireMksTicket()
{
	vw1__AcquireMksTicketRequestType req;
	req._USCOREthis = _mor;

	// call_defs AcquireMksTicket to post the request to ESX server or virtual center
	_vw1__AcquireMksTicketResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AcquireMksTicket))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__VirtualMachineTicket VirtualMachine::AcquireTicket(char* ticketType)
{
	vw1__AcquireTicketRequestType req;
	req._USCOREthis = _mor;
	req.ticketType = ticketType;

	// call_defs AcquireTicket to post the request to ESX server or virtual center
	_vw1__AcquireTicketResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AcquireTicket))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

void VirtualMachine::AnswerVM(char* questionId, char* answerChoice)
{
	vw1__AnswerVMRequestType req;
	req._USCOREthis = _mor;
	req.questionId = questionId;
	req.answerChoice = answerChoice;

	// call_defs AnswerVM to post the request to ESX server or virtual center
	_vw1__AnswerVMResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AnswerVM))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void VirtualMachine::CheckCustomizationSpec(vw1__CustomizationSpec*  spec)
{
	vw1__CheckCustomizationSpecRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;

	// call_defs CheckCustomizationSpec to post the request to ESX server or virtual center
	_vw1__CheckCustomizationSpecResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CheckCustomizationSpec))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task VirtualMachine::CloneVM_Task(Folder folder, char* name, vw1__VirtualMachineCloneSpec*  spec)
{
	vw1__CloneVMRequestType req;
	req._USCOREthis = _mor;
	req.folder = folder;
	req.name = name;
	req.spec = spec;

	// call_defs CloneVM_USCORETask to post the request to ESX server or virtual center
	_vw1__CloneVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CloneVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::ConsolidateVMDisks_Task()
{
	vw1__ConsolidateVMDisksRequestType req;
	req._USCOREthis = _mor;

	// call_defs ConsolidateVMDisks_USCORETask to post the request to ESX server or virtual center
	_vw1__ConsolidateVMDisks_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ConsolidateVMDisks_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::CreateScreenshot_Task()
{
	vw1__CreateScreenshotRequestType req;
	req._USCOREthis = _mor;

	// call_defs CreateScreenshot_USCORETask to post the request to ESX server or virtual center
	_vw1__CreateScreenshot_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateScreenshot_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::CreateSecondaryVM_Task(HostSystem host)
{
	vw1__CreateSecondaryVMRequestType req;
	req._USCOREthis = _mor;
	req.host = host;

	// call_defs CreateSecondaryVM_USCORETask to post the request to ESX server or virtual center
	_vw1__CreateSecondaryVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateSecondaryVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::CreateSnapshot_Task(char* name, char* description, bool memory, bool quiesce)
{
	vw1__CreateSnapshotRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.description = description;
	req.memory = memory;
	req.quiesce = quiesce;

	// call_defs CreateSnapshot_USCORETask to post the request to ESX server or virtual center
	_vw1__CreateSnapshot_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateSnapshot_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::CustomizeVM_Task(vw1__CustomizationSpec*  spec)
{
	vw1__CustomizeVMRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;

	// call_defs CustomizeVM_USCORETask to post the request to ESX server or virtual center
	_vw1__CustomizeVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CustomizeVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void VirtualMachine::DefragmentAllDisks()
{
	vw1__DefragmentAllDisksRequestType req;
	req._USCOREthis = _mor;

	// call_defs DefragmentAllDisks to post the request to ESX server or virtual center
	_vw1__DefragmentAllDisksResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DefragmentAllDisks))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task VirtualMachine::DisableSecondaryVM_Task(VirtualMachine vm)
{
	vw1__DisableSecondaryVMRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;

	// call_defs DisableSecondaryVM_USCORETask to post the request to ESX server or virtual center
	_vw1__DisableSecondaryVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DisableSecondaryVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::EnableSecondaryVM_Task(VirtualMachine vm, HostSystem host)
{
	vw1__EnableSecondaryVMRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.host = host;

	// call_defs EnableSecondaryVM_USCORETask to post the request to ESX server or virtual center
	_vw1__EnableSecondaryVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::EnableSecondaryVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::EstimateStorageForConsolidateSnapshots_Task()
{
	vw1__EstimateStorageForConsolidateSnapshotsRequestType req;
	req._USCOREthis = _mor;

	// call_defs EstimateStorageForConsolidateSnapshots_USCORETask to post the request to ESX server or virtual center
	_vw1__EstimateStorageForConsolidateSnapshots_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::EstimateStorageForConsolidateSnapshots_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

HttpNfcLease VirtualMachine::ExportVm()
{
	vw1__ExportVmRequestType req;
	req._USCOREthis = _mor;

	// call_defs ExportVm to post the request to ESX server or virtual center
	_vw1__ExportVmResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ExportVm))
	{
		return HttpNfcLease(rsp.returnval);
	}

	throw get_last_error();
}

std::string VirtualMachine::ExtractOvfEnvironment()
{
	vw1__ExtractOvfEnvironmentRequestType req;
	req._USCOREthis = _mor;

	// call_defs ExtractOvfEnvironment to post the request to ESX server or virtual center
	_vw1__ExtractOvfEnvironmentResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ExtractOvfEnvironment))
	{
		return static_cast<std::string>(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::MakePrimaryVM_Task(VirtualMachine vm)
{
	vw1__MakePrimaryVMRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;

	// call_defs MakePrimaryVM_USCORETask to post the request to ESX server or virtual center
	_vw1__MakePrimaryVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MakePrimaryVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void VirtualMachine::MarkAsTemplate()
{
	vw1__MarkAsTemplateRequestType req;
	req._USCOREthis = _mor;

	// call_defs MarkAsTemplate to post the request to ESX server or virtual center
	_vw1__MarkAsTemplateResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MarkAsTemplate))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void VirtualMachine::MarkAsVirtualMachine(ResourcePool pool, HostSystem host)
{
	vw1__MarkAsVirtualMachineRequestType req;
	req._USCOREthis = _mor;
	req.pool = pool;
	req.host = host;

	// call_defs MarkAsVirtualMachine to post the request to ESX server or virtual center
	_vw1__MarkAsVirtualMachineResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MarkAsVirtualMachine))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task VirtualMachine::MigrateVM_Task(ResourcePool pool, HostSystem host, vw1__VirtualMachineMovePriority priority, vw1__VirtualMachinePowerState*  state)
{
	vw1__MigrateVMRequestType req;
	req._USCOREthis = _mor;
	req.pool = pool;
	req.host = host;
	req.priority = priority;
	req.state = state;

	// call_defs MigrateVM_USCORETask to post the request to ESX server or virtual center
	_vw1__MigrateVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MigrateVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void VirtualMachine::MountToolsInstaller()
{
	vw1__MountToolsInstallerRequestType req;
	req._USCOREthis = _mor;

	// call_defs MountToolsInstaller to post the request to ESX server or virtual center
	_vw1__MountToolsInstallerResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MountToolsInstaller))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task VirtualMachine::PowerOffVM_Task()
{
	vw1__PowerOffVMRequestType req;
	req._USCOREthis = _mor;

	// call_defs PowerOffVM_USCORETask to post the request to ESX server or virtual center
	_vw1__PowerOffVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::PowerOffVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::PowerOnVM_Task(HostSystem host)
{
	vw1__PowerOnVMRequestType req;
	req._USCOREthis = _mor;
	req.host = host;

	// call_defs PowerOnVM_USCORETask to post the request to ESX server or virtual center
	_vw1__PowerOnVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::PowerOnVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::PromoteDisks_Task(bool unlink, int sizedisks, vw1__VirtualDisk**  disks)
{
	vw1__PromoteDisksRequestType req;
	req._USCOREthis = _mor;
	req.unlink = unlink;
	req.__sizedisks = sizedisks;
	req.disks = disks;

	// call_defs PromoteDisks_USCORETask to post the request to ESX server or virtual center
	_vw1__PromoteDisks_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::PromoteDisks_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

vw1__DiskChangeInfo VirtualMachine::QueryChangedDiskAreas(VirtualMachineSnapshot snapshot, int deviceKey, __int64 startOffset, char* changeId)
{
	vw1__QueryChangedDiskAreasRequestType req;
	req._USCOREthis = _mor;
	req.snapshot = snapshot;
	req.deviceKey = deviceKey;
	req.startOffset = startOffset;
	req.changeId = changeId;

	// call_defs QueryChangedDiskAreas to post the request to ESX server or virtual center
	_vw1__QueryChangedDiskAreasResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryChangedDiskAreas))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

std::vector<vw1__LocalizedMethodFault> VirtualMachine::QueryFaultToleranceCompatibility()
{
	vw1__QueryFaultToleranceCompatibilityRequestType req;
	req._USCOREthis = _mor;

	// call_defs QueryFaultToleranceCompatibility to post the request to ESX server or virtual center
	_vw1__QueryFaultToleranceCompatibilityResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryFaultToleranceCompatibility))
	{
		std::vector<vw1__LocalizedMethodFault> vw1__localizedmethodfaults;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__LocalizedMethodFault tmp(*rsp.returnval[i]);
			vw1__localizedmethodfaults.push_back(tmp);
		}
		return vw1__localizedmethodfaults;
	}

	throw get_last_error();
}

std::vector<std::string> VirtualMachine::QueryUnownedFiles()
{
	vw1__QueryUnownedFilesRequestType req;
	req._USCOREthis = _mor;

	// call_defs QueryUnownedFiles to post the request to ESX server or virtual center
	_vw1__QueryUnownedFilesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryUnownedFiles))
	{
		std::vector<std::string> strings;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			std::string tmp(rsp.returnval[i]);
			strings.push_back(tmp);
		}
		return strings;
	}

	throw get_last_error();
}

void VirtualMachine::RebootGuest()
{
	vw1__RebootGuestRequestType req;
	req._USCOREthis = _mor;

	// call_defs RebootGuest to post the request to ESX server or virtual center
	_vw1__RebootGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RebootGuest))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task VirtualMachine::ReconfigVM_Task(vw1__VirtualMachineConfigSpec*  spec)
{
	vw1__ReconfigVMRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;

	// call_defs ReconfigVM_USCORETask to post the request to ESX server or virtual center
	_vw1__ReconfigVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReconfigVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void VirtualMachine::RefreshStorageInfo()
{
	vw1__RefreshStorageInfoRequestType req;
	req._USCOREthis = _mor;

	// call_defs RefreshStorageInfo to post the request to ESX server or virtual center
	_vw1__RefreshStorageInfoResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RefreshStorageInfo))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task VirtualMachine::reloadVirtualMachineFromPath_Task(char* configurationPath)
{
	vw1__reloadVirtualMachineFromPathRequestType req;
	req._USCOREthis = _mor;
	req.configurationPath = configurationPath;

	// call_defs reloadVirtualMachineFromPath_USCORETask to post the request to ESX server or virtual center
	_vw1__reloadVirtualMachineFromPath_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::reloadVirtualMachineFromPath_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::RelocateVM_Task(vw1__VirtualMachineRelocateSpec*  spec, vw1__VirtualMachineMovePriority*  priority)
{
	vw1__RelocateVMRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;
	req.priority = priority;

	// call_defs RelocateVM_USCORETask to post the request to ESX server or virtual center
	_vw1__RelocateVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RelocateVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::RemoveAllSnapshots_Task(bool*  consolidate)
{
	vw1__RemoveAllSnapshotsRequestType req;
	req._USCOREthis = _mor;
	req.consolidate = consolidate;

	// call_defs RemoveAllSnapshots_USCORETask to post the request to ESX server or virtual center
	_vw1__RemoveAllSnapshots_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemoveAllSnapshots_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void VirtualMachine::ResetGuestInformation()
{
	vw1__ResetGuestInformationRequestType req;
	req._USCOREthis = _mor;

	// call_defs ResetGuestInformation to post the request to ESX server or virtual center
	_vw1__ResetGuestInformationResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ResetGuestInformation))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task VirtualMachine::ResetVM_Task()
{
	vw1__ResetVMRequestType req;
	req._USCOREthis = _mor;

	// call_defs ResetVM_USCORETask to post the request to ESX server or virtual center
	_vw1__ResetVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ResetVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::RevertToCurrentSnapshot_Task(HostSystem host, bool*  suppressPowerOn)
{
	vw1__RevertToCurrentSnapshotRequestType req;
	req._USCOREthis = _mor;
	req.host = host;
	req.suppressPowerOn = suppressPowerOn;

	// call_defs RevertToCurrentSnapshot_USCORETask to post the request to ESX server or virtual center
	_vw1__RevertToCurrentSnapshot_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RevertToCurrentSnapshot_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void VirtualMachine::SetDisplayTopology(int sizedisplays, vw1__VirtualMachineDisplayTopology**  displays)
{
	vw1__SetDisplayTopologyRequestType req;
	req._USCOREthis = _mor;
	req.__sizedisplays = sizedisplays;
	req.displays = displays;

	// call_defs SetDisplayTopology to post the request to ESX server or virtual center
	_vw1__SetDisplayTopologyResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SetDisplayTopology))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void VirtualMachine::SetScreenResolution(int width, int height)
{
	vw1__SetScreenResolutionRequestType req;
	req._USCOREthis = _mor;
	req.width = width;
	req.height = height;

	// call_defs SetScreenResolution to post the request to ESX server or virtual center
	_vw1__SetScreenResolutionResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SetScreenResolution))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void VirtualMachine::ShutdownGuest()
{
	vw1__ShutdownGuestRequestType req;
	req._USCOREthis = _mor;

	// call_defs ShutdownGuest to post the request to ESX server or virtual center
	_vw1__ShutdownGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ShutdownGuest))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void VirtualMachine::StandbyGuest()
{
	vw1__StandbyGuestRequestType req;
	req._USCOREthis = _mor;

	// call_defs StandbyGuest to post the request to ESX server or virtual center
	_vw1__StandbyGuestResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::StandbyGuest))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task VirtualMachine::StartRecording_Task(char* name, char* description)
{
	vw1__StartRecordingRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.description = description;

	// call_defs StartRecording_USCORETask to post the request to ESX server or virtual center
	_vw1__StartRecording_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::StartRecording_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::StartReplaying_Task(VirtualMachineSnapshot replaySnapshot)
{
	vw1__StartReplayingRequestType req;
	req._USCOREthis = _mor;
	req.replaySnapshot = replaySnapshot;

	// call_defs StartReplaying_USCORETask to post the request to ESX server or virtual center
	_vw1__StartReplaying_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::StartReplaying_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::StopRecording_Task()
{
	vw1__StopRecordingRequestType req;
	req._USCOREthis = _mor;

	// call_defs StopRecording_USCORETask to post the request to ESX server or virtual center
	_vw1__StopRecording_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::StopRecording_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::StopReplaying_Task()
{
	vw1__StopReplayingRequestType req;
	req._USCOREthis = _mor;

	// call_defs StopReplaying_USCORETask to post the request to ESX server or virtual center
	_vw1__StopReplaying_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::StopReplaying_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::SuspendVM_Task()
{
	vw1__SuspendVMRequestType req;
	req._USCOREthis = _mor;

	// call_defs SuspendVM_USCORETask to post the request to ESX server or virtual center
	_vw1__SuspendVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::SuspendVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::TerminateFaultTolerantVM_Task(VirtualMachine vm)
{
	vw1__TerminateFaultTolerantVMRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;

	// call_defs TerminateFaultTolerantVM_USCORETask to post the request to ESX server or virtual center
	_vw1__TerminateFaultTolerantVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::TerminateFaultTolerantVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::TurnOffFaultToleranceForVM_Task()
{
	vw1__TurnOffFaultToleranceForVMRequestType req;
	req._USCOREthis = _mor;

	// call_defs TurnOffFaultToleranceForVM_USCORETask to post the request to ESX server or virtual center
	_vw1__TurnOffFaultToleranceForVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::TurnOffFaultToleranceForVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void VirtualMachine::UnmountToolsInstaller()
{
	vw1__UnmountToolsInstallerRequestType req;
	req._USCOREthis = _mor;

	// call_defs UnmountToolsInstaller to post the request to ESX server or virtual center
	_vw1__UnmountToolsInstallerResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UnmountToolsInstaller))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void VirtualMachine::UnregisterVM()
{
	vw1__UnregisterVMRequestType req;
	req._USCOREthis = _mor;

	// call_defs UnregisterVM to post the request to ESX server or virtual center
	_vw1__UnregisterVMResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UnregisterVM))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task VirtualMachine::UpgradeTools_Task(char* installerOptions)
{
	vw1__UpgradeToolsRequestType req;
	req._USCOREthis = _mor;
	req.installerOptions = installerOptions;

	// call_defs UpgradeTools_USCORETask to post the request to ESX server or virtual center
	_vw1__UpgradeTools_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpgradeTools_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachine::UpgradeVM_Task(char* version)
{
	vw1__UpgradeVMRequestType req;
	req._USCOREthis = _mor;
	req.version = version;

	// call_defs UpgradeVM_USCORETask to post the request to ESX server or virtual center
	_vw1__UpgradeVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpgradeVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}
vw1__VirtualMachineCapability VirtualMachine::get_capability() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("capability", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__VirtualMachineCapability();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__VirtualMachineCapability*>(it->val);
}

vw1__VirtualMachineConfigInfo VirtualMachine::get_config() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("config", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__VirtualMachineConfigInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__VirtualMachineConfigInfo*>(it->val);
}

std::vector<Datastore> VirtualMachine::get_datastore() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("datastore", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<Datastore>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<Datastore> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(Datastore(amo->ManagedObjectReference[i]));
	return ret;
}

EnvironmentBrowser VirtualMachine::get_environmentBrowser() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("environmentBrowser", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return EnvironmentBrowser();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return EnvironmentBrowser(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}

vw1__GuestInfo VirtualMachine::get_guest() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("guest", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__GuestInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__GuestInfo*>(it->val);
}

vw1__ManagedEntityStatus VirtualMachine::get_guestHeartbeatStatus() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("guestHeartbeatStatus", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__ManagedEntityStatus();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<vw1__ManagedEntityStatus_*>(it->val)->__item;
}

vw1__VirtualMachineFileLayout VirtualMachine::get_layout() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("layout", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__VirtualMachineFileLayout();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__VirtualMachineFileLayout*>(it->val);
}

vw1__VirtualMachineFileLayoutEx VirtualMachine::get_layoutEx() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("layoutEx", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__VirtualMachineFileLayoutEx();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__VirtualMachineFileLayoutEx*>(it->val);
}

std::vector<Network> VirtualMachine::get_network() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("network", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<Network>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<Network> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(Network(amo->ManagedObjectReference[i]));
	return ret;
}

ManagedEntity VirtualMachine::get_parentVApp() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("parentVApp", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return ManagedEntity();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return ManagedEntity(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}

vw1__ResourceConfigSpec VirtualMachine::get_resourceConfig() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("resourceConfig", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__ResourceConfigSpec();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__ResourceConfigSpec*>(it->val);
}

ResourcePool VirtualMachine::get_resourcePool() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("resourcePool", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return ResourcePool();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return ResourcePool(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}

std::vector<VirtualMachineSnapshot> VirtualMachine::get_rootSnapshot() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("rootSnapshot", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<VirtualMachineSnapshot>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<VirtualMachineSnapshot> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(VirtualMachineSnapshot(amo->ManagedObjectReference[i]));
	return ret;
}

vw1__VirtualMachineRuntimeInfo VirtualMachine::get_runtime() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("runtime", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__VirtualMachineRuntimeInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__VirtualMachineRuntimeInfo*>(it->val);
}

vw1__VirtualMachineSnapshotInfo VirtualMachine::get_snapshot() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("snapshot", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__VirtualMachineSnapshotInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__VirtualMachineSnapshotInfo*>(it->val);
}

vw1__VirtualMachineStorageInfo VirtualMachine::get_storage() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("storage", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__VirtualMachineStorageInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__VirtualMachineStorageInfo*>(it->val);
}

vw1__VirtualMachineSummary VirtualMachine::get_summary() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("summary", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__VirtualMachineSummary();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__VirtualMachineSummary*>(it->val);
}


const char* VirtualMachineCompatibilityChecker::type = "VirtualMachineCompatibilityChecker";
VirtualMachineCompatibilityChecker::VirtualMachineCompatibilityChecker(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
VirtualMachineCompatibilityChecker::~VirtualMachineCompatibilityChecker() {}

Task VirtualMachineCompatibilityChecker::CheckCompatibility_Task(VirtualMachine vm, HostSystem host, ResourcePool pool, int sizetestType, char** testType)
{
	vw1__CheckCompatibilityRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.host = host;
	req.pool = pool;
	req.__sizetestType = sizetestType;
	req.testType = testType;

	// call_defs CheckCompatibility_USCORETask to post the request to ESX server or virtual center
	_vw1__CheckCompatibility_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CheckCompatibility_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

const char* VirtualMachineProvisioningChecker::type = "VirtualMachineProvisioningChecker";
VirtualMachineProvisioningChecker::VirtualMachineProvisioningChecker(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
VirtualMachineProvisioningChecker::~VirtualMachineProvisioningChecker() {}

Task VirtualMachineProvisioningChecker::CheckMigrate_Task(VirtualMachine vm, HostSystem host, ResourcePool pool, vw1__VirtualMachinePowerState*  state, int sizetestType, char** testType)
{
	vw1__CheckMigrateRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.host = host;
	req.pool = pool;
	req.state = state;
	req.__sizetestType = sizetestType;
	req.testType = testType;

	// call_defs CheckMigrate_USCORETask to post the request to ESX server or virtual center
	_vw1__CheckMigrate_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CheckMigrate_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachineProvisioningChecker::CheckRelocate_Task(VirtualMachine vm, vw1__VirtualMachineRelocateSpec*  spec, int sizetestType, char** testType)
{
	vw1__CheckRelocateRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.spec = spec;
	req.__sizetestType = sizetestType;
	req.testType = testType;

	// call_defs CheckRelocate_USCORETask to post the request to ESX server or virtual center
	_vw1__CheckRelocate_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CheckRelocate_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task VirtualMachineProvisioningChecker::QueryVMotionCompatibilityEx_Task(int sizevm, VirtualMachine* vm, int sizehost, HostSystem* host)
{
	vw1__QueryVMotionCompatibilityExRequestType req;
	req._USCOREthis = _mor;
	req.__sizevm = sizevm;
	vw1__ManagedObjectReference ** pvm = new vw1__ManagedObjectReference *[sizevm];
	for (int i = 0; i < sizevm; ++i)
		pvm[i] = vm[i];
	req.vm = pvm;
	req.__sizehost = sizehost;
	vw1__ManagedObjectReference ** phost = new vw1__ManagedObjectReference *[sizehost];
	for (int i = 0; i < sizehost; ++i)
		phost[i] = host[i];
	req.host = phost;

	// call_defs QueryVMotionCompatibilityEx_USCORETask to post the request to ESX server or virtual center
	_vw1__QueryVMotionCompatibilityEx_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryVMotionCompatibilityEx_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

const char* VirtualMachineSnapshot::type = "VirtualMachineSnapshot";
VirtualMachineSnapshot::VirtualMachineSnapshot(vw1__ManagedObjectReference* mor) : ExtensibleManagedObject(mor) {}
VirtualMachineSnapshot::~VirtualMachineSnapshot() {}

Task VirtualMachineSnapshot::RemoveSnapshot_Task(bool removeChildren, bool*  consolidate)
{
	vw1__RemoveSnapshotRequestType req;
	req._USCOREthis = _mor;
	req.removeChildren = removeChildren;
	req.consolidate = consolidate;

	// call_defs RemoveSnapshot_USCORETask to post the request to ESX server or virtual center
	_vw1__RemoveSnapshot_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemoveSnapshot_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void VirtualMachineSnapshot::RenameSnapshot(char* name, char* description)
{
	vw1__RenameSnapshotRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.description = description;

	// call_defs RenameSnapshot to post the request to ESX server or virtual center
	_vw1__RenameSnapshotResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RenameSnapshot))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task VirtualMachineSnapshot::RevertToSnapshot_Task(HostSystem host, bool*  suppressPowerOn)
{
	vw1__RevertToSnapshotRequestType req;
	req._USCOREthis = _mor;
	req.host = host;
	req.suppressPowerOn = suppressPowerOn;

	// call_defs RevertToSnapshot_USCORETask to post the request to ESX server or virtual center
	_vw1__RevertToSnapshot_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RevertToSnapshot_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}
std::vector<VirtualMachineSnapshot> VirtualMachineSnapshot::get_childSnapshot() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("childSnapshot", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<VirtualMachineSnapshot>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<VirtualMachineSnapshot> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(VirtualMachineSnapshot(amo->ManagedObjectReference[i]));
	return ret;
}

vw1__VirtualMachineConfigInfo VirtualMachineSnapshot::get_config() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("config", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__VirtualMachineConfigInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__VirtualMachineConfigInfo*>(it->val);
}


const char* VirtualizationManager::type = "VirtualizationManager";
VirtualizationManager::VirtualizationManager(vw1__ManagedObjectReference* mor) : mor_handle(mor) {}
VirtualizationManager::~VirtualizationManager() {}

const char* Alarm::type = "Alarm";
Alarm::Alarm(vw1__ManagedObjectReference* mor) : ExtensibleManagedObject(mor) {}
Alarm::~Alarm() {}

void Alarm::ReconfigureAlarm(vw1__AlarmSpec*  spec)
{
	vw1__ReconfigureAlarmRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;

	// call_defs ReconfigureAlarm to post the request to ESX server or virtual center
	_vw1__ReconfigureAlarmResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReconfigureAlarm))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void Alarm::RemoveAlarm()
{
	vw1__RemoveAlarmRequestType req;
	req._USCOREthis = _mor;

	// call_defs RemoveAlarm to post the request to ESX server or virtual center
	_vw1__RemoveAlarmResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemoveAlarm))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__AlarmInfo Alarm::get_info() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("info", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__AlarmInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__AlarmInfo*>(it->val);
}


const char* ClusterProfile::type = "ClusterProfile";
ClusterProfile::ClusterProfile(vw1__ManagedObjectReference* mor) : Profile(mor) {}
ClusterProfile::~ClusterProfile() {}

void ClusterProfile::UpdateClusterProfile(vw1__ClusterProfileConfigSpec*  config)
{
	vw1__UpdateClusterProfileRequestType req;
	req._USCOREthis = _mor;
	req.config = config;

	// call_defs UpdateClusterProfile to post the request to ESX server or virtual center
	_vw1__UpdateClusterProfileResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateClusterProfile))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

const char* ClusterProfileManager::type = "ClusterProfileManager";
ClusterProfileManager::ClusterProfileManager(vw1__ManagedObjectReference* mor) : ProfileManager(mor) {}
ClusterProfileManager::~ClusterProfileManager() {}

const char* ComputeResource::type = "ComputeResource";
ComputeResource::ComputeResource(vw1__ManagedObjectReference* mor) : ManagedEntity(mor) {}
ComputeResource::~ComputeResource() {}

Task ComputeResource::ReconfigureComputeResource_Task(vw1__ComputeResourceConfigSpec*  spec, bool modify)
{
	vw1__ReconfigureComputeResourceRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;
	req.modify = modify;

	// call_defs ReconfigureComputeResource_USCORETask to post the request to ESX server or virtual center
	_vw1__ReconfigureComputeResource_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReconfigureComputeResource_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}
vw1__ComputeResourceConfigInfo ComputeResource::get_configurationEx() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("configurationEx", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__ComputeResourceConfigInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__ComputeResourceConfigInfo*>(it->val);
}

std::vector<Datastore> ComputeResource::get_datastore() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("datastore", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<Datastore>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<Datastore> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(Datastore(amo->ManagedObjectReference[i]));
	return ret;
}

EnvironmentBrowser ComputeResource::get_environmentBrowser() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("environmentBrowser", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return EnvironmentBrowser();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return EnvironmentBrowser(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}

std::vector<HostSystem> ComputeResource::get_host() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("host", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<HostSystem>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<HostSystem> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(HostSystem(amo->ManagedObjectReference[i]));
	return ret;
}

std::vector<Network> ComputeResource::get_network() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("network", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<Network>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<Network> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(Network(amo->ManagedObjectReference[i]));
	return ret;
}

ResourcePool ComputeResource::get_resourcePool() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("resourcePool", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return ResourcePool();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return ResourcePool(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}

vw1__ComputeResourceSummary ComputeResource::get_summary() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("summary", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__ComputeResourceSummary();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__ComputeResourceSummary*>(it->val);
}


const char* Datacenter::type = "Datacenter";
Datacenter::Datacenter(vw1__ManagedObjectReference* mor) : ManagedEntity(mor) {}
Datacenter::~Datacenter() {}

Task Datacenter::PowerOnMultiVM_Task(int sizevm, VirtualMachine* vm, int sizeoption, vw1__OptionValue**  option)
{
	vw1__PowerOnMultiVMRequestType req;
	req._USCOREthis = _mor;
	req.__sizevm = sizevm;
	vw1__ManagedObjectReference ** pvm = new vw1__ManagedObjectReference *[sizevm];
	for (int i = 0; i < sizevm; ++i)
		pvm[i] = vm[i];
	req.vm = pvm;
	req.__sizeoption = sizeoption;
	req.option = option;

	// call_defs PowerOnMultiVM_USCORETask to post the request to ESX server or virtual center
	_vw1__PowerOnMultiVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::PowerOnMultiVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

vw1__HostConnectInfo Datacenter::QueryConnectionInfo(char* hostname, int port, char* username, char* password, char* sslThumbprint)
{
	vw1__QueryConnectionInfoRequestType req;
	req._USCOREthis = _mor;
	req.hostname = hostname;
	req.port = port;
	req.username = username;
	req.password = password;
	req.sslThumbprint = sslThumbprint;

	// call_defs QueryConnectionInfo to post the request to ESX server or virtual center
	_vw1__QueryConnectionInfoResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryConnectionInfo))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}
std::vector<Datastore> Datacenter::get_datastore() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("datastore", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<Datastore>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<Datastore> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(Datastore(amo->ManagedObjectReference[i]));
	return ret;
}

Folder Datacenter::get_datastoreFolder() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("datastoreFolder", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return Folder();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return Folder(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}

Folder Datacenter::get_hostFolder() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("hostFolder", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return Folder();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return Folder(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}

std::vector<Network> Datacenter::get_network() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("network", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<Network>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<Network> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(Network(amo->ManagedObjectReference[i]));
	return ret;
}

Folder Datacenter::get_networkFolder() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("networkFolder", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return Folder();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return Folder(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}

Folder Datacenter::get_vmFolder() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("vmFolder", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return Folder();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return Folder(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}


const char* Datastore::type = "Datastore";
Datastore::Datastore(vw1__ManagedObjectReference* mor) : ManagedEntity(mor) {}
Datastore::~Datastore() {}

vw1__StoragePlacementResult Datastore::DatastoreEnterMaintenanceMode()
{
	vw1__DatastoreEnterMaintenanceModeRequestType req;
	req._USCOREthis = _mor;

	// call_defs DatastoreEnterMaintenanceMode to post the request to ESX server or virtual center
	_vw1__DatastoreEnterMaintenanceModeResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DatastoreEnterMaintenanceMode))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

Task Datastore::DatastoreExitMaintenanceMode_Task()
{
	vw1__DatastoreExitMaintenanceModeRequestType req;
	req._USCOREthis = _mor;

	// call_defs DatastoreExitMaintenanceMode_USCORETask to post the request to ESX server or virtual center
	_vw1__DatastoreExitMaintenanceMode_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DatastoreExitMaintenanceMode_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void Datastore::DestroyDatastore()
{
	vw1__DestroyDatastoreRequestType req;
	req._USCOREthis = _mor;

	// call_defs DestroyDatastore to post the request to ESX server or virtual center
	_vw1__DestroyDatastoreResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DestroyDatastore))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void Datastore::RefreshDatastore()
{
	vw1__RefreshDatastoreRequestType req;
	req._USCOREthis = _mor;

	// call_defs RefreshDatastore to post the request to ESX server or virtual center
	_vw1__RefreshDatastoreResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RefreshDatastore))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void Datastore::RefreshDatastoreStorageInfo()
{
	vw1__RefreshDatastoreStorageInfoRequestType req;
	req._USCOREthis = _mor;

	// call_defs RefreshDatastoreStorageInfo to post the request to ESX server or virtual center
	_vw1__RefreshDatastoreStorageInfoResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RefreshDatastoreStorageInfo))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void Datastore::RenameDatastore(char* newName)
{
	vw1__RenameDatastoreRequestType req;
	req._USCOREthis = _mor;
	req.newName = newName;

	// call_defs RenameDatastore to post the request to ESX server or virtual center
	_vw1__RenameDatastoreResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RenameDatastore))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task Datastore::UpdateVirtualMachineFiles_Task(int sizemountPathDatastoreMapping, vw1__DatastoreMountPathDatastorePair**  mountPathDatastoreMapping)
{
	vw1__UpdateVirtualMachineFilesRequestType req;
	req._USCOREthis = _mor;
	req.__sizemountPathDatastoreMapping = sizemountPathDatastoreMapping;
	req.mountPathDatastoreMapping = mountPathDatastoreMapping;

	// call_defs UpdateVirtualMachineFiles_USCORETask to post the request to ESX server or virtual center
	_vw1__UpdateVirtualMachineFiles_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateVirtualMachineFiles_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}
HostDatastoreBrowser Datastore::get_browser() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("browser", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return HostDatastoreBrowser();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return HostDatastoreBrowser(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}

vw1__DatastoreCapability Datastore::get_capability() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("capability", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__DatastoreCapability();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__DatastoreCapability*>(it->val);
}

std::vector<vw1__DatastoreHostMount> Datastore::get_host() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("host", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__DatastoreHostMount>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfDatastoreHostMount* amo = dynamic_cast<vw1__ArrayOfDatastoreHostMount*>(it->val);
	std::vector<vw1__DatastoreHostMount> ret;
	for (int i = 0; i < amo->__sizeDatastoreHostMount; ++i)
		ret.push_back(*amo->DatastoreHostMount[i]);
	return ret;
}

vw1__DatastoreInfo Datastore::get_info() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("info", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__DatastoreInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__DatastoreInfo*>(it->val);
}

vw1__StorageIORMInfo Datastore::get_iormConfiguration() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("iormConfiguration", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__StorageIORMInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__StorageIORMInfo*>(it->val);
}

vw1__DatastoreSummary Datastore::get_summary() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("summary", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__DatastoreSummary();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__DatastoreSummary*>(it->val);
}

std::vector<VirtualMachine> Datastore::get_vm() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("vm", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<VirtualMachine>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<VirtualMachine> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(VirtualMachine(amo->ManagedObjectReference[i]));
	return ret;
}


const char* DistributedVirtualPortgroup::type = "DistributedVirtualPortgroup";
DistributedVirtualPortgroup::DistributedVirtualPortgroup(vw1__ManagedObjectReference* mor) : Network(mor) {}
DistributedVirtualPortgroup::~DistributedVirtualPortgroup() {}

Task DistributedVirtualPortgroup::ReconfigureDVPortgroup_Task(vw1__DVPortgroupConfigSpec*  spec)
{
	vw1__ReconfigureDVPortgroupRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;

	// call_defs ReconfigureDVPortgroup_USCORETask to post the request to ESX server or virtual center
	_vw1__ReconfigureDVPortgroup_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReconfigureDVPortgroup_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}
vw1__DVPortgroupConfigInfo DistributedVirtualPortgroup::get_config() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("config", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__DVPortgroupConfigInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__DVPortgroupConfigInfo*>(it->val);
}

std::string DistributedVirtualPortgroup::get_key() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("key", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::string();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<xsd__string*>(it->val)->__item;
}

std::vector<std::string> DistributedVirtualPortgroup::get_portKeys() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("portKeys", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<std::string>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfString* amo = dynamic_cast<vw1__ArrayOfString*>(it->val);
	std::vector<std::string> ret;
	for (int i = 0; i < amo->__sizestring; ++i)
		ret.push_back(amo->string[i]);
	return ret;
}


const char* DistributedVirtualSwitch::type = "DistributedVirtualSwitch";
DistributedVirtualSwitch::DistributedVirtualSwitch(vw1__ManagedObjectReference* mor) : ManagedEntity(mor) {}
DistributedVirtualSwitch::~DistributedVirtualSwitch() {}

Task DistributedVirtualSwitch::AddDVPortgroup_Task(int sizespec, vw1__DVPortgroupConfigSpec**  spec)
{
	vw1__AddDVPortgroupRequestType req;
	req._USCOREthis = _mor;
	req.__sizespec = sizespec;
	req.spec = spec;

	// call_defs AddDVPortgroup_USCORETask to post the request to ESX server or virtual center
	_vw1__AddDVPortgroup_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AddDVPortgroup_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void DistributedVirtualSwitch::AddNetworkResourcePool(int sizeconfigSpec, vw1__DVSNetworkResourcePoolConfigSpec**  configSpec)
{
	vw1__AddNetworkResourcePoolRequestType req;
	req._USCOREthis = _mor;
	req.__sizeconfigSpec = sizeconfigSpec;
	req.configSpec = configSpec;

	// call_defs AddNetworkResourcePool to post the request to ESX server or virtual center
	_vw1__AddNetworkResourcePoolResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AddNetworkResourcePool))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void DistributedVirtualSwitch::EnableNetworkResourceManagement(bool enable)
{
	vw1__EnableNetworkResourceManagementRequestType req;
	req._USCOREthis = _mor;
	req.enable = enable;

	// call_defs EnableNetworkResourceManagement to post the request to ESX server or virtual center
	_vw1__EnableNetworkResourceManagementResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::EnableNetworkResourceManagement))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

std::vector<std::string> DistributedVirtualSwitch::FetchDVPortKeys(vw1__DistributedVirtualSwitchPortCriteria*  criteria)
{
	vw1__FetchDVPortKeysRequestType req;
	req._USCOREthis = _mor;
	req.criteria = criteria;

	// call_defs FetchDVPortKeys to post the request to ESX server or virtual center
	_vw1__FetchDVPortKeysResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::FetchDVPortKeys))
	{
		std::vector<std::string> strings;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			std::string tmp(rsp.returnval[i]);
			strings.push_back(tmp);
		}
		return strings;
	}

	throw get_last_error();
}

std::vector<vw1__DistributedVirtualPort> DistributedVirtualSwitch::FetchDVPorts(vw1__DistributedVirtualSwitchPortCriteria*  criteria)
{
	vw1__FetchDVPortsRequestType req;
	req._USCOREthis = _mor;
	req.criteria = criteria;

	// call_defs FetchDVPorts to post the request to ESX server or virtual center
	_vw1__FetchDVPortsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::FetchDVPorts))
	{
		std::vector<vw1__DistributedVirtualPort> vw1__distributedvirtualports;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__DistributedVirtualPort tmp(*rsp.returnval[i]);
			vw1__distributedvirtualports.push_back(tmp);
		}
		return vw1__distributedvirtualports;
	}

	throw get_last_error();
}

Task DistributedVirtualSwitch::MergeDvs_Task(DistributedVirtualSwitch dvs)
{
	vw1__MergeDvsRequestType req;
	req._USCOREthis = _mor;
	req.dvs = dvs;

	// call_defs MergeDvs_USCORETask to post the request to ESX server or virtual center
	_vw1__MergeDvs_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MergeDvs_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task DistributedVirtualSwitch::MoveDVPort_Task(int sizeportKey, char** portKey, char* destinationPortgroupKey)
{
	vw1__MoveDVPortRequestType req;
	req._USCOREthis = _mor;
	req.__sizeportKey = sizeportKey;
	req.portKey = portKey;
	req.destinationPortgroupKey = destinationPortgroupKey;

	// call_defs MoveDVPort_USCORETask to post the request to ESX server or virtual center
	_vw1__MoveDVPort_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MoveDVPort_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task DistributedVirtualSwitch::PerformDvsProductSpecOperation_Task(char* operation, vw1__DistributedVirtualSwitchProductSpec*  productSpec)
{
	vw1__PerformDvsProductSpecOperationRequestType req;
	req._USCOREthis = _mor;
	req.operation = operation;
	req.productSpec = productSpec;

	// call_defs PerformDvsProductSpecOperation_USCORETask to post the request to ESX server or virtual center
	_vw1__PerformDvsProductSpecOperation_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::PerformDvsProductSpecOperation_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

std::vector<int> DistributedVirtualSwitch::QueryUsedVlanIdInDvs()
{
	vw1__QueryUsedVlanIdInDvsRequestType req;
	req._USCOREthis = _mor;

	// call_defs QueryUsedVlanIdInDvs to post the request to ESX server or virtual center
	_vw1__QueryUsedVlanIdInDvsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryUsedVlanIdInDvs))
	{
		std::vector<int> ints;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			int tmp(rsp.returnval[i]);
			ints.push_back(tmp);
		}
		return ints;
	}

	throw get_last_error();
}

Task DistributedVirtualSwitch::ReconfigureDVPort_Task(int sizeport, vw1__DVPortConfigSpec**  port)
{
	vw1__ReconfigureDVPortRequestType req;
	req._USCOREthis = _mor;
	req.__sizeport = sizeport;
	req.port = port;

	// call_defs ReconfigureDVPort_USCORETask to post the request to ESX server or virtual center
	_vw1__ReconfigureDVPort_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReconfigureDVPort_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task DistributedVirtualSwitch::ReconfigureDvs_Task(vw1__DVSConfigSpec*  spec)
{
	vw1__ReconfigureDvsRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;

	// call_defs ReconfigureDvs_USCORETask to post the request to ESX server or virtual center
	_vw1__ReconfigureDvs_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReconfigureDvs_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task DistributedVirtualSwitch::RectifyDvsHost_Task(int sizehosts, HostSystem* hosts)
{
	vw1__RectifyDvsHostRequestType req;
	req._USCOREthis = _mor;
	req.__sizehosts = sizehosts;
	vw1__ManagedObjectReference ** phosts = new vw1__ManagedObjectReference *[sizehosts];
	for (int i = 0; i < sizehosts; ++i)
		phosts[i] = hosts[i];
	req.hosts = phosts;

	// call_defs RectifyDvsHost_USCORETask to post the request to ESX server or virtual center
	_vw1__RectifyDvsHost_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RectifyDvsHost_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void DistributedVirtualSwitch::RefreshDVPortState(int sizeportKeys, char** portKeys)
{
	vw1__RefreshDVPortStateRequestType req;
	req._USCOREthis = _mor;
	req.__sizeportKeys = sizeportKeys;
	req.portKeys = portKeys;

	// call_defs RefreshDVPortState to post the request to ESX server or virtual center
	_vw1__RefreshDVPortStateResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RefreshDVPortState))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void DistributedVirtualSwitch::RemoveNetworkResourcePool(int sizekey, char** key)
{
	vw1__RemoveNetworkResourcePoolRequestType req;
	req._USCOREthis = _mor;
	req.__sizekey = sizekey;
	req.key = key;

	// call_defs RemoveNetworkResourcePool to post the request to ESX server or virtual center
	_vw1__RemoveNetworkResourcePoolResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RemoveNetworkResourcePool))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void DistributedVirtualSwitch::UpdateDvsCapability(vw1__DVSCapability*  capability)
{
	vw1__UpdateDvsCapabilityRequestType req;
	req._USCOREthis = _mor;
	req.capability = capability;

	// call_defs UpdateDvsCapability to post the request to ESX server or virtual center
	_vw1__UpdateDvsCapabilityResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateDvsCapability))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void DistributedVirtualSwitch::UpdateNetworkResourcePool(int sizeconfigSpec, vw1__DVSNetworkResourcePoolConfigSpec**  configSpec)
{
	vw1__UpdateNetworkResourcePoolRequestType req;
	req._USCOREthis = _mor;
	req.__sizeconfigSpec = sizeconfigSpec;
	req.configSpec = configSpec;

	// call_defs UpdateNetworkResourcePool to post the request to ESX server or virtual center
	_vw1__UpdateNetworkResourcePoolResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateNetworkResourcePool))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__DVSCapability DistributedVirtualSwitch::get_capability() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("capability", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__DVSCapability();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__DVSCapability*>(it->val);
}

vw1__DVSConfigInfo DistributedVirtualSwitch::get_config() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("config", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__DVSConfigInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__DVSConfigInfo*>(it->val);
}

std::vector<vw1__DVSNetworkResourcePool> DistributedVirtualSwitch::get_networkResourcePool() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("networkResourcePool", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__DVSNetworkResourcePool>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfDVSNetworkResourcePool* amo = dynamic_cast<vw1__ArrayOfDVSNetworkResourcePool*>(it->val);
	std::vector<vw1__DVSNetworkResourcePool> ret;
	for (int i = 0; i < amo->__sizeDVSNetworkResourcePool; ++i)
		ret.push_back(*amo->DVSNetworkResourcePool[i]);
	return ret;
}

std::vector<DistributedVirtualPortgroup> DistributedVirtualSwitch::get_portgroup() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("portgroup", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<DistributedVirtualPortgroup>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<DistributedVirtualPortgroup> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(DistributedVirtualPortgroup(amo->ManagedObjectReference[i]));
	return ret;
}

vw1__DVSSummary DistributedVirtualSwitch::get_summary() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("summary", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__DVSSummary();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__DVSSummary*>(it->val);
}

std::string DistributedVirtualSwitch::get_uuid() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("uuid", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::string();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<xsd__string*>(it->val)->__item;
}


const char* EventHistoryCollector::type = "EventHistoryCollector";
EventHistoryCollector::EventHistoryCollector(vw1__ManagedObjectReference* mor) : HistoryCollector(mor) {}
EventHistoryCollector::~EventHistoryCollector() {}

std::vector<vw1__Event> EventHistoryCollector::ReadNextEvents(int maxCount)
{
	vw1__ReadNextEventsRequestType req;
	req._USCOREthis = _mor;
	req.maxCount = maxCount;

	// call_defs ReadNextEvents to post the request to ESX server or virtual center
	_vw1__ReadNextEventsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReadNextEvents))
	{
		std::vector<vw1__Event> vw1__events;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__Event tmp(*rsp.returnval[i]);
			vw1__events.push_back(tmp);
		}
		return vw1__events;
	}

	throw get_last_error();
}

std::vector<vw1__Event> EventHistoryCollector::ReadPreviousEvents(int maxCount)
{
	vw1__ReadPreviousEventsRequestType req;
	req._USCOREthis = _mor;
	req.maxCount = maxCount;

	// call_defs ReadPreviousEvents to post the request to ESX server or virtual center
	_vw1__ReadPreviousEventsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReadPreviousEvents))
	{
		std::vector<vw1__Event> vw1__events;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__Event tmp(*rsp.returnval[i]);
			vw1__events.push_back(tmp);
		}
		return vw1__events;
	}

	throw get_last_error();
}
std::vector<vw1__Event> EventHistoryCollector::get_latestPage() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("latestPage", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__Event>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfEvent* amo = dynamic_cast<vw1__ArrayOfEvent*>(it->val);
	std::vector<vw1__Event> ret;
	for (int i = 0; i < amo->__sizeEvent; ++i)
		ret.push_back(*amo->Event[i]);
	return ret;
}


const char* Folder::type = "Folder";
Folder::Folder(vw1__ManagedObjectReference* mor) : ManagedEntity(mor) {}
Folder::~Folder() {}

Task Folder::AddStandaloneHost_Task(vw1__HostConnectSpec*  spec, vw1__ComputeResourceConfigSpec*  compResSpec, bool addConnected, char* license)
{
	vw1__AddStandaloneHostRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;
	req.compResSpec = compResSpec;
	req.addConnected = addConnected;
	req.license = license;

	// call_defs AddStandaloneHost_USCORETask to post the request to ESX server or virtual center
	_vw1__AddStandaloneHost_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AddStandaloneHost_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

ClusterComputeResource Folder::CreateCluster(char* name, vw1__ClusterConfigSpec*  spec)
{
	vw1__CreateClusterRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.spec = spec;

	// call_defs CreateCluster to post the request to ESX server or virtual center
	_vw1__CreateClusterResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateCluster))
	{
		return ClusterComputeResource(rsp.returnval);
	}

	throw get_last_error();
}

ClusterComputeResource Folder::CreateClusterEx(char* name, vw1__ClusterConfigSpecEx*  spec)
{
	vw1__CreateClusterExRequestType req;
	req._USCOREthis = _mor;
	req.name = name;
	req.spec = spec;

	// call_defs CreateClusterEx to post the request to ESX server or virtual center
	_vw1__CreateClusterExResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateClusterEx))
	{
		return ClusterComputeResource(rsp.returnval);
	}

	throw get_last_error();
}

Datacenter Folder::CreateDatacenter(char* name)
{
	vw1__CreateDatacenterRequestType req;
	req._USCOREthis = _mor;
	req.name = name;

	// call_defs CreateDatacenter to post the request to ESX server or virtual center
	_vw1__CreateDatacenterResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateDatacenter))
	{
		return Datacenter(rsp.returnval);
	}

	throw get_last_error();
}

Task Folder::CreateDVS_Task(vw1__DVSCreateSpec*  spec)
{
	vw1__CreateDVSRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;

	// call_defs CreateDVS_USCORETask to post the request to ESX server or virtual center
	_vw1__CreateDVS_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateDVS_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Folder Folder::CreateFolder(char* name)
{
	vw1__CreateFolderRequestType req;
	req._USCOREthis = _mor;
	req.name = name;

	// call_defs CreateFolder to post the request to ESX server or virtual center
	_vw1__CreateFolderResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateFolder))
	{
		return Folder(rsp.returnval);
	}

	throw get_last_error();
}

StoragePod Folder::CreateStoragePod(char* name)
{
	vw1__CreateStoragePodRequestType req;
	req._USCOREthis = _mor;
	req.name = name;

	// call_defs CreateStoragePod to post the request to ESX server or virtual center
	_vw1__CreateStoragePodResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateStoragePod))
	{
		return StoragePod(rsp.returnval);
	}

	throw get_last_error();
}

Task Folder::CreateVM_Task(vw1__VirtualMachineConfigSpec*  config, ResourcePool pool, HostSystem host)
{
	vw1__CreateVMRequestType req;
	req._USCOREthis = _mor;
	req.config = config;
	req.pool = pool;
	req.host = host;

	// call_defs CreateVM_USCORETask to post the request to ESX server or virtual center
	_vw1__CreateVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task Folder::MoveIntoFolder_Task(int sizelist, ManagedEntity* list)
{
	vw1__MoveIntoFolderRequestType req;
	req._USCOREthis = _mor;
	req.__sizelist = sizelist;
	vw1__ManagedObjectReference ** plist = new vw1__ManagedObjectReference *[sizelist];
	for (int i = 0; i < sizelist; ++i)
		plist[i] = list[i];
	req.list = plist;

	// call_defs MoveIntoFolder_USCORETask to post the request to ESX server or virtual center
	_vw1__MoveIntoFolder_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MoveIntoFolder_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task Folder::RegisterVM_Task(char* path, char* name, bool asTemplate, ResourcePool pool, HostSystem host)
{
	vw1__RegisterVMRequestType req;
	req._USCOREthis = _mor;
	req.path = path;
	req.name = name;
	req.asTemplate = asTemplate;
	req.pool = pool;
	req.host = host;

	// call_defs RegisterVM_USCORETask to post the request to ESX server or virtual center
	_vw1__RegisterVM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RegisterVM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task Folder::UnregisterAndDestroy_Task()
{
	vw1__UnregisterAndDestroyRequestType req;
	req._USCOREthis = _mor;

	// call_defs UnregisterAndDestroy_USCORETask to post the request to ESX server or virtual center
	_vw1__UnregisterAndDestroy_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UnregisterAndDestroy_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}
std::vector<ManagedEntity> Folder::get_childEntity() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("childEntity", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<ManagedEntity>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<ManagedEntity> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(ManagedEntity(amo->ManagedObjectReference[i]));
	return ret;
}

std::vector<std::string> Folder::get_childType() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("childType", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<std::string>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfString* amo = dynamic_cast<vw1__ArrayOfString*>(it->val);
	std::vector<std::string> ret;
	for (int i = 0; i < amo->__sizestring; ++i)
		ret.push_back(amo->string[i]);
	return ret;
}


const char* HostActiveDirectoryAuthentication::type = "HostActiveDirectoryAuthentication";
HostActiveDirectoryAuthentication::HostActiveDirectoryAuthentication(vw1__ManagedObjectReference* mor) : HostDirectoryStore(mor) {}
HostActiveDirectoryAuthentication::~HostActiveDirectoryAuthentication() {}

Task HostActiveDirectoryAuthentication::ImportCertificateForCAM_Task(char* certPath, char* camServer)
{
	vw1__ImportCertificateForCAMRequestType req;
	req._USCOREthis = _mor;
	req.certPath = certPath;
	req.camServer = camServer;

	// call_defs ImportCertificateForCAM_USCORETask to post the request to ESX server or virtual center
	_vw1__ImportCertificateForCAM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ImportCertificateForCAM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task HostActiveDirectoryAuthentication::JoinDomain_Task(char* domainName, char* userName, char* password)
{
	vw1__JoinDomainRequestType req;
	req._USCOREthis = _mor;
	req.domainName = domainName;
	req.userName = userName;
	req.password = password;

	// call_defs JoinDomain_USCORETask to post the request to ESX server or virtual center
	_vw1__JoinDomain_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::JoinDomain_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task HostActiveDirectoryAuthentication::JoinDomainWithCAM_Task(char* domainName, char* camServer)
{
	vw1__JoinDomainWithCAMRequestType req;
	req._USCOREthis = _mor;
	req.domainName = domainName;
	req.camServer = camServer;

	// call_defs JoinDomainWithCAM_USCORETask to post the request to ESX server or virtual center
	_vw1__JoinDomainWithCAM_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::JoinDomainWithCAM_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task HostActiveDirectoryAuthentication::LeaveCurrentDomain_Task(bool force)
{
	vw1__LeaveCurrentDomainRequestType req;
	req._USCOREthis = _mor;
	req.force = force;

	// call_defs LeaveCurrentDomain_USCORETask to post the request to ESX server or virtual center
	_vw1__LeaveCurrentDomain_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::LeaveCurrentDomain_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

const char* HostProfile::type = "HostProfile";
HostProfile::HostProfile(vw1__ManagedObjectReference* mor) : Profile(mor) {}
HostProfile::~HostProfile() {}

vw1__ProfileExecuteResult HostProfile::ExecuteHostProfile(HostSystem host, int sizedeferredParam, vw1__ProfileDeferredPolicyOptionParameter**  deferredParam)
{
	vw1__ExecuteHostProfileRequestType req;
	req._USCOREthis = _mor;
	req.host = host;
	req.__sizedeferredParam = sizedeferredParam;
	req.deferredParam = deferredParam;

	// call_defs ExecuteHostProfile to post the request to ESX server or virtual center
	_vw1__ExecuteHostProfileResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ExecuteHostProfile))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

void HostProfile::UpdateHostProfile(vw1__HostProfileConfigSpec*  config)
{
	vw1__UpdateHostProfileRequestType req;
	req._USCOREthis = _mor;
	req.config = config;

	// call_defs UpdateHostProfile to post the request to ESX server or virtual center
	_vw1__UpdateHostProfileResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateHostProfile))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostProfile::UpdateReferenceHost(HostSystem host)
{
	vw1__UpdateReferenceHostRequestType req;
	req._USCOREthis = _mor;
	req.host = host;

	// call_defs UpdateReferenceHost to post the request to ESX server or virtual center
	_vw1__UpdateReferenceHostResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateReferenceHost))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
HostSystem HostProfile::get_referenceHost() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("referenceHost", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return HostSystem();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return HostSystem(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}


const char* HostProfileManager::type = "HostProfileManager";
HostProfileManager::HostProfileManager(vw1__ManagedObjectReference* mor) : ProfileManager(mor) {}
HostProfileManager::~HostProfileManager() {}

Task HostProfileManager::ApplyHostConfig_Task(HostSystem host, vw1__HostConfigSpec*  configSpec, int sizeuserInput, vw1__ProfileDeferredPolicyOptionParameter**  userInput)
{
	vw1__ApplyHostConfigRequestType req;
	req._USCOREthis = _mor;
	req.host = host;
	req.configSpec = configSpec;
	req.__sizeuserInput = sizeuserInput;
	req.userInput = userInput;

	// call_defs ApplyHostConfig_USCORETask to post the request to ESX server or virtual center
	_vw1__ApplyHostConfig_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ApplyHostConfig_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task HostProfileManager::CheckAnswerFileStatus_Task(int sizehost, HostSystem* host)
{
	vw1__CheckAnswerFileStatusRequestType req;
	req._USCOREthis = _mor;
	req.__sizehost = sizehost;
	vw1__ManagedObjectReference ** phost = new vw1__ManagedObjectReference *[sizehost];
	for (int i = 0; i < sizehost; ++i)
		phost[i] = host[i];
	req.host = phost;

	// call_defs CheckAnswerFileStatus_USCORETask to post the request to ESX server or virtual center
	_vw1__CheckAnswerFileStatus_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CheckAnswerFileStatus_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

vw1__ApplyProfile HostProfileManager::CreateDefaultProfile(char* profileType, char* profileTypeName, Profile profile)
{
	vw1__CreateDefaultProfileRequestType req;
	req._USCOREthis = _mor;
	req.profileType = profileType;
	req.profileTypeName = profileTypeName;
	req.profile = profile;

	// call_defs CreateDefaultProfile to post the request to ESX server or virtual center
	_vw1__CreateDefaultProfileResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CreateDefaultProfile))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

Task HostProfileManager::ExportAnswerFile_Task(HostSystem host)
{
	vw1__ExportAnswerFileRequestType req;
	req._USCOREthis = _mor;
	req.host = host;

	// call_defs ExportAnswerFile_USCORETask to post the request to ESX server or virtual center
	_vw1__ExportAnswerFile_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ExportAnswerFile_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

vw1__HostProfileManagerConfigTaskList HostProfileManager::GenerateConfigTaskList(vw1__HostConfigSpec*  configSpec, HostSystem host)
{
	vw1__GenerateConfigTaskListRequestType req;
	req._USCOREthis = _mor;
	req.configSpec = configSpec;
	req.host = host;

	// call_defs GenerateConfigTaskList to post the request to ESX server or virtual center
	_vw1__GenerateConfigTaskListResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::GenerateConfigTaskList))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

std::vector<vw1__AnswerFileStatusResult> HostProfileManager::QueryAnswerFileStatus(int sizehost, HostSystem* host)
{
	vw1__QueryAnswerFileStatusRequestType req;
	req._USCOREthis = _mor;
	req.__sizehost = sizehost;
	vw1__ManagedObjectReference ** phost = new vw1__ManagedObjectReference *[sizehost];
	for (int i = 0; i < sizehost; ++i)
		phost[i] = host[i];
	req.host = phost;

	// call_defs QueryAnswerFileStatus to post the request to ESX server or virtual center
	_vw1__QueryAnswerFileStatusResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryAnswerFileStatus))
	{
		std::vector<vw1__AnswerFileStatusResult> vw1__answerfilestatusresults;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__AnswerFileStatusResult tmp(*rsp.returnval[i]);
			vw1__answerfilestatusresults.push_back(tmp);
		}
		return vw1__answerfilestatusresults;
	}

	throw get_last_error();
}

std::vector<vw1__ProfileMetadata> HostProfileManager::QueryHostProfileMetadata(int sizeprofileName, char** profileName, Profile profile)
{
	vw1__QueryHostProfileMetadataRequestType req;
	req._USCOREthis = _mor;
	req.__sizeprofileName = sizeprofileName;
	req.profileName = profileName;
	req.profile = profile;

	// call_defs QueryHostProfileMetadata to post the request to ESX server or virtual center
	_vw1__QueryHostProfileMetadataResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryHostProfileMetadata))
	{
		std::vector<vw1__ProfileMetadata> vw1__profilemetadatas;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__ProfileMetadata tmp(*rsp.returnval[i]);
			vw1__profilemetadatas.push_back(tmp);
		}
		return vw1__profilemetadatas;
	}

	throw get_last_error();
}

vw1__ProfileProfileStructure HostProfileManager::QueryProfileStructure(Profile profile)
{
	vw1__QueryProfileStructureRequestType req;
	req._USCOREthis = _mor;
	req.profile = profile;

	// call_defs QueryProfileStructure to post the request to ESX server or virtual center
	_vw1__QueryProfileStructureResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryProfileStructure))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

vw1__AnswerFile HostProfileManager::RetrieveAnswerFile(HostSystem host)
{
	vw1__RetrieveAnswerFileRequestType req;
	req._USCOREthis = _mor;
	req.host = host;

	// call_defs RetrieveAnswerFile to post the request to ESX server or virtual center
	_vw1__RetrieveAnswerFileResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RetrieveAnswerFile))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

Task HostProfileManager::UpdateAnswerFile_Task(HostSystem host, vw1__AnswerFileCreateSpec*  configSpec)
{
	vw1__UpdateAnswerFileRequestType req;
	req._USCOREthis = _mor;
	req.host = host;
	req.configSpec = configSpec;

	// call_defs UpdateAnswerFile_USCORETask to post the request to ESX server or virtual center
	_vw1__UpdateAnswerFile_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateAnswerFile_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

const char* HostSystem::type = "HostSystem";
HostSystem::HostSystem(vw1__ManagedObjectReference* mor) : ManagedEntity(mor) {}
HostSystem::~HostSystem() {}

vw1__HostServiceTicket HostSystem::AcquireCimServicesTicket()
{
	vw1__AcquireCimServicesTicketRequestType req;
	req._USCOREthis = _mor;

	// call_defs AcquireCimServicesTicket to post the request to ESX server or virtual center
	_vw1__AcquireCimServicesTicketResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AcquireCimServicesTicket))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

Task HostSystem::DisconnectHost_Task()
{
	vw1__DisconnectHostRequestType req;
	req._USCOREthis = _mor;

	// call_defs DisconnectHost_USCORETask to post the request to ESX server or virtual center
	_vw1__DisconnectHost_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::DisconnectHost_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void HostSystem::EnterLockdownMode()
{
	vw1__EnterLockdownModeRequestType req;
	req._USCOREthis = _mor;

	// call_defs EnterLockdownMode to post the request to ESX server or virtual center
	_vw1__EnterLockdownModeResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::EnterLockdownMode))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task HostSystem::EnterMaintenanceMode_Task(int timeout, bool*  evacuatePoweredOffVms)
{
	vw1__EnterMaintenanceModeRequestType req;
	req._USCOREthis = _mor;
	req.timeout = timeout;
	req.evacuatePoweredOffVms = evacuatePoweredOffVms;

	// call_defs EnterMaintenanceMode_USCORETask to post the request to ESX server or virtual center
	_vw1__EnterMaintenanceMode_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::EnterMaintenanceMode_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void HostSystem::ExitLockdownMode()
{
	vw1__ExitLockdownModeRequestType req;
	req._USCOREthis = _mor;

	// call_defs ExitLockdownMode to post the request to ESX server or virtual center
	_vw1__ExitLockdownModeResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ExitLockdownMode))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

Task HostSystem::ExitMaintenanceMode_Task(int timeout)
{
	vw1__ExitMaintenanceModeRequestType req;
	req._USCOREthis = _mor;
	req.timeout = timeout;

	// call_defs ExitMaintenanceMode_USCORETask to post the request to ESX server or virtual center
	_vw1__ExitMaintenanceMode_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ExitMaintenanceMode_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task HostSystem::PowerDownHostToStandBy_Task(int timeoutSec, bool*  evacuatePoweredOffVms)
{
	vw1__PowerDownHostToStandByRequestType req;
	req._USCOREthis = _mor;
	req.timeoutSec = timeoutSec;
	req.evacuatePoweredOffVms = evacuatePoweredOffVms;

	// call_defs PowerDownHostToStandBy_USCORETask to post the request to ESX server or virtual center
	_vw1__PowerDownHostToStandBy_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::PowerDownHostToStandBy_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task HostSystem::PowerUpHostFromStandBy_Task(int timeoutSec)
{
	vw1__PowerUpHostFromStandByRequestType req;
	req._USCOREthis = _mor;
	req.timeoutSec = timeoutSec;

	// call_defs PowerUpHostFromStandBy_USCORETask to post the request to ESX server or virtual center
	_vw1__PowerUpHostFromStandBy_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::PowerUpHostFromStandBy_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

vw1__HostConnectInfo HostSystem::QueryHostConnectionInfo()
{
	vw1__QueryHostConnectionInfoRequestType req;
	req._USCOREthis = _mor;

	// call_defs QueryHostConnectionInfo to post the request to ESX server or virtual center
	_vw1__QueryHostConnectionInfoResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryHostConnectionInfo))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

__int64 HostSystem::QueryMemoryOverhead(__int64 memorySize, int*  videoRamSize, int numVcpus)
{
	vw1__QueryMemoryOverheadRequestType req;
	req._USCOREthis = _mor;
	req.memorySize = memorySize;
	req.videoRamSize = videoRamSize;
	req.numVcpus = numVcpus;

	// call_defs QueryMemoryOverhead to post the request to ESX server or virtual center
	_vw1__QueryMemoryOverheadResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryMemoryOverhead))
	{
		return static_cast<__int64>(rsp.returnval);
	}

	throw get_last_error();
}

__int64 HostSystem::QueryMemoryOverheadEx(vw1__VirtualMachineConfigInfo*  vmConfigInfo)
{
	vw1__QueryMemoryOverheadExRequestType req;
	req._USCOREthis = _mor;
	req.vmConfigInfo = vmConfigInfo;

	// call_defs QueryMemoryOverheadEx to post the request to ESX server or virtual center
	_vw1__QueryMemoryOverheadExResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::QueryMemoryOverheadEx))
	{
		return static_cast<__int64>(rsp.returnval);
	}

	throw get_last_error();
}

Task HostSystem::RebootHost_Task(bool force)
{
	vw1__RebootHostRequestType req;
	req._USCOREthis = _mor;
	req.force = force;

	// call_defs RebootHost_USCORETask to post the request to ESX server or virtual center
	_vw1__RebootHost_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RebootHost_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task HostSystem::ReconfigureHostForDAS_Task()
{
	vw1__ReconfigureHostForDASRequestType req;
	req._USCOREthis = _mor;

	// call_defs ReconfigureHostForDAS_USCORETask to post the request to ESX server or virtual center
	_vw1__ReconfigureHostForDAS_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReconfigureHostForDAS_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task HostSystem::ReconnectHost_Task(vw1__HostConnectSpec*  cnxSpec, vw1__HostSystemReconnectSpec*  reconnectSpec)
{
	vw1__ReconnectHostRequestType req;
	req._USCOREthis = _mor;
	req.cnxSpec = cnxSpec;
	req.reconnectSpec = reconnectSpec;

	// call_defs ReconnectHost_USCORETask to post the request to ESX server or virtual center
	_vw1__ReconnectHost_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReconnectHost_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

__int64 HostSystem::RetrieveHardwareUptime()
{
	vw1__RetrieveHardwareUptimeRequestType req;
	req._USCOREthis = _mor;

	// call_defs RetrieveHardwareUptime to post the request to ESX server or virtual center
	_vw1__RetrieveHardwareUptimeResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RetrieveHardwareUptime))
	{
		return static_cast<__int64>(rsp.returnval);
	}

	throw get_last_error();
}

Task HostSystem::ShutdownHost_Task(bool force)
{
	vw1__ShutdownHostRequestType req;
	req._USCOREthis = _mor;
	req.force = force;

	// call_defs ShutdownHost_USCORETask to post the request to ESX server or virtual center
	_vw1__ShutdownHost_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ShutdownHost_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void HostSystem::UpdateFlags(vw1__HostFlagInfo*  flagInfo)
{
	vw1__UpdateFlagsRequestType req;
	req._USCOREthis = _mor;
	req.flagInfo = flagInfo;

	// call_defs UpdateFlags to post the request to ESX server or virtual center
	_vw1__UpdateFlagsResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateFlags))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostSystem::UpdateIpmi(vw1__HostIpmiInfo*  ipmiInfo)
{
	vw1__UpdateIpmiRequestType req;
	req._USCOREthis = _mor;
	req.ipmiInfo = ipmiInfo;

	// call_defs UpdateIpmi to post the request to ESX server or virtual center
	_vw1__UpdateIpmiResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateIpmi))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void HostSystem::UpdateSystemResources(vw1__HostSystemResourceInfo*  resourceInfo)
{
	vw1__UpdateSystemResourcesRequestType req;
	req._USCOREthis = _mor;
	req.resourceInfo = resourceInfo;

	// call_defs UpdateSystemResources to post the request to ESX server or virtual center
	_vw1__UpdateSystemResourcesResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::UpdateSystemResources))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}
vw1__HostCapability HostSystem::get_capability() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("capability", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostCapability();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostCapability*>(it->val);
}

vw1__HostConfigInfo HostSystem::get_config() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("config", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostConfigInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostConfigInfo*>(it->val);
}

vw1__HostConfigManager HostSystem::get_configManager() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("configManager", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostConfigManager();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostConfigManager*>(it->val);
}

std::vector<Datastore> HostSystem::get_datastore() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("datastore", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<Datastore>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<Datastore> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(Datastore(amo->ManagedObjectReference[i]));
	return ret;
}

HostDatastoreBrowser HostSystem::get_datastoreBrowser() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("datastoreBrowser", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return HostDatastoreBrowser();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return HostDatastoreBrowser(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}

vw1__HostHardwareInfo HostSystem::get_hardware() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("hardware", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostHardwareInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostHardwareInfo*>(it->val);
}

vw1__HostLicensableResourceInfo HostSystem::get_licensableResource() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("licensableResource", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostLicensableResourceInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostLicensableResourceInfo*>(it->val);
}

std::vector<Network> HostSystem::get_network() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("network", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<Network>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<Network> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(Network(amo->ManagedObjectReference[i]));
	return ret;
}

vw1__HostRuntimeInfo HostSystem::get_runtime() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("runtime", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostRuntimeInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostRuntimeInfo*>(it->val);
}

vw1__HostListSummary HostSystem::get_summary() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("summary", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostListSummary();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostListSummary*>(it->val);
}

vw1__HostSystemResourceInfo HostSystem::get_systemResources() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("systemResources", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__HostSystemResourceInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__HostSystemResourceInfo*>(it->val);
}

std::vector<VirtualMachine> HostSystem::get_vm() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("vm", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<VirtualMachine>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<VirtualMachine> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(VirtualMachine(amo->ManagedObjectReference[i]));
	return ret;
}


const char* ManagedObjectView::type = "ManagedObjectView";
ManagedObjectView::ManagedObjectView(vw1__ManagedObjectReference* mor) : View(mor) {}
ManagedObjectView::~ManagedObjectView() {}
std::vector<View> ManagedObjectView::get_view() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("view", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<View>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfManagedObjectReference* amo = dynamic_cast<vw1__ArrayOfManagedObjectReference*>(it->val);
	std::vector<View> ret;
	for (int i = 0; i < amo->__sizeManagedObjectReference; ++i)
		ret.push_back(View(amo->ManagedObjectReference[i]));
	return ret;
}


const char* StoragePod::type = "StoragePod";
StoragePod::StoragePod(vw1__ManagedObjectReference* mor) : Folder(mor) {}
StoragePod::~StoragePod() {}
vw1__PodStorageDrsEntry StoragePod::get_podStorageDrsEntry() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("podStorageDrsEntry", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__PodStorageDrsEntry();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__PodStorageDrsEntry*>(it->val);
}

vw1__StoragePodSummary StoragePod::get_summary() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("summary", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__StoragePodSummary();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__StoragePodSummary*>(it->val);
}


const char* VmwareDistributedVirtualSwitch::type = "VmwareDistributedVirtualSwitch";
VmwareDistributedVirtualSwitch::VmwareDistributedVirtualSwitch(vw1__ManagedObjectReference* mor) : DistributedVirtualSwitch(mor) {}
VmwareDistributedVirtualSwitch::~VmwareDistributedVirtualSwitch() {}

const char* ClusterComputeResource::type = "ClusterComputeResource";
ClusterComputeResource::ClusterComputeResource(vw1__ManagedObjectReference* mor) : ComputeResource(mor) {}
ClusterComputeResource::~ClusterComputeResource() {}

Task ClusterComputeResource::AddHost_Task(vw1__HostConnectSpec*  spec, bool asConnected, ResourcePool resourcePool, char* license)
{
	vw1__AddHostRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;
	req.asConnected = asConnected;
	req.resourcePool = resourcePool;
	req.license = license;

	// call_defs AddHost_USCORETask to post the request to ESX server or virtual center
	_vw1__AddHost_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::AddHost_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void ClusterComputeResource::ApplyRecommendation(char* key)
{
	vw1__ApplyRecommendationRequestType req;
	req._USCOREthis = _mor;
	req.key = key;

	// call_defs ApplyRecommendation to post the request to ESX server or virtual center
	_vw1__ApplyRecommendationResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ApplyRecommendation))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

void ClusterComputeResource::CancelRecommendation(char* key)
{
	vw1__CancelRecommendationRequestType req;
	req._USCOREthis = _mor;
	req.key = key;

	// call_defs CancelRecommendation to post the request to ESX server or virtual center
	_vw1__CancelRecommendationResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CancelRecommendation))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

vw1__ClusterEnterMaintenanceResult ClusterComputeResource::ClusterEnterMaintenanceMode(int sizehost, HostSystem* host, int sizeoption, vw1__OptionValue**  option)
{
	vw1__ClusterEnterMaintenanceModeRequestType req;
	req._USCOREthis = _mor;
	req.__sizehost = sizehost;
	vw1__ManagedObjectReference ** phost = new vw1__ManagedObjectReference *[sizehost];
	for (int i = 0; i < sizehost; ++i)
		phost[i] = host[i];
	req.host = phost;
	req.__sizeoption = sizeoption;
	req.option = option;

	// call_defs ClusterEnterMaintenanceMode to post the request to ESX server or virtual center
	_vw1__ClusterEnterMaintenanceModeResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ClusterEnterMaintenanceMode))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}

Task ClusterComputeResource::MoveHostInto_Task(HostSystem host, ResourcePool resourcePool)
{
	vw1__MoveHostIntoRequestType req;
	req._USCOREthis = _mor;
	req.host = host;
	req.resourcePool = resourcePool;

	// call_defs MoveHostInto_USCORETask to post the request to ESX server or virtual center
	_vw1__MoveHostInto_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MoveHostInto_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

Task ClusterComputeResource::MoveInto_Task(int sizehost, HostSystem* host)
{
	vw1__MoveIntoRequestType req;
	req._USCOREthis = _mor;
	req.__sizehost = sizehost;
	vw1__ManagedObjectReference ** phost = new vw1__ManagedObjectReference *[sizehost];
	for (int i = 0; i < sizehost; ++i)
		phost[i] = host[i];
	req.host = phost;

	// call_defs MoveInto_USCORETask to post the request to ESX server or virtual center
	_vw1__MoveInto_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::MoveInto_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

std::vector<vw1__ClusterHostRecommendation> ClusterComputeResource::RecommendHostsForVm(VirtualMachine vm, ResourcePool pool)
{
	vw1__RecommendHostsForVmRequestType req;
	req._USCOREthis = _mor;
	req.vm = vm;
	req.pool = pool;

	// call_defs RecommendHostsForVm to post the request to ESX server or virtual center
	_vw1__RecommendHostsForVmResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RecommendHostsForVm))
	{
		std::vector<vw1__ClusterHostRecommendation> vw1__clusterhostrecommendations;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__ClusterHostRecommendation tmp(*rsp.returnval[i]);
			vw1__clusterhostrecommendations.push_back(tmp);
		}
		return vw1__clusterhostrecommendations;
	}

	throw get_last_error();
}

Task ClusterComputeResource::ReconfigureCluster_Task(vw1__ClusterConfigSpec*  spec, bool modify)
{
	vw1__ReconfigureClusterRequestType req;
	req._USCOREthis = _mor;
	req.spec = spec;
	req.modify = modify;

	// call_defs ReconfigureCluster_USCORETask to post the request to ESX server or virtual center
	_vw1__ReconfigureCluster_USCORETaskResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ReconfigureCluster_USCORETask))
	{
		return Task(rsp.returnval);
	}

	throw get_last_error();
}

void ClusterComputeResource::RefreshRecommendation()
{
	vw1__RefreshRecommendationRequestType req;
	req._USCOREthis = _mor;

	// call_defs RefreshRecommendation to post the request to ESX server or virtual center
	_vw1__RefreshRecommendationResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RefreshRecommendation))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

vw1__ClusterDasAdvancedRuntimeInfo ClusterComputeResource::RetrieveDasAdvancedRuntimeInfo()
{
	vw1__RetrieveDasAdvancedRuntimeInfoRequestType req;
	req._USCOREthis = _mor;

	// call_defs RetrieveDasAdvancedRuntimeInfo to post the request to ESX server or virtual center
	_vw1__RetrieveDasAdvancedRuntimeInfoResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::RetrieveDasAdvancedRuntimeInfo))
	{
		return *rsp.returnval;
	}

	throw get_last_error();
}
std::vector<vw1__ClusterActionHistory> ClusterComputeResource::get_actionHistory() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("actionHistory", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__ClusterActionHistory>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfClusterActionHistory* amo = dynamic_cast<vw1__ArrayOfClusterActionHistory*>(it->val);
	std::vector<vw1__ClusterActionHistory> ret;
	for (int i = 0; i < amo->__sizeClusterActionHistory; ++i)
		ret.push_back(*amo->ClusterActionHistory[i]);
	return ret;
}

vw1__ClusterConfigInfo ClusterComputeResource::get_configuration() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("configuration", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return vw1__ClusterConfigInfo();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return *dynamic_cast<vw1__ClusterConfigInfo*>(it->val);
}

std::vector<vw1__ClusterDrsFaults> ClusterComputeResource::get_drsFault() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("drsFault", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__ClusterDrsFaults>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfClusterDrsFaults* amo = dynamic_cast<vw1__ArrayOfClusterDrsFaults*>(it->val);
	std::vector<vw1__ClusterDrsFaults> ret;
	for (int i = 0; i < amo->__sizeClusterDrsFaults; ++i)
		ret.push_back(*amo->ClusterDrsFaults[i]);
	return ret;
}

std::vector<vw1__ClusterDrsRecommendation> ClusterComputeResource::get_drsRecommendation() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("drsRecommendation", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__ClusterDrsRecommendation>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfClusterDrsRecommendation* amo = dynamic_cast<vw1__ArrayOfClusterDrsRecommendation*>(it->val);
	std::vector<vw1__ClusterDrsRecommendation> ret;
	for (int i = 0; i < amo->__sizeClusterDrsRecommendation; ++i)
		ret.push_back(*amo->ClusterDrsRecommendation[i]);
	return ret;
}

std::vector<vw1__ClusterDrsMigration> ClusterComputeResource::get_migrationHistory() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("migrationHistory", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__ClusterDrsMigration>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfClusterDrsMigration* amo = dynamic_cast<vw1__ArrayOfClusterDrsMigration*>(it->val);
	std::vector<vw1__ClusterDrsMigration> ret;
	for (int i = 0; i < amo->__sizeClusterDrsMigration; ++i)
		ret.push_back(*amo->ClusterDrsMigration[i]);
	return ret;
}

std::vector<vw1__ClusterRecommendation> ClusterComputeResource::get_recommendation() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("recommendation", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<vw1__ClusterRecommendation>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfClusterRecommendation* amo = dynamic_cast<vw1__ArrayOfClusterRecommendation*>(it->val);
	std::vector<vw1__ClusterRecommendation> ret;
	for (int i = 0; i < amo->__sizeClusterRecommendation; ++i)
		ret.push_back(*amo->ClusterRecommendation[i]);
	return ret;
}


const char* ContainerView::type = "ContainerView";
ContainerView::ContainerView(vw1__ManagedObjectReference* mor) : ManagedObjectView(mor) {}
ContainerView::~ContainerView() {}
ManagedEntity ContainerView::get_container() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("container", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return ManagedEntity();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return ManagedEntity(dynamic_cast<vw1__ManagedObjectReference *>(it->val));
}

bool ContainerView::get_recursive() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("recursive", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return bool();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	return dynamic_cast<xsd__boolean*>(it->val)->__item;
}

std::vector<std::string> ContainerView::get_type() const
{
	std::vector<vw1__ObjectContent> ocs = get_property("type", this);
	if (ocs.size() != 1)
		throw get_last_error();
	if (ocs[0].__sizepropSet == 0) // empty result, not set yet
	{
//		assert(ocs[0].__sizemissingSet == 1);
//		ns1__MissingProperty* miss = ocs[0].missingSet[0];
//		if (miss->fault->fault)
//			throw get_fault_msg(miss->fault->fault);
//		else
			return std::vector<std::string>();
	}

	vw1__DynamicProperty* it = ocs[0].propSet[0];
	vw1__ArrayOfString* amo = dynamic_cast<vw1__ArrayOfString*>(it->val);
	std::vector<std::string> ret;
	for (int i = 0; i < amo->__sizestring; ++i)
		ret.push_back(amo->string[i]);
	return ret;
}


const char* InventoryView::type = "InventoryView";
InventoryView::InventoryView(vw1__ManagedObjectReference* mor) : ManagedObjectView(mor) {}
InventoryView::~InventoryView() {}

std::vector<ManagedEntity> InventoryView::CloseInventoryViewFolder(int sizeentity, ManagedEntity* entity)
{
	vw1__CloseInventoryViewFolderRequestType req;
	req._USCOREthis = _mor;
	req.__sizeentity = sizeentity;
	vw1__ManagedObjectReference ** pentity = new vw1__ManagedObjectReference *[sizeentity];
	for (int i = 0; i < sizeentity; ++i)
		pentity[i] = entity[i];
	req.entity = pentity;

	// call_defs CloseInventoryViewFolder to post the request to ESX server or virtual center
	_vw1__CloseInventoryViewFolderResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::CloseInventoryViewFolder))
	{
		std::vector<ManagedEntity> managedentitys;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			ManagedEntity tmp(rsp.returnval[i]);
			managedentitys.push_back(tmp);
		}
		return managedentitys;
	}

	throw get_last_error();
}

std::vector<ManagedEntity> InventoryView::OpenInventoryViewFolder(int sizeentity, ManagedEntity* entity)
{
	vw1__OpenInventoryViewFolderRequestType req;
	req._USCOREthis = _mor;
	req.__sizeentity = sizeentity;
	vw1__ManagedObjectReference ** pentity = new vw1__ManagedObjectReference *[sizeentity];
	for (int i = 0; i < sizeentity; ++i)
		pentity[i] = entity[i];
	req.entity = pentity;

	// call_defs OpenInventoryViewFolder to post the request to ESX server or virtual center
	_vw1__OpenInventoryViewFolderResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::OpenInventoryViewFolder))
	{
		std::vector<ManagedEntity> managedentitys;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			ManagedEntity tmp(rsp.returnval[i]);
			managedentitys.push_back(tmp);
		}
		return managedentitys;
	}

	throw get_last_error();
}

const char* ListView::type = "ListView";
ListView::ListView(vw1__ManagedObjectReference* mor) : ManagedObjectView(mor) {}
ListView::~ListView() {}

std::vector<vw1__ManagedObjectReference> ListView::ModifyListView(int sizeadd, vw1__ManagedObjectReference**  add, int sizeremove, vw1__ManagedObjectReference**  remove)
{
	vw1__ModifyListViewRequestType req;
	req._USCOREthis = _mor;
	req.__sizeadd = sizeadd;
	vw1__ManagedObjectReference ** padd = new vw1__ManagedObjectReference *[sizeadd];
	for (int i = 0; i < sizeadd; ++i)
		padd[i] = add[i];
	req.add = padd;
	req.__sizeremove = sizeremove;
	vw1__ManagedObjectReference ** premove = new vw1__ManagedObjectReference *[sizeremove];
	for (int i = 0; i < sizeremove; ++i)
		premove[i] = remove[i];
	req.remove = premove;

	// call_defs ModifyListView to post the request to ESX server or virtual center
	_vw1__ModifyListViewResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ModifyListView))
	{
		std::vector<vw1__ManagedObjectReference> vw1__managedobjectreferences;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__ManagedObjectReference tmp(*rsp.returnval[i]);
			vw1__managedobjectreferences.push_back(tmp);
		}
		return vw1__managedobjectreferences;
	}

	throw get_last_error();
}

std::vector<vw1__ManagedObjectReference> ListView::ResetListView(int sizeobj, vw1__ManagedObjectReference**  obj)
{
	vw1__ResetListViewRequestType req;
	req._USCOREthis = _mor;
	req.__sizeobj = sizeobj;
	vw1__ManagedObjectReference ** pobj = new vw1__ManagedObjectReference *[sizeobj];
	for (int i = 0; i < sizeobj; ++i)
		pobj[i] = obj[i];
	req.obj = pobj;

	// call_defs ResetListView to post the request to ESX server or virtual center
	_vw1__ResetListViewResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ResetListView))
	{
		std::vector<vw1__ManagedObjectReference> vw1__managedobjectreferences;
		for (int i = 0; i < rsp.__sizereturnval; ++i)
		{
			vw1__ManagedObjectReference tmp(*rsp.returnval[i]);
			vw1__managedobjectreferences.push_back(tmp);
		}
		return vw1__managedobjectreferences;
	}

	throw get_last_error();
}

void ListView::ResetListViewFromView(View view)
{
	vw1__ResetListViewFromViewRequestType req;
	req._USCOREthis = _mor;
	req.view = view;

	// call_defs ResetListViewFromView to post the request to ESX server or virtual center
	_vw1__ResetListViewFromViewResponse rsp;
	if (proxy_call_defs(req, rsp, *this, &VimBindingProxy::ResetListViewFromView))
	{
		// empty return value
		return;
	}

	throw get_last_error();
}

// The entry of ServiceInstance singleton
Vim& VimInstance(binding_proxy* proxy)
{
	static Vim v;
	if (proxy)
		v._proxy = proxy;
	return v;
}

PropertyCollector& get_collector()
{
	ServiceInstance& si = VimInstance().get_service_instance();
	vw1__ServiceContent sc = si.RetrieveServiceContent();
	static PropertyCollector pc(sc.propertyCollector);
	return pc;
}

std::vector<vw1__ObjectContent> get_property(const char* prop, const mor_handle* mo)
{
	// ObjectSpec specifies the starting object
	vw1__ObjectSpec os;
	os.obj = *mo;
	vw1__ObjectSpec* oss[] = {&os};

	// PropertySpec specifies what properties to retrieve and from type of Managed Object
	char* pps[1]; pps[0] = const_cast<char*>(prop);
	vw1__PropertySpec ps;
	ps.type = os.obj->type;
	ps.__sizepathSet = 1;
	ps.pathSet = pps;
	vw1__PropertySpec* pss[] = {&ps};

	// PropertyFilterSpec is used to hold the ObjectSpec and PropertySpec for the call_defs
	vw1__PropertyFilterSpec pfs;
	pfs.__sizeobjectSet = 1;
	pfs.objectSet = oss;
	pfs.__sizepropSet = 1;
	pfs.propSet = pss;
	vw1__PropertyFilterSpec* pfSpec[] = {&pfs};

	vim::PropertyCollector pc = get_collector();
	return pc.RetrieveProperties(1, pfSpec);
}
} // namespace vim

