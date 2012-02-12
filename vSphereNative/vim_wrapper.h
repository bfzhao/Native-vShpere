////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Bingfeng Zhao . All rights reserved.
// 

#ifndef VIM_WRAPPER_H
#define VIM_WRAPPER_H

#include <string>
#include <vector>
#include "soapStub.h"				// gSOAP generated
#include "soapVimBindingProxy.h"	// gSOAP generated

namespace vim
{
// forward declarations
typedef xsd__anyType anyType;

// Generic helper to retrieve property of managed object
class mor_handle;
std::vector<vw1__ObjectContent> get_property(const char* prop, const mor_handle* mo);

// Class definitions
class binding_proxy
{
	binding_proxy(const binding_proxy&);
	binding_proxy& operator=(const binding_proxy&);

	VimBindingProxy* _binding_proxy;
	std::string _url;
public:
	binding_proxy(const char* ip, bool use_ssl);
	~binding_proxy();
	operator VimBindingProxy*() { return _binding_proxy; }
};

class mor_handle
{
protected:
	std::string	 _last_error;
	vw1__ManagedObjectReference* _mor;

public:
	explicit mor_handle(vw1__ManagedObjectReference* mor);
	virtual ~mor_handle();

	void set_last_error(const char* msg = 0) { _last_error = msg? msg : ""; }
	std::string get_last_error() const { return _last_error; }

	bool is_type_of(const char* type) const { return _strcmpi(_mor->type, type) == 0; }
	const char* present() const { return _mor->__item; }
	operator vw1__ManagedObjectReference*() const { return _mor; }
	operator vw1__ManagedObjectReference*() { return _mor; }
	operator bool() const { return _mor != 0; }
};

class Alarm;
class AlarmManager;
class AuthorizationManager;
class ClusterComputeResource;
class ClusterProfile;
class ClusterProfileManager;
class ComputeResource;
class ContainerView;
class CustomFieldsManager;
class CustomizationSpecManager;
class Datacenter;
class Datastore;
class DiagnosticManager;
class DistributedVirtualPortgroup;
class DistributedVirtualSwitch;
class DistributedVirtualSwitchManager;
class EnvironmentBrowser;
class EventHistoryCollector;
class EventManager;
class ExtensibleManagedObject;
class ExtensionManager;
class FileManager;
class Folder;
class GuestAuthManager;
class GuestFileManager;
class GuestOperationsManager;
class GuestProcessManager;
class HistoryCollector;
class HostActiveDirectoryAuthentication;
class HostAuthenticationManager;
class HostAuthenticationStore;
class HostAutoStartManager;
class HostBootDeviceSystem;
class HostCacheConfigurationManager;
class HostCpuSchedulerSystem;
class HostDatastoreBrowser;
class HostDatastoreSystem;
class HostDateTimeSystem;
class HostDiagnosticSystem;
class HostDirectoryStore;
class HostEsxAgentHostManager;
class HostFirewallSystem;
class HostFirmwareSystem;
class HostHealthStatusSystem;
class HostImageConfigManager;
class HostKernelModuleSystem;
class HostLocalAccountManager;
class HostLocalAuthentication;
class HostMemorySystem;
class HostNetworkSystem;
class HostPatchManager;
class HostPciPassthruSystem;
class HostPowerSystem;
class HostProfile;
class HostProfileManager;
class HostServiceSystem;
class HostSnmpSystem;
class HostStorageSystem;
class HostSystem;
class HostVMotionSystem;
class HostVirtualNicManager;
class HttpNfcLease;
class InventoryView;
class IpPoolManager;
class IscsiManager;
class LicenseAssignmentManager;
class LicenseManager;
class ListView;
class LocalizationManager;
class ManagedEntity;
class ManagedObjectView;
class Network;
class OptionManager;
class OvfManager;
class PerformanceManager;
class Profile;
class ProfileComplianceManager;
class ProfileManager;
class PropertyCollector;
class PropertyFilter;
class ResourcePlanningManager;
class ResourcePool;
class ScheduledTask;
class ScheduledTaskManager;
class SearchIndex;
class ServiceInstance;
class SessionManager;
class StoragePod;
class StorageResourceManager;
class Task;
class TaskHistoryCollector;
class TaskManager;
class UserDirectory;
class View;
class ViewManager;
class VirtualApp;
class VirtualDiskManager;
class VirtualMachine;
class VirtualMachineCompatibilityChecker;
class VirtualMachineProvisioningChecker;
class VirtualMachineSnapshot;
class VirtualizationManager;
class VmwareDistributedVirtualSwitch;


class AlarmManager : public mor_handle
{
public:
	AlarmManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~AlarmManager();

	static const char* type;
	void AcknowledgeAlarm(Alarm alarm, ManagedEntity entity);
	bool AreAlarmActionsEnabled(ManagedEntity entity);
	Alarm CreateAlarm(ManagedEntity entity, vw1__AlarmSpec*  spec);
	void EnableAlarmActions(ManagedEntity entity, bool enabled);
	std::vector<Alarm> GetAlarm(ManagedEntity entity);
	std::vector<vw1__AlarmState> GetAlarmState(ManagedEntity entity);

public:
	// Property getter
	std::vector<vw1__AlarmExpression> get_defaultExpression() const;
	vw1__AlarmDescription get_description() const;
};

class AuthorizationManager : public mor_handle
{
public:
	AuthorizationManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~AuthorizationManager();

	static const char* type;
	int AddAuthorizationRole(char* name, int sizeprivIds, char** privIds);
	std::vector<bool> HasPrivilegeOnEntity(ManagedEntity entity, char* sessionId, int sizeprivId, char** privId);
	void MergePermissions(int srcRoleId, int dstRoleId);
	void RemoveAuthorizationRole(int roleId, bool failIfUsed);
	void RemoveEntityPermission(ManagedEntity entity, char* user, bool isGroup);
	void ResetEntityPermissions(ManagedEntity entity, int sizepermission, vw1__Permission**  permission);
	std::vector<vw1__Permission> RetrieveAllPermissions();
	std::vector<vw1__Permission> RetrieveEntityPermissions(ManagedEntity entity, bool inherited);
	std::vector<vw1__Permission> RetrieveRolePermissions(int roleId);
	void SetEntityPermissions(ManagedEntity entity, int sizepermission, vw1__Permission**  permission);
	void UpdateAuthorizationRole(int roleId, char* newName, int sizeprivIds, char** privIds);

public:
	// Property getter
	vw1__AuthorizationDescription get_description() const;
	std::vector<vw1__AuthorizationPrivilege> get_privilegeList() const;
	std::vector<vw1__AuthorizationRole> get_roleList() const;
};

class CustomFieldsManager : public mor_handle
{
public:
	CustomFieldsManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~CustomFieldsManager();

	static const char* type;
	vw1__CustomFieldDef AddCustomFieldDef(char* name, char* moType, vw1__PrivilegePolicyDef*  fieldDefPolicy, vw1__PrivilegePolicyDef*  fieldPolicy);
	void RemoveCustomFieldDef(int key);
	void RenameCustomFieldDef(int key, char* name);
	void SetField(ManagedEntity entity, int key, char* value);

public:
	// Property getter
	std::vector<vw1__CustomFieldDef> get_field() const;
};

class CustomizationSpecManager : public mor_handle
{
public:
	CustomizationSpecManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~CustomizationSpecManager();

	static const char* type;
	void CheckCustomizationResources(char* guestOs);
	void CreateCustomizationSpec(vw1__CustomizationSpecItem*  item);
	std::string CustomizationSpecItemToXml(vw1__CustomizationSpecItem*  item);
	void DeleteCustomizationSpec(char* name);
	bool DoesCustomizationSpecExist(char* name);
	void DuplicateCustomizationSpec(char* name, char* newName);
	vw1__CustomizationSpecItem GetCustomizationSpec(char* name);
	void OverwriteCustomizationSpec(vw1__CustomizationSpecItem*  item);
	void RenameCustomizationSpec(char* name, char* newName);
	vw1__CustomizationSpecItem XmlToCustomizationSpecItem(char* specItemXml);

public:
	// Property getter
	std::vector<byte> get_encryptionKey() const;
	std::vector<vw1__CustomizationSpecInfo> get_info() const;
};

class DiagnosticManager : public mor_handle
{
public:
	DiagnosticManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~DiagnosticManager();

	static const char* type;
	vw1__DiagnosticManagerLogHeader BrowseDiagnosticLog(HostSystem host, char* key, int*  start, int*  lines);
	Task GenerateLogBundles_Task(bool includeDefault, int sizehost, HostSystem* host);
	std::vector<vw1__DiagnosticManagerLogDescriptor> QueryDescriptions(HostSystem host);

public:
	// Property getter
};

class DistributedVirtualSwitchManager : public mor_handle
{
public:
	DistributedVirtualSwitchManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~DistributedVirtualSwitchManager();

	static const char* type;
	std::vector<vw1__DistributedVirtualSwitchProductSpec> QueryAvailableDvsSpec();
	std::vector<HostSystem> QueryCompatibleHostForExistingDvs(ManagedEntity container, bool recursive, DistributedVirtualSwitch dvs);
	std::vector<HostSystem> QueryCompatibleHostForNewDvs(ManagedEntity container, bool recursive, vw1__DistributedVirtualSwitchProductSpec*  switchProductSpec);
	DistributedVirtualSwitch QueryDvsByUuid(char* uuid);
	std::vector<vw1__DistributedVirtualSwitchManagerCompatibilityResult> QueryDvsCheckCompatibility(vw1__DistributedVirtualSwitchManagerHostContainer*  hostContainer, vw1__DistributedVirtualSwitchManagerDvsProductSpec*  dvsProductSpec, int sizehostFilterSpec, vw1__DistributedVirtualSwitchManagerHostDvsFilterSpec**  hostFilterSpec);
	std::vector<vw1__DistributedVirtualSwitchHostProductSpec> QueryDvsCompatibleHostSpec(vw1__DistributedVirtualSwitchProductSpec*  switchProductSpec);
	vw1__DVSManagerDvsConfigTarget QueryDvsConfigTarget(HostSystem host, DistributedVirtualSwitch dvs);
	vw1__DVSFeatureCapability QueryDvsFeatureCapability(vw1__DistributedVirtualSwitchProductSpec*  switchProductSpec);
	Task RectifyDvsOnHost_Task(int sizehosts, HostSystem* hosts);

public:
	// Property getter
};

class EnvironmentBrowser : public mor_handle
{
public:
	EnvironmentBrowser(vw1__ManagedObjectReference* mor = 0);
	virtual ~EnvironmentBrowser();

	static const char* type;
	vw1__VirtualMachineConfigOption QueryConfigOption(char* key, HostSystem host);
	std::vector<vw1__VirtualMachineConfigOptionDescriptor> QueryConfigOptionDescriptor();
	vw1__ConfigTarget QueryConfigTarget(HostSystem host);
	vw1__HostCapability QueryTargetCapabilities(HostSystem host);

public:
	// Property getter
	HostDatastoreBrowser get_datastoreBrowser() const;
};

class EventManager : public mor_handle
{
public:
	EventManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~EventManager();

	static const char* type;
	EventHistoryCollector CreateCollectorForEvents(vw1__EventFilterSpec*  filter);
	void LogUserEvent(ManagedEntity entity, char* msg);
	void PostEvent(vw1__Event*  eventToPost, vw1__TaskInfo*  taskInfo);
	std::vector<vw1__Event> QueryEvents(vw1__EventFilterSpec*  filter);
	std::vector<vw1__EventArgDesc> RetrieveArgumentDescription(char* eventTypeId);

public:
	// Property getter
	vw1__EventDescription get_description() const;
	vw1__Event get_latestEvent() const;
	int get_maxCollector() const;
};

class ExtensibleManagedObject : public mor_handle
{
public:
	ExtensibleManagedObject(vw1__ManagedObjectReference* mor = 0);
	virtual ~ExtensibleManagedObject();

	static const char* type;
	void setCustomValue(char* key, char* value);

public:
	// Property getter
	std::vector<vw1__CustomFieldDef> get_availableField() const;
	std::vector<vw1__CustomFieldValue> get_value() const;
};

class ExtensionManager : public mor_handle
{
public:
	ExtensionManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~ExtensionManager();

	static const char* type;
	vw1__Extension FindExtension(char* extensionKey);
	std::string GetPublicKey();
	std::vector<ManagedEntity> QueryManagedBy(char* extensionKey);
	void RegisterExtension(vw1__Extension*  extension);
	void SetExtensionCertificate(char* extensionKey, char* certificatePem);
	void SetPublicKey(char* extensionKey, char* publicKey);
	void UnregisterExtension(char* extensionKey);
	void UpdateExtension(vw1__Extension*  extension);

public:
	// Property getter
	std::vector<vw1__Extension> get_extensionList() const;
};

class FileManager : public mor_handle
{
public:
	FileManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~FileManager();

	static const char* type;
	void ChangeOwner(char* name, Datacenter datacenter, char* owner);
	Task CopyDatastoreFile_Task(char* sourceName, Datacenter sourceDatacenter, char* destinationName, Datacenter destinationDatacenter, bool*  force);
	Task DeleteDatastoreFile_Task(char* name, Datacenter datacenter);
	void MakeDirectory(char* name, Datacenter datacenter, bool*  createParentDirectories);
	Task MoveDatastoreFile_Task(char* sourceName, Datacenter sourceDatacenter, char* destinationName, Datacenter destinationDatacenter, bool*  force);

public:
	// Property getter
};

class GuestAuthManager : public mor_handle
{
public:
	GuestAuthManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~GuestAuthManager();

	static const char* type;
	vw1__GuestAuthentication AcquireCredentialsInGuest(VirtualMachine vm, vw1__GuestAuthentication*  requestedAuth, __int64*  sessionID);
	void ReleaseCredentialsInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth);
	void ValidateCredentialsInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth);

public:
	// Property getter
};

class GuestFileManager : public mor_handle
{
public:
	GuestFileManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~GuestFileManager();

	static const char* type;
	void ChangeFileAttributesInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* guestFilePath, vw1__GuestFileAttributes*  fileAttributes);
	std::string CreateTemporaryDirectoryInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* prefix, char* suffix, char* directoryPath);
	std::string CreateTemporaryFileInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* prefix, char* suffix, char* directoryPath);
	void DeleteDirectoryInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* directoryPath, bool recursive);
	void DeleteFileInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* filePath);
	vw1__FileTransferInformation InitiateFileTransferFromGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* guestFilePath);
	std::string InitiateFileTransferToGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* guestFilePath, vw1__GuestFileAttributes*  fileAttributes, __int64 fileSize, bool overwrite);
	vw1__GuestListFileInfo ListFilesInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* filePath, int*  index, int*  maxResults, char* matchPattern);
	void MakeDirectoryInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* directoryPath, bool createParentDirectories);
	void MoveDirectoryInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* srcDirectoryPath, char* dstDirectoryPath);
	void MoveFileInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, char* srcFilePath, char* dstFilePath, bool overwrite);

public:
	// Property getter
};

class GuestOperationsManager : public mor_handle
{
public:
	GuestOperationsManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~GuestOperationsManager();

	static const char* type;

public:
	// Property getter
	GuestAuthManager get_authManager() const;
	GuestFileManager get_fileManager() const;
	GuestProcessManager get_processManager() const;
};

class GuestProcessManager : public mor_handle
{
public:
	GuestProcessManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~GuestProcessManager();

	static const char* type;
	std::vector<vw1__GuestProcessInfo> ListProcessesInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, int sizepids, __int64* pids);
	std::vector<std::string> ReadEnvironmentVariableInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, int sizenames, char** names);
	__int64 StartProgramInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, vw1__GuestProgramSpec*  spec);
	void TerminateProcessInGuest(VirtualMachine vm, vw1__GuestAuthentication*  auth, __int64 pid);

public:
	// Property getter
};

class HistoryCollector : public mor_handle
{
public:
	HistoryCollector(vw1__ManagedObjectReference* mor = 0);
	virtual ~HistoryCollector();

	static const char* type;
	void DestroyCollector();
	void ResetCollector();
	void RewindCollector();
	void SetCollectorPageSize(int maxCount);

public:
	// Property getter
	anyType get_filter() const;
};

class HostAuthenticationManager : public mor_handle
{
public:
	HostAuthenticationManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostAuthenticationManager();

	static const char* type;

public:
	// Property getter
	vw1__HostAuthenticationManagerInfo get_info() const;
	std::vector<HostAuthenticationStore> get_supportedStore() const;
};

class HostAuthenticationStore : public mor_handle
{
public:
	HostAuthenticationStore(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostAuthenticationStore();

	static const char* type;

public:
	// Property getter
	vw1__HostAuthenticationStoreInfo get_info() const;
};

class HostAutoStartManager : public mor_handle
{
public:
	HostAutoStartManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostAutoStartManager();

	static const char* type;
	void AutoStartPowerOff();
	void AutoStartPowerOn();
	void ReconfigureAutostart(vw1__HostAutoStartManagerConfig*  spec);

public:
	// Property getter
	vw1__HostAutoStartManagerConfig get_config() const;
};

class HostBootDeviceSystem : public mor_handle
{
public:
	HostBootDeviceSystem(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostBootDeviceSystem();

	static const char* type;
	vw1__HostBootDeviceInfo QueryBootDevices();
	void UpdateBootDevice(char* key);

public:
	// Property getter
};

class HostCacheConfigurationManager : public mor_handle
{
public:
	HostCacheConfigurationManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostCacheConfigurationManager();

	static const char* type;
	Task ConfigureHostCache_Task(vw1__HostCacheConfigurationSpec*  spec);

public:
	// Property getter
	std::vector<vw1__HostCacheConfigurationInfo> get_cacheConfigurationInfo() const;
};

class HostCpuSchedulerSystem : public ExtensibleManagedObject
{
public:
	HostCpuSchedulerSystem(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostCpuSchedulerSystem();

	static const char* type;
	void DisableHyperThreading();
	void EnableHyperThreading();

public:
	// Property getter
	vw1__HostHyperThreadScheduleInfo get_hyperthreadInfo() const;
};

class HostDatastoreBrowser : public mor_handle
{
public:
	HostDatastoreBrowser(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostDatastoreBrowser();

	static const char* type;
	void DeleteFile(char* datastorePath);
	Task SearchDatastore_Task(char* datastorePath, vw1__HostDatastoreBrowserSearchSpec*  searchSpec);
	Task SearchDatastoreSubFolders_Task(char* datastorePath, vw1__HostDatastoreBrowserSearchSpec*  searchSpec);

public:
	// Property getter
	std::vector<Datastore> get_datastore() const;
	std::vector<vw1__FileQuery> get_supportedType() const;
};

class HostDatastoreSystem : public mor_handle
{
public:
	HostDatastoreSystem(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostDatastoreSystem();

	static const char* type;
	void ConfigureDatastorePrincipal(char* userName, char* password);
	Datastore CreateLocalDatastore(char* name, char* path);
	Datastore CreateNasDatastore(vw1__HostNasVolumeSpec*  spec);
	Datastore CreateVmfsDatastore(vw1__VmfsDatastoreCreateSpec*  spec);
	Datastore ExpandVmfsDatastore(Datastore datastore, vw1__VmfsDatastoreExpandSpec*  spec);
	Datastore ExtendVmfsDatastore(Datastore datastore, vw1__VmfsDatastoreExtendSpec*  spec);
	std::vector<vw1__HostScsiDisk> QueryAvailableDisksForVmfs(Datastore datastore);
	std::vector<vw1__HostUnresolvedVmfsVolume> QueryUnresolvedVmfsVolumes();
	std::vector<vw1__VmfsDatastoreOption> QueryVmfsDatastoreCreateOptions(char* devicePath, int*  vmfsMajorVersion);
	std::vector<vw1__VmfsDatastoreOption> QueryVmfsDatastoreExpandOptions(Datastore datastore);
	std::vector<vw1__VmfsDatastoreOption> QueryVmfsDatastoreExtendOptions(Datastore datastore, char* devicePath, bool*  suppressExpandCandidates);
	void RemoveDatastore(Datastore datastore);
	Task ResignatureUnresolvedVmfsVolume_Task(vw1__HostUnresolvedVmfsResignatureSpec*  resolutionSpec);
	void UpdateLocalSwapDatastore(Datastore datastore);

public:
	// Property getter
	vw1__HostDatastoreSystemCapabilities get_capabilities() const;
	std::vector<Datastore> get_datastore() const;
};

class HostDateTimeSystem : public mor_handle
{
public:
	HostDateTimeSystem(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostDateTimeSystem();

	static const char* type;
	std::vector<vw1__HostDateTimeSystemTimeZone> QueryAvailableTimeZones();
	time_t QueryDateTime();
	void RefreshDateTimeSystem();
	void UpdateDateTime(time_t dateTime);
	void UpdateDateTimeConfig(vw1__HostDateTimeConfig*  config);

public:
	// Property getter
	vw1__HostDateTimeInfo get_dateTimeInfo() const;
};

class HostDiagnosticSystem : public mor_handle
{
public:
	HostDiagnosticSystem(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostDiagnosticSystem();

	static const char* type;
	void CreateDiagnosticPartition(vw1__HostDiagnosticPartitionCreateSpec*  spec);
	std::vector<vw1__HostDiagnosticPartition> QueryAvailablePartition();
	vw1__HostDiagnosticPartitionCreateDescription QueryPartitionCreateDesc(char* diskUuid, char* diagnosticType);
	std::vector<vw1__HostDiagnosticPartitionCreateOption> QueryPartitionCreateOptions(char* storageType, char* diagnosticType);
	void SelectActivePartition(vw1__HostScsiDiskPartition*  partition);

public:
	// Property getter
	vw1__HostDiagnosticPartition get_activePartition() const;
};

class HostDirectoryStore : public HostAuthenticationStore
{
public:
	HostDirectoryStore(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostDirectoryStore();

	static const char* type;

public:
	// Property getter
};

class HostEsxAgentHostManager : public mor_handle
{
public:
	HostEsxAgentHostManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostEsxAgentHostManager();

	static const char* type;
	void EsxAgentHostManagerUpdateConfig(vw1__HostEsxAgentHostManagerConfigInfo*  configInfo);

public:
	// Property getter
	vw1__HostEsxAgentHostManagerConfigInfo get_configInfo() const;
};

class HostFirewallSystem : public ExtensibleManagedObject
{
public:
	HostFirewallSystem(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostFirewallSystem();

	static const char* type;
	void DisableRuleset(char* id);
	void EnableRuleset(char* id);
	void RefreshFirewall();
	void UpdateDefaultPolicy(vw1__HostFirewallDefaultPolicy*  defaultPolicy);
	void UpdateRuleset(char* id, vw1__HostFirewallRulesetRulesetSpec*  spec);

public:
	// Property getter
	vw1__HostFirewallInfo get_firewallInfo() const;
};

class HostFirmwareSystem : public mor_handle
{
public:
	HostFirmwareSystem(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostFirmwareSystem();

	static const char* type;
	std::string BackupFirmwareConfiguration();
	std::string QueryFirmwareConfigUploadURL();
	void ResetFirmwareToFactoryDefaults();
	void RestoreFirmwareConfiguration(bool force);

public:
	// Property getter
};

class HostHealthStatusSystem : public mor_handle
{
public:
	HostHealthStatusSystem(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostHealthStatusSystem();

	static const char* type;
	void RefreshHealthStatusSystem();
	void ResetSystemHealthInfo();

public:
	// Property getter
	vw1__HealthSystemRuntime get_runtime() const;
};

class HostImageConfigManager : public mor_handle
{
public:
	HostImageConfigManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostImageConfigManager();

	static const char* type;
	std::string HostImageConfigGetAcceptance();
	vw1__HostImageProfileSummary HostImageConfigGetProfile();
	void UpdateHostImageAcceptanceLevel(char* newAcceptanceLevel);

public:
	// Property getter
};

class HostKernelModuleSystem : public mor_handle
{
public:
	HostKernelModuleSystem(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostKernelModuleSystem();

	static const char* type;
	std::string QueryConfiguredModuleOptionString(char* name);
	std::vector<vw1__KernelModuleInfo> QueryModules();
	void UpdateModuleOptionString(char* name, char* options);

public:
	// Property getter
};

class HostLocalAccountManager : public mor_handle
{
public:
	HostLocalAccountManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostLocalAccountManager();

	static const char* type;
	void AssignUserToGroup(char* user, char* group);
	void CreateGroup(vw1__HostAccountSpec*  group);
	void CreateUser(vw1__HostAccountSpec*  user);
	void RemoveGroup(char* groupName);
	void RemoveUser(char* userName);
	void UnassignUserFromGroup(char* user, char* group);
	void UpdateUser(vw1__HostAccountSpec*  user);

public:
	// Property getter
};

class HostLocalAuthentication : public HostAuthenticationStore
{
public:
	HostLocalAuthentication(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostLocalAuthentication();

	static const char* type;

public:
	// Property getter
};

class HostMemorySystem : public ExtensibleManagedObject
{
public:
	HostMemorySystem(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostMemorySystem();

	static const char* type;
	void ReconfigureServiceConsoleReservation(__int64 cfgBytes);
	void ReconfigureVirtualMachineReservation(vw1__VirtualMachineMemoryReservationSpec*  spec);

public:
	// Property getter
	vw1__ServiceConsoleReservationInfo get_consoleReservationInfo() const;
	vw1__VirtualMachineMemoryReservationInfo get_virtualMachineReservationInfo() const;
};

class HostNetworkSystem : public ExtensibleManagedObject
{
public:
	HostNetworkSystem(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostNetworkSystem();

	static const char* type;
	void AddPortGroup(vw1__HostPortGroupSpec*  portgrp);
	std::string AddServiceConsoleVirtualNic(char* portgroup, vw1__HostVirtualNicSpec*  nic);
	std::string AddVirtualNic(char* portgroup, vw1__HostVirtualNicSpec*  nic);
	void AddVirtualSwitch(char* vswitchName, vw1__HostVirtualSwitchSpec*  spec);
	std::vector<vw1__PhysicalNicHintInfo> QueryNetworkHint(int sizedevice, char** device);
	void RefreshNetworkSystem();
	void RemovePortGroup(char* pgName);
	void RemoveServiceConsoleVirtualNic(char* device);
	void RemoveVirtualNic(char* device);
	void RemoveVirtualSwitch(char* vswitchName);
	void RestartServiceConsoleVirtualNic(char* device);
	void UpdateConsoleIpRouteConfig(vw1__HostIpRouteConfig*  config);
	void UpdateDnsConfig(vw1__HostDnsConfig*  config);
	void UpdateIpRouteConfig(vw1__HostIpRouteConfig*  config);
	void UpdateIpRouteTableConfig(vw1__HostIpRouteTableConfig*  config);
	vw1__HostNetworkConfigResult UpdateNetworkConfig(vw1__HostNetworkConfig*  config, char* changeMode);
	void UpdatePhysicalNicLinkSpeed(char* device, vw1__PhysicalNicLinkInfo*  linkSpeed);
	void UpdatePortGroup(char* pgName, vw1__HostPortGroupSpec*  portgrp);
	void UpdateServiceConsoleVirtualNic(char* device, vw1__HostVirtualNicSpec*  nic);
	void UpdateVirtualNic(char* device, vw1__HostVirtualNicSpec*  nic);
	void UpdateVirtualSwitch(char* vswitchName, vw1__HostVirtualSwitchSpec*  spec);

public:
	// Property getter
	vw1__HostNetCapabilities get_capabilities() const;
	vw1__HostIpRouteConfig get_consoleIpRouteConfig() const;
	vw1__HostDnsConfig get_dnsConfig() const;
	vw1__HostIpRouteConfig get_ipRouteConfig() const;
	vw1__HostNetworkConfig get_networkConfig() const;
	vw1__HostNetworkInfo get_networkInfo() const;
	vw1__HostNetOffloadCapabilities get_offloadCapabilities() const;
};

class HostPatchManager : public mor_handle
{
public:
	HostPatchManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostPatchManager();

	static const char* type;
	Task CheckHostPatch_Task(int sizemetaUrls, char** metaUrls, int sizebundleUrls, char** bundleUrls, vw1__HostPatchManagerPatchManagerOperationSpec*  spec);
	Task InstallHostPatch_Task(vw1__HostPatchManagerLocator*  repository, char* updateID, bool*  force);
	Task InstallHostPatchV2_Task(int sizemetaUrls, char** metaUrls, int sizebundleUrls, char** bundleUrls, int sizevibUrls, char** vibUrls, vw1__HostPatchManagerPatchManagerOperationSpec*  spec);
	Task QueryHostPatch_Task(vw1__HostPatchManagerPatchManagerOperationSpec*  spec);
	Task ScanHostPatch_Task(vw1__HostPatchManagerLocator*  repository, int sizeupdateID, char** updateID);
	Task ScanHostPatchV2_Task(int sizemetaUrls, char** metaUrls, int sizebundleUrls, char** bundleUrls, vw1__HostPatchManagerPatchManagerOperationSpec*  spec);
	Task StageHostPatch_Task(int sizemetaUrls, char** metaUrls, int sizebundleUrls, char** bundleUrls, int sizevibUrls, char** vibUrls, vw1__HostPatchManagerPatchManagerOperationSpec*  spec);
	Task UninstallHostPatch_Task(int sizebulletinIds, char** bulletinIds, vw1__HostPatchManagerPatchManagerOperationSpec*  spec);

public:
	// Property getter
};

class HostPciPassthruSystem : public ExtensibleManagedObject
{
public:
	HostPciPassthruSystem(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostPciPassthruSystem();

	static const char* type;
	void Refresh();
	void UpdatePassthruConfig(int sizeconfig, vw1__HostPciPassthruConfig**  config);

public:
	// Property getter
	std::vector<vw1__HostPciPassthruInfo> get_pciPassthruInfo() const;
};

class HostPowerSystem : public mor_handle
{
public:
	HostPowerSystem(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostPowerSystem();

	static const char* type;
	void ConfigurePowerPolicy(int key);

public:
	// Property getter
	vw1__PowerSystemCapability get_capability() const;
	vw1__PowerSystemInfo get_info() const;
};

class HostServiceSystem : public ExtensibleManagedObject
{
public:
	HostServiceSystem(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostServiceSystem();

	static const char* type;
	void RefreshServices();
	void RestartService(char* id);
	void StartService(char* id);
	void StopService(char* id);
	void UninstallService(char* id);
	void UpdateServicePolicy(char* id, char* policy);

public:
	// Property getter
	vw1__HostServiceInfo get_serviceInfo() const;
};

class HostSnmpSystem : public mor_handle
{
public:
	HostSnmpSystem(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostSnmpSystem();

	static const char* type;
	void ReconfigureSnmpAgent(vw1__HostSnmpConfigSpec*  spec);
	void SendTestNotification();

public:
	// Property getter
	vw1__HostSnmpConfigSpec get_configuration() const;
	vw1__HostSnmpSystemAgentLimits get_limits() const;
};

class HostStorageSystem : public ExtensibleManagedObject
{
public:
	HostStorageSystem(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostStorageSystem();

	static const char* type;
	void AddInternetScsiSendTargets(char* iScsiHbaDevice, int sizetargets, vw1__HostInternetScsiHbaSendTarget**  targets);
	void AddInternetScsiStaticTargets(char* iScsiHbaDevice, int sizetargets, vw1__HostInternetScsiHbaStaticTarget**  targets);
	void AttachScsiLun(char* lunUuid);
	void AttachVmfsExtent(char* vmfsPath, vw1__HostScsiDiskPartition*  extent);
	vw1__HostDiskPartitionInfo ComputeDiskPartitionInfo(char* devicePath, vw1__HostDiskPartitionLayout*  layout, char* partitionFormat);
	vw1__HostDiskPartitionInfo ComputeDiskPartitionInfoForResize(vw1__HostScsiDiskPartition*  partition, vw1__HostDiskPartitionBlockRange*  blockRange, char* partitionFormat);
	void DetachScsiLun(char* lunUuid);
	void DisableMultipathPath(char* pathName);
	void DiscoverFcoeHbas(vw1__FcoeConfigFcoeSpecification*  fcoeSpec);
	void EnableMultipathPath(char* pathName);
	void ExpandVmfsExtent(char* vmfsPath, vw1__HostScsiDiskPartition*  extent);
	vw1__HostVmfsVolume FormatVmfs(vw1__HostVmfsSpec*  createSpec);
	void MarkForRemoval(char* hbaName, bool remove);
	void MountVmfsVolume(char* vmfsUuid);
	std::vector<vw1__HostPathSelectionPolicyOption> QueryPathSelectionPolicyOptions();
	std::vector<vw1__HostStorageArrayTypePolicyOption> QueryStorageArrayTypePolicyOptions();
	std::vector<vw1__HostUnresolvedVmfsVolume> QueryUnresolvedVmfsVolume();
	void RefreshStorageSystem();
	void RemoveInternetScsiSendTargets(char* iScsiHbaDevice, int sizetargets, vw1__HostInternetScsiHbaSendTarget**  targets);
	void RemoveInternetScsiStaticTargets(char* iScsiHbaDevice, int sizetargets, vw1__HostInternetScsiHbaStaticTarget**  targets);
	void RescanAllHba();
	void RescanHba(char* hbaDevice);
	void RescanVmfs();
	std::vector<vw1__HostUnresolvedVmfsResolutionResult> ResolveMultipleUnresolvedVmfsVolumes(int sizeresolutionSpec, vw1__HostUnresolvedVmfsResolutionSpec**  resolutionSpec);
	std::vector<vw1__HostDiskPartitionInfo> RetrieveDiskPartitionInfo(int sizedevicePath, char** devicePath);
	void SetMultipathLunPolicy(char* lunId, vw1__HostMultipathInfoLogicalUnitPolicy*  policy);
	void UnmountForceMountedVmfsVolume(char* vmfsUuid);
	void UnmountVmfsVolume(char* vmfsUuid);
	void UpdateDiskPartitions(char* devicePath, vw1__HostDiskPartitionSpec*  spec);
	void UpdateInternetScsiAdvancedOptions(char* iScsiHbaDevice, vw1__HostInternetScsiHbaTargetSet*  targetSet, int sizeoptions, vw1__HostInternetScsiHbaParamValue**  options);
	void UpdateInternetScsiAlias(char* iScsiHbaDevice, char* iScsiAlias);
	void UpdateInternetScsiAuthenticationProperties(char* iScsiHbaDevice, vw1__HostInternetScsiHbaAuthenticationProperties*  authenticationProperties, vw1__HostInternetScsiHbaTargetSet*  targetSet);
	void UpdateInternetScsiDigestProperties(char* iScsiHbaDevice, vw1__HostInternetScsiHbaTargetSet*  targetSet, vw1__HostInternetScsiHbaDigestProperties*  digestProperties);
	void UpdateInternetScsiDiscoveryProperties(char* iScsiHbaDevice, vw1__HostInternetScsiHbaDiscoveryProperties*  discoveryProperties);
	void UpdateInternetScsiIPProperties(char* iScsiHbaDevice, vw1__HostInternetScsiHbaIPProperties*  ipProperties);
	void UpdateInternetScsiName(char* iScsiHbaDevice, char* iScsiName);
	void UpdateScsiLunDisplayName(char* lunUuid, char* displayName);
	void UpdateSoftwareInternetScsiEnabled(bool enabled);
	void UpgradeVmfs(char* vmfsPath);
	void UpgradeVmLayout();

public:
	// Property getter
	vw1__HostFileSystemVolumeInfo get_fileSystemVolumeInfo() const;
	vw1__HostMultipathStateInfo get_multipathStateInfo() const;
	vw1__HostStorageDeviceInfo get_storageDeviceInfo() const;
	std::vector<std::string> get_systemFile() const;
};

class HostVMotionSystem : public ExtensibleManagedObject
{
public:
	HostVMotionSystem(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostVMotionSystem();

	static const char* type;
	void DeselectVnic();
	void SelectVnic(char* device);
	void UpdateIpConfig(vw1__HostIpConfig*  ipConfig);

public:
	// Property getter
	vw1__HostIpConfig get_ipConfig() const;
	vw1__HostVMotionNetConfig get_netConfig() const;
};

class HostVirtualNicManager : public ExtensibleManagedObject
{
public:
	HostVirtualNicManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostVirtualNicManager();

	static const char* type;
	void DeselectVnicForNicType(char* nicType, char* device);
	vw1__VirtualNicManagerNetConfig QueryNetConfig(char* nicType);
	void SelectVnicForNicType(char* nicType, char* device);

public:
	// Property getter
	vw1__HostVirtualNicManagerInfo get_info() const;
};

class HttpNfcLease : public mor_handle
{
public:
	HttpNfcLease(vw1__ManagedObjectReference* mor = 0);
	virtual ~HttpNfcLease();

	static const char* type;
	void HttpNfcLeaseAbort(vw1__LocalizedMethodFault*  fault);
	void HttpNfcLeaseComplete();
	std::vector<vw1__HttpNfcLeaseManifestEntry> HttpNfcLeaseGetManifest();
	void HttpNfcLeaseProgress(int percent);

public:
	// Property getter
	vw1__LocalizedMethodFault get_error() const;
	vw1__HttpNfcLeaseInfo get_info() const;
	int get_initializeProgress() const;
	vw1__HttpNfcLeaseState get_state() const;
};

class IpPoolManager : public mor_handle
{
public:
	IpPoolManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~IpPoolManager();

	static const char* type;
	int CreateIpPool(Datacenter dc, vw1__IpPool*  pool);
	void DestroyIpPool(Datacenter dc, int id, bool force);
	std::vector<vw1__IpPool> QueryIpPools(Datacenter dc);
	void UpdateIpPool(Datacenter dc, vw1__IpPool*  pool);

public:
	// Property getter
};

class IscsiManager : public mor_handle
{
public:
	IscsiManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~IscsiManager();

	static const char* type;
	void BindVnic(char* iScsiHbaName, char* vnicDevice);
	std::vector<vw1__IscsiPortInfo> QueryBoundVnics(char* iScsiHbaName);
	std::vector<vw1__IscsiPortInfo> QueryCandidateNics(char* iScsiHbaName);
	vw1__IscsiMigrationDependency QueryMigrationDependencies(int sizepnicDevice, char** pnicDevice);
	vw1__IscsiStatus QueryPnicStatus(char* pnicDevice);
	vw1__IscsiStatus QueryVnicStatus(char* vnicDevice);
	void UnbindVnic(char* iScsiHbaName, char* vnicDevice, bool force);

public:
	// Property getter
};

class LicenseAssignmentManager : public mor_handle
{
public:
	LicenseAssignmentManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~LicenseAssignmentManager();

	static const char* type;
	std::vector<vw1__LicenseAssignmentManagerLicenseAssignment> QueryAssignedLicenses(char* entityId);
	void RemoveAssignedLicense(char* entityId);
	vw1__LicenseManagerLicenseInfo UpdateAssignedLicense(char* entity, char* licenseKey, char* entityDisplayName);

public:
	// Property getter
};

class LicenseManager : public mor_handle
{
public:
	LicenseManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~LicenseManager();

	static const char* type;
	vw1__LicenseManagerLicenseInfo AddLicense(char* licenseKey, int sizelabels, vw1__KeyValue**  labels);
	bool CheckLicenseFeature(HostSystem host, char* featureKey);
	void ConfigureLicenseSource(HostSystem host, vw1__LicenseSource*  licenseSource);
	vw1__LicenseManagerLicenseInfo DecodeLicense(char* licenseKey);
	bool DisableFeature(HostSystem host, char* featureKey);
	bool EnableFeature(HostSystem host, char* featureKey);
	std::vector<vw1__LicenseAvailabilityInfo> QueryLicenseSourceAvailability(HostSystem host);
	vw1__LicenseUsageInfo QueryLicenseUsage(HostSystem host);
	std::vector<vw1__LicenseFeatureInfo> QuerySupportedFeatures(HostSystem host);
	void RemoveLicense(char* licenseKey);
	void RemoveLicenseLabel(char* licenseKey, char* labelKey);
	void SetLicenseEdition(HostSystem host, char* featureKey);
	vw1__LicenseManagerLicenseInfo UpdateLicense(char* licenseKey, int sizelabels, vw1__KeyValue**  labels);
	void UpdateLicenseLabel(char* licenseKey, char* labelKey, char* labelValue);

public:
	// Property getter
	vw1__LicenseDiagnostics get_diagnostics() const;
	vw1__LicenseManagerEvaluationInfo get_evaluation() const;
	std::vector<vw1__LicenseFeatureInfo> get_featureInfo() const;
	LicenseAssignmentManager get_licenseAssignmentManager() const;
	std::string get_licensedEdition() const;
	std::vector<vw1__LicenseManagerLicenseInfo> get_licenses() const;
	vw1__LicenseSource get_source() const;
	bool get_sourceAvailable() const;
};

class LocalizationManager : public mor_handle
{
public:
	LocalizationManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~LocalizationManager();

	static const char* type;

public:
	// Property getter
	std::vector<vw1__LocalizationManagerMessageCatalog> get_catalog() const;
};

class ManagedEntity : public ExtensibleManagedObject
{
public:
	ManagedEntity(vw1__ManagedObjectReference* mor = 0);
	virtual ~ManagedEntity();

	static const char* type;
	Task Destroy_Task();
	void Reload();
	Task Rename_Task(char* newName);

public:
	// Property getter
	bool get_alarmActionsEnabled() const;
	std::vector<vw1__Event> get_configIssue() const;
	vw1__ManagedEntityStatus get_configStatus() const;
	std::vector<vw1__CustomFieldValue> get_customValue() const;
	std::vector<vw1__AlarmState> get_declaredAlarmState() const;
	std::vector<std::string> get_disabledMethod() const;
	std::vector<int> get_effectiveRole() const;
	std::string get_name() const;
	vw1__ManagedEntityStatus get_overallStatus() const;
	ManagedEntity get_parent() const;
	std::vector<vw1__Permission> get_permission() const;
	std::vector<Task> get_recentTask() const;
	std::vector<vw1__Tag> get_tag() const;
	std::vector<vw1__AlarmState> get_triggeredAlarmState() const;
};

class Network : public ManagedEntity
{
public:
	Network(vw1__ManagedObjectReference* mor = 0);
	virtual ~Network();

	static const char* type;
	void DestroyNetwork();

public:
	// Property getter
	std::vector<HostSystem> get_host() const;
	std::string get_name() const;
	vw1__NetworkSummary get_summary() const;
	std::vector<VirtualMachine> get_vm() const;
};

class OptionManager : public mor_handle
{
public:
	OptionManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~OptionManager();

	static const char* type;
	std::vector<vw1__OptionValue> QueryOptions(char* name);
	void UpdateOptions(int sizechangedValue, vw1__OptionValue**  changedValue);

public:
	// Property getter
	std::vector<vw1__OptionValue> get_setting() const;
	std::vector<vw1__OptionDef> get_supportedOption() const;
};

class OvfManager : public mor_handle
{
public:
	OvfManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~OvfManager();

	static const char* type;
	vw1__OvfCreateDescriptorResult CreateDescriptor(ManagedEntity obj, vw1__OvfCreateDescriptorParams*  cdp);
	vw1__OvfCreateImportSpecResult CreateImportSpec(char* ovfDescriptor, ResourcePool resourcePool, Datastore datastore, vw1__OvfCreateImportSpecParams*  cisp);
	vw1__OvfParseDescriptorResult ParseDescriptor(char* ovfDescriptor, vw1__OvfParseDescriptorParams*  pdp);
	vw1__OvfValidateHostResult ValidateHost(char* ovfDescriptor, HostSystem host, vw1__OvfValidateHostParams*  vhp);

public:
	// Property getter
};

class PerformanceManager : public mor_handle
{
public:
	PerformanceManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~PerformanceManager();

	static const char* type;
	void CreatePerfInterval(vw1__PerfInterval*  intervalId);
	std::vector<vw1__PerfMetricId> QueryAvailablePerfMetric(vw1__ManagedObjectReference*  entity, time_t*  beginTime, time_t*  endTime, int*  intervalId);
	std::vector<vw1__PerfEntityMetricBase> QueryPerf(int sizequerySpec, vw1__PerfQuerySpec**  querySpec);
	vw1__PerfCompositeMetric QueryPerfComposite(vw1__PerfQuerySpec*  querySpec);
	std::vector<vw1__PerfCounterInfo> QueryPerfCounter(int sizecounterId, int* counterId);
	std::vector<vw1__PerfCounterInfo> QueryPerfCounterByLevel(int level);
	vw1__PerfProviderSummary QueryPerfProviderSummary(vw1__ManagedObjectReference*  entity);
	void RemovePerfInterval(int samplePeriod);
	void ResetCounterLevelMapping(int sizecounters, int* counters);
	void UpdateCounterLevelMapping(int sizecounterLevelMap, vw1__PerformanceManagerCounterLevelMapping**  counterLevelMap);
	void UpdatePerfInterval(vw1__PerfInterval*  interval);

public:
	// Property getter
	vw1__PerformanceDescription get_description() const;
	std::vector<vw1__PerfInterval> get_historicalInterval() const;
	std::vector<vw1__PerfCounterInfo> get_perfCounter() const;
};

class Profile : public mor_handle
{
public:
	Profile(vw1__ManagedObjectReference* mor = 0);
	virtual ~Profile();

	static const char* type;
	void AssociateProfile(int sizeentity, ManagedEntity* entity);
	Task CheckProfileCompliance_Task(int sizeentity, ManagedEntity* entity);
	void DestroyProfile();
	void DissociateProfile(int sizeentity, ManagedEntity* entity);
	std::string ExportProfile();
	vw1__ProfileDescription RetrieveDescription();

public:
	// Property getter
	std::string get_complianceStatus() const;
	vw1__ProfileConfigInfo get_config() const;
	time_t get_createdTime() const;
	vw1__ProfileDescription get_description() const;
	std::vector<ManagedEntity> get_entity() const;
	time_t get_modifiedTime() const;
	std::string get_name() const;
};

class ProfileComplianceManager : public mor_handle
{
public:
	ProfileComplianceManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~ProfileComplianceManager();

	static const char* type;
	Task CheckCompliance_Task(int sizeprofile, Profile* profile, int sizeentity, ManagedEntity* entity);
	void ClearComplianceStatus(int sizeprofile, Profile* profile, int sizeentity, ManagedEntity* entity);
	std::vector<vw1__ComplianceResult> QueryComplianceStatus(int sizeprofile, Profile* profile, int sizeentity, ManagedEntity* entity);
	std::vector<vw1__ProfileExpressionMetadata> QueryExpressionMetadata(int sizeexpressionName, char** expressionName, Profile profile);

public:
	// Property getter
};

class ProfileManager : public mor_handle
{
public:
	ProfileManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~ProfileManager();

	static const char* type;
	Profile CreateProfile(vw1__ProfileCreateSpec*  createSpec);
	std::vector<Profile> FindAssociatedProfile(ManagedEntity entity);
	std::vector<vw1__ProfilePolicyMetadata> QueryPolicyMetadata(int sizepolicyName, char** policyName, Profile profile);

public:
	// Property getter
	std::vector<Profile> get_profile() const;
};

class PropertyCollector : public mor_handle
{
public:
	PropertyCollector(vw1__ManagedObjectReference* mor = 0);
	virtual ~PropertyCollector();

	static const char* type;
	void CancelRetrievePropertiesEx(char* token);
	void CancelWaitForUpdates();
	vw1__UpdateSet CheckForUpdates(char* version);
	vw1__RetrieveResult ContinueRetrievePropertiesEx(char* token);
	PropertyFilter CreateFilter(vw1__PropertyFilterSpec*  spec, bool partialUpdates);
	PropertyCollector CreatePropertyCollector();
	void DestroyPropertyCollector();
	std::vector<vw1__ObjectContent> RetrieveProperties(int sizespecSet, vw1__PropertyFilterSpec**  specSet);
	vw1__RetrieveResult RetrievePropertiesEx(int sizespecSet, vw1__PropertyFilterSpec**  specSet, vw1__RetrieveOptions*  options);
	vw1__UpdateSet WaitForUpdates(char* version);
	vw1__UpdateSet WaitForUpdatesEx(char* version, vw1__WaitOptions*  options);

public:
	// Property getter
	std::vector<PropertyFilter> get_filter() const;
};

class PropertyFilter : public mor_handle
{
public:
	PropertyFilter(vw1__ManagedObjectReference* mor = 0);
	virtual ~PropertyFilter();

	static const char* type;
	void DestroyPropertyFilter();

public:
	// Property getter
	bool get_partialUpdates() const;
	vw1__PropertyFilterSpec get_spec() const;
};

class ResourcePlanningManager : public mor_handle
{
public:
	ResourcePlanningManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~ResourcePlanningManager();

	static const char* type;
	vw1__DatabaseSizeEstimate EstimateDatabaseSize(vw1__DatabaseSizeParam*  dbSizeParam);

public:
	// Property getter
};

class ResourcePool : public ManagedEntity
{
public:
	ResourcePool(vw1__ManagedObjectReference* mor = 0);
	virtual ~ResourcePool();

	static const char* type;
	Task CreateChildVM_Task(vw1__VirtualMachineConfigSpec*  config, HostSystem host);
	ResourcePool CreateResourcePool(char* name, vw1__ResourceConfigSpec*  spec);
	VirtualApp CreateVApp(char* name, vw1__ResourceConfigSpec*  resSpec, vw1__VAppConfigSpec*  configSpec, Folder vmFolder);
	void DestroyChildren();
	HttpNfcLease ImportVApp(vw1__ImportSpec*  spec, Folder folder, HostSystem host);
	void MoveIntoResourcePool(int sizelist, ManagedEntity* list);
	vw1__ResourceConfigOption QueryResourceConfigOption();
	void RefreshRuntime();
	Task RegisterChildVM_Task(char* path, char* name, HostSystem host);
	void UpdateChildResourceConfiguration(int sizespec, vw1__ResourceConfigSpec**  spec);
	void UpdateConfig(char* name, vw1__ResourceConfigSpec*  config);

public:
	// Property getter
	std::vector<vw1__ResourceConfigSpec> get_childConfiguration() const;
	vw1__ResourceConfigSpec get_config() const;
	ComputeResource get_owner() const;
	std::vector<ResourcePool> get_resourcePool() const;
	vw1__ResourcePoolRuntimeInfo get_runtime() const;
	vw1__ResourcePoolSummary get_summary() const;
	std::vector<VirtualMachine> get_vm() const;
};

class ScheduledTask : public ExtensibleManagedObject
{
public:
	ScheduledTask(vw1__ManagedObjectReference* mor = 0);
	virtual ~ScheduledTask();

	static const char* type;
	void ReconfigureScheduledTask(vw1__ScheduledTaskSpec*  spec);
	void RemoveScheduledTask();
	void RunScheduledTask();

public:
	// Property getter
	vw1__ScheduledTaskInfo get_info() const;
};

class ScheduledTaskManager : public mor_handle
{
public:
	ScheduledTaskManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~ScheduledTaskManager();

	static const char* type;
	ScheduledTask CreateObjectScheduledTask(vw1__ManagedObjectReference*  obj, vw1__ScheduledTaskSpec*  spec);
	ScheduledTask CreateScheduledTask(ManagedEntity entity, vw1__ScheduledTaskSpec*  spec);
	std::vector<ScheduledTask> RetrieveEntityScheduledTask(ManagedEntity entity);
	std::vector<ScheduledTask> RetrieveObjectScheduledTask(vw1__ManagedObjectReference*  obj);

public:
	// Property getter
	vw1__ScheduledTaskDescription get_description() const;
	std::vector<ScheduledTask> get_scheduledTask() const;
};

class SearchIndex : public mor_handle
{
public:
	SearchIndex(vw1__ManagedObjectReference* mor = 0);
	virtual ~SearchIndex();

	static const char* type;
	std::vector<ManagedEntity> FindAllByDnsName(Datacenter datacenter, char* dnsName, bool vmSearch);
	std::vector<ManagedEntity> FindAllByIp(Datacenter datacenter, char* ip, bool vmSearch);
	std::vector<ManagedEntity> FindAllByUuid(Datacenter datacenter, char* uuid, bool vmSearch, bool*  instanceUuid);
	VirtualMachine FindByDatastorePath(Datacenter datacenter, char* path);
	ManagedEntity FindByDnsName(Datacenter datacenter, char* dnsName, bool vmSearch);
	ManagedEntity FindByInventoryPath(char* inventoryPath);
	ManagedEntity FindByIp(Datacenter datacenter, char* ip, bool vmSearch);
	ManagedEntity FindByUuid(Datacenter datacenter, char* uuid, bool vmSearch, bool*  instanceUuid);
	ManagedEntity FindChild(ManagedEntity entity, char* name);

public:
	// Property getter
};

class ServiceInstance : public mor_handle
{
public:
	ServiceInstance(vw1__ManagedObjectReference* mor = 0);
	virtual ~ServiceInstance();

	static const char* type;
	time_t CurrentTime();
	std::vector<vw1__HostVMotionCompatibility> QueryVMotionCompatibility(VirtualMachine vm, int sizehost, HostSystem* host, int sizecompatibility, char** compatibility);
	std::vector<vw1__ProductComponentInfo> RetrieveProductComponents();
	vw1__ServiceContent RetrieveServiceContent();
	std::vector<vw1__Event> ValidateMigration(int sizevm, VirtualMachine* vm, vw1__VirtualMachinePowerState*  state, int sizetestType, char** testType, ResourcePool pool, HostSystem host);

public:
	// Property getter
	vw1__Capability get_capability() const;
	vw1__ServiceContent get_content() const;
	time_t get_serverClock() const;
};

class SessionManager : public mor_handle
{
public:
	SessionManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~SessionManager();

	static const char* type;
	std::string AcquireCloneTicket();
	vw1__SessionManagerGenericServiceTicket AcquireGenericServiceTicket(vw1__SessionManagerServiceRequestSpec*  spec);
	vw1__SessionManagerLocalTicket AcquireLocalTicket(char* userName);
	vw1__UserSession CloneSession(char* cloneTicket);
	vw1__UserSession ImpersonateUser(char* userName, char* locale);
	vw1__UserSession Login(char* userName, char* password, char* locale);
	vw1__UserSession LoginBySSPI(char* base64Token, char* locale);
	vw1__UserSession LoginExtensionByCertificate(char* extensionKey, char* locale);
	vw1__UserSession LoginExtensionBySubjectName(char* extensionKey, char* locale);
	void Logout();
	bool SessionIsActive(char* sessionID, char* userName);
	void SetLocale(char* locale);
	void TerminateSession(int sizesessionId, char** sessionId);
	void UpdateServiceMessage(char* message);

public:
	// Property getter
	vw1__UserSession get_currentSession() const;
	std::string get_defaultLocale() const;
	std::string get_message() const;
	std::vector<std::string> get_messageLocaleList() const;
	std::vector<vw1__UserSession> get_sessionList() const;
	std::vector<std::string> get_supportedLocaleList() const;
};

class StorageResourceManager : public mor_handle
{
public:
	StorageResourceManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~StorageResourceManager();

	static const char* type;
	Task ApplyStorageDrsRecommendation_Task(int sizekey, char** key);
	Task ApplyStorageDrsRecommendationToPod_Task(StoragePod pod, char* key);
	void CancelStorageDrsRecommendation(int sizekey, char** key);
	Task ConfigureDatastoreIORM_Task(Datastore datastore, vw1__StorageIORMConfigSpec*  spec);
	Task ConfigureStorageDrsForPod_Task(StoragePod pod, vw1__StorageDrsConfigSpec*  spec, bool modify);
	vw1__StorageIORMConfigOption QueryIORMConfigOption(HostSystem host);
	vw1__StoragePlacementResult RecommendDatastores(vw1__StoragePlacementSpec*  storageSpec);
	void RefreshStorageDrsRecommendation(StoragePod pod);

public:
	// Property getter
};

class Task : public ExtensibleManagedObject
{
public:
	Task(vw1__ManagedObjectReference* mor = 0);
	virtual ~Task();

	static const char* type;
	void CancelTask();
	void SetTaskDescription(vw1__LocalizableMessage*  description);
	void SetTaskState(vw1__TaskInfoState state, anyType*  result, vw1__LocalizedMethodFault*  fault);
	void UpdateProgress(int percentDone);

public:
	// Property getter
	vw1__TaskInfo get_info() const;
};

class TaskHistoryCollector : public HistoryCollector
{
public:
	TaskHistoryCollector(vw1__ManagedObjectReference* mor = 0);
	virtual ~TaskHistoryCollector();

	static const char* type;
	std::vector<vw1__TaskInfo> ReadNextTasks(int maxCount);
	std::vector<vw1__TaskInfo> ReadPreviousTasks(int maxCount);

public:
	// Property getter
	std::vector<vw1__TaskInfo> get_latestPage() const;
};

class TaskManager : public mor_handle
{
public:
	TaskManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~TaskManager();

	static const char* type;
	TaskHistoryCollector CreateCollectorForTasks(vw1__TaskFilterSpec*  filter);
	vw1__TaskInfo CreateTask(vw1__ManagedObjectReference*  obj, char* taskTypeId, char* initiatedBy, bool cancelable, char* parentTaskKey);

public:
	// Property getter
	vw1__TaskDescription get_description() const;
	int get_maxCollector() const;
	std::vector<Task> get_recentTask() const;
};

class UserDirectory : public mor_handle
{
public:
	UserDirectory(vw1__ManagedObjectReference* mor = 0);
	virtual ~UserDirectory();

	static const char* type;
	std::vector<vw1__UserSearchResult> RetrieveUserGroups(char* domain, char* searchStr, char* belongsToGroup, char* belongsToUser, bool exactMatch, bool findUsers, bool findGroups);

public:
	// Property getter
	std::vector<std::string> get_domainList() const;
};

class View : public mor_handle
{
public:
	View(vw1__ManagedObjectReference* mor = 0);
	virtual ~View();

	static const char* type;
	void DestroyView();

public:
	// Property getter
};

class ViewManager : public mor_handle
{
public:
	ViewManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~ViewManager();

	static const char* type;
	ContainerView CreateContainerView(ManagedEntity container, int sizetype, char** type, bool recursive);
	InventoryView CreateInventoryView();
	ListView CreateListView(int sizeobj, vw1__ManagedObjectReference**  obj);
	ListView CreateListViewFromView(View view);

public:
	// Property getter
	std::vector<View> get_viewList() const;
};

class VirtualApp : public ResourcePool
{
public:
	VirtualApp(vw1__ManagedObjectReference* mor = 0);
	virtual ~VirtualApp();

	static const char* type;
	Task CloneVApp_Task(char* name, ResourcePool target, vw1__VAppCloneSpec*  spec);
	HttpNfcLease ExportVApp();
	Task PowerOffVApp_Task(bool force);
	Task PowerOnVApp_Task();
	Task SuspendVApp_Task();
	Task unregisterVApp_Task();
	void UpdateLinkedChildren(int sizeaddChangeSet, vw1__VirtualAppLinkInfo**  addChangeSet, int sizeremoveSet, ManagedEntity* removeSet);
	void UpdateVAppConfig(vw1__VAppConfigSpec*  spec);

public:
	// Property getter
	std::vector<vw1__VirtualAppLinkInfo> get_childLink() const;
	std::vector<Datastore> get_datastore() const;
	std::vector<Network> get_network() const;
	Folder get_parentFolder() const;
	ManagedEntity get_parentVApp() const;
	vw1__VAppConfigInfo get_vAppConfig() const;
};

class VirtualDiskManager : public mor_handle
{
public:
	VirtualDiskManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~VirtualDiskManager();

	static const char* type;
	Task CopyVirtualDisk_Task(char* sourceName, Datacenter sourceDatacenter, char* destName, Datacenter destDatacenter, vw1__VirtualDiskSpec*  destSpec, bool*  force);
	Task CreateVirtualDisk_Task(char* name, Datacenter datacenter, vw1__VirtualDiskSpec*  spec);
	Task DefragmentVirtualDisk_Task(char* name, Datacenter datacenter);
	Task DeleteVirtualDisk_Task(char* name, Datacenter datacenter);
	Task EagerZeroVirtualDisk_Task(char* name, Datacenter datacenter);
	Task ExtendVirtualDisk_Task(char* name, Datacenter datacenter, __int64 newCapacityKb, bool*  eagerZero);
	Task InflateVirtualDisk_Task(char* name, Datacenter datacenter);
	Task MoveVirtualDisk_Task(char* sourceName, Datacenter sourceDatacenter, char* destName, Datacenter destDatacenter, bool*  force);
	int QueryVirtualDiskFragmentation(char* name, Datacenter datacenter);
	vw1__HostDiskDimensionsChs QueryVirtualDiskGeometry(char* name, Datacenter datacenter);
	std::string QueryVirtualDiskUuid(char* name, Datacenter datacenter);
	void SetVirtualDiskUuid(char* name, Datacenter datacenter, char* uuid);
	Task ShrinkVirtualDisk_Task(char* name, Datacenter datacenter, bool*  copy);
	Task ZeroFillVirtualDisk_Task(char* name, Datacenter datacenter);

public:
	// Property getter
};

class VirtualMachine : public ManagedEntity
{
public:
	VirtualMachine(vw1__ManagedObjectReference* mor = 0);
	virtual ~VirtualMachine();

	static const char* type;
	vw1__VirtualMachineMksTicket AcquireMksTicket();
	vw1__VirtualMachineTicket AcquireTicket(char* ticketType);
	void AnswerVM(char* questionId, char* answerChoice);
	void CheckCustomizationSpec(vw1__CustomizationSpec*  spec);
	Task CloneVM_Task(Folder folder, char* name, vw1__VirtualMachineCloneSpec*  spec);
	Task ConsolidateVMDisks_Task();
	Task CreateScreenshot_Task();
	Task CreateSecondaryVM_Task(HostSystem host);
	Task CreateSnapshot_Task(char* name, char* description, bool memory, bool quiesce);
	Task CustomizeVM_Task(vw1__CustomizationSpec*  spec);
	void DefragmentAllDisks();
	Task DisableSecondaryVM_Task(VirtualMachine vm);
	Task EnableSecondaryVM_Task(VirtualMachine vm, HostSystem host);
	Task EstimateStorageForConsolidateSnapshots_Task();
	HttpNfcLease ExportVm();
	std::string ExtractOvfEnvironment();
	Task MakePrimaryVM_Task(VirtualMachine vm);
	void MarkAsTemplate();
	void MarkAsVirtualMachine(ResourcePool pool, HostSystem host);
	Task MigrateVM_Task(ResourcePool pool, HostSystem host, vw1__VirtualMachineMovePriority priority, vw1__VirtualMachinePowerState*  state);
	void MountToolsInstaller();
	Task PowerOffVM_Task();
	Task PowerOnVM_Task(HostSystem host);
	Task PromoteDisks_Task(bool unlink, int sizedisks, vw1__VirtualDisk**  disks);
	vw1__DiskChangeInfo QueryChangedDiskAreas(VirtualMachineSnapshot snapshot, int deviceKey, __int64 startOffset, char* changeId);
	std::vector<vw1__LocalizedMethodFault> QueryFaultToleranceCompatibility();
	std::vector<std::string> QueryUnownedFiles();
	void RebootGuest();
	Task ReconfigVM_Task(vw1__VirtualMachineConfigSpec*  spec);
	void RefreshStorageInfo();
	Task reloadVirtualMachineFromPath_Task(char* configurationPath);
	Task RelocateVM_Task(vw1__VirtualMachineRelocateSpec*  spec, vw1__VirtualMachineMovePriority*  priority);
	Task RemoveAllSnapshots_Task(bool*  consolidate);
	void ResetGuestInformation();
	Task ResetVM_Task();
	Task RevertToCurrentSnapshot_Task(HostSystem host, bool*  suppressPowerOn);
	void SetDisplayTopology(int sizedisplays, vw1__VirtualMachineDisplayTopology**  displays);
	void SetScreenResolution(int width, int height);
	void ShutdownGuest();
	void StandbyGuest();
	Task StartRecording_Task(char* name, char* description);
	Task StartReplaying_Task(VirtualMachineSnapshot replaySnapshot);
	Task StopRecording_Task();
	Task StopReplaying_Task();
	Task SuspendVM_Task();
	Task TerminateFaultTolerantVM_Task(VirtualMachine vm);
	Task TurnOffFaultToleranceForVM_Task();
	void UnmountToolsInstaller();
	void UnregisterVM();
	Task UpgradeTools_Task(char* installerOptions);
	Task UpgradeVM_Task(char* version);

public:
	// Property getter
	vw1__VirtualMachineCapability get_capability() const;
	vw1__VirtualMachineConfigInfo get_config() const;
	std::vector<Datastore> get_datastore() const;
	EnvironmentBrowser get_environmentBrowser() const;
	vw1__GuestInfo get_guest() const;
	vw1__ManagedEntityStatus get_guestHeartbeatStatus() const;
	vw1__VirtualMachineFileLayout get_layout() const;
	vw1__VirtualMachineFileLayoutEx get_layoutEx() const;
	std::vector<Network> get_network() const;
	ManagedEntity get_parentVApp() const;
	vw1__ResourceConfigSpec get_resourceConfig() const;
	ResourcePool get_resourcePool() const;
	std::vector<VirtualMachineSnapshot> get_rootSnapshot() const;
	vw1__VirtualMachineRuntimeInfo get_runtime() const;
	vw1__VirtualMachineSnapshotInfo get_snapshot() const;
	vw1__VirtualMachineStorageInfo get_storage() const;
	vw1__VirtualMachineSummary get_summary() const;
};

class VirtualMachineCompatibilityChecker : public mor_handle
{
public:
	VirtualMachineCompatibilityChecker(vw1__ManagedObjectReference* mor = 0);
	virtual ~VirtualMachineCompatibilityChecker();

	static const char* type;
	Task CheckCompatibility_Task(VirtualMachine vm, HostSystem host, ResourcePool pool, int sizetestType, char** testType);

public:
	// Property getter
};

class VirtualMachineProvisioningChecker : public mor_handle
{
public:
	VirtualMachineProvisioningChecker(vw1__ManagedObjectReference* mor = 0);
	virtual ~VirtualMachineProvisioningChecker();

	static const char* type;
	Task CheckMigrate_Task(VirtualMachine vm, HostSystem host, ResourcePool pool, vw1__VirtualMachinePowerState*  state, int sizetestType, char** testType);
	Task CheckRelocate_Task(VirtualMachine vm, vw1__VirtualMachineRelocateSpec*  spec, int sizetestType, char** testType);
	Task QueryVMotionCompatibilityEx_Task(int sizevm, VirtualMachine* vm, int sizehost, HostSystem* host);

public:
	// Property getter
};

class VirtualMachineSnapshot : public ExtensibleManagedObject
{
public:
	VirtualMachineSnapshot(vw1__ManagedObjectReference* mor = 0);
	virtual ~VirtualMachineSnapshot();

	static const char* type;
	Task RemoveSnapshot_Task(bool removeChildren, bool*  consolidate);
	void RenameSnapshot(char* name, char* description);
	Task RevertToSnapshot_Task(HostSystem host, bool*  suppressPowerOn);

public:
	// Property getter
	std::vector<VirtualMachineSnapshot> get_childSnapshot() const;
	vw1__VirtualMachineConfigInfo get_config() const;
};

class VirtualizationManager : public mor_handle
{
public:
	VirtualizationManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~VirtualizationManager();

	static const char* type;

public:
	// Property getter
};

class Alarm : public ExtensibleManagedObject
{
public:
	Alarm(vw1__ManagedObjectReference* mor = 0);
	virtual ~Alarm();

	static const char* type;
	void ReconfigureAlarm(vw1__AlarmSpec*  spec);
	void RemoveAlarm();

public:
	// Property getter
	vw1__AlarmInfo get_info() const;
};

class ClusterProfile : public Profile
{
public:
	ClusterProfile(vw1__ManagedObjectReference* mor = 0);
	virtual ~ClusterProfile();

	static const char* type;
	void UpdateClusterProfile(vw1__ClusterProfileConfigSpec*  config);

public:
	// Property getter
};

class ClusterProfileManager : public ProfileManager
{
public:
	ClusterProfileManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~ClusterProfileManager();

	static const char* type;

public:
	// Property getter
};

class ComputeResource : public ManagedEntity
{
public:
	ComputeResource(vw1__ManagedObjectReference* mor = 0);
	virtual ~ComputeResource();

	static const char* type;
	Task ReconfigureComputeResource_Task(vw1__ComputeResourceConfigSpec*  spec, bool modify);

public:
	// Property getter
	vw1__ComputeResourceConfigInfo get_configurationEx() const;
	std::vector<Datastore> get_datastore() const;
	EnvironmentBrowser get_environmentBrowser() const;
	std::vector<HostSystem> get_host() const;
	std::vector<Network> get_network() const;
	ResourcePool get_resourcePool() const;
	vw1__ComputeResourceSummary get_summary() const;
};

class Datacenter : public ManagedEntity
{
public:
	Datacenter(vw1__ManagedObjectReference* mor = 0);
	virtual ~Datacenter();

	static const char* type;
	Task PowerOnMultiVM_Task(int sizevm, VirtualMachine* vm, int sizeoption, vw1__OptionValue**  option);
	vw1__HostConnectInfo QueryConnectionInfo(char* hostname, int port, char* username, char* password, char* sslThumbprint);

public:
	// Property getter
	std::vector<Datastore> get_datastore() const;
	Folder get_datastoreFolder() const;
	Folder get_hostFolder() const;
	std::vector<Network> get_network() const;
	Folder get_networkFolder() const;
	Folder get_vmFolder() const;
};

class Datastore : public ManagedEntity
{
public:
	Datastore(vw1__ManagedObjectReference* mor = 0);
	virtual ~Datastore();

	static const char* type;
	vw1__StoragePlacementResult DatastoreEnterMaintenanceMode();
	Task DatastoreExitMaintenanceMode_Task();
	void DestroyDatastore();
	void RefreshDatastore();
	void RefreshDatastoreStorageInfo();
	void RenameDatastore(char* newName);
	Task UpdateVirtualMachineFiles_Task(int sizemountPathDatastoreMapping, vw1__DatastoreMountPathDatastorePair**  mountPathDatastoreMapping);

public:
	// Property getter
	HostDatastoreBrowser get_browser() const;
	vw1__DatastoreCapability get_capability() const;
	std::vector<vw1__DatastoreHostMount> get_host() const;
	vw1__DatastoreInfo get_info() const;
	vw1__StorageIORMInfo get_iormConfiguration() const;
	vw1__DatastoreSummary get_summary() const;
	std::vector<VirtualMachine> get_vm() const;
};

class DistributedVirtualPortgroup : public Network
{
public:
	DistributedVirtualPortgroup(vw1__ManagedObjectReference* mor = 0);
	virtual ~DistributedVirtualPortgroup();

	static const char* type;
	Task ReconfigureDVPortgroup_Task(vw1__DVPortgroupConfigSpec*  spec);

public:
	// Property getter
	vw1__DVPortgroupConfigInfo get_config() const;
	std::string get_key() const;
	std::vector<std::string> get_portKeys() const;
};

class DistributedVirtualSwitch : public ManagedEntity
{
public:
	DistributedVirtualSwitch(vw1__ManagedObjectReference* mor = 0);
	virtual ~DistributedVirtualSwitch();

	static const char* type;
	Task AddDVPortgroup_Task(int sizespec, vw1__DVPortgroupConfigSpec**  spec);
	void AddNetworkResourcePool(int sizeconfigSpec, vw1__DVSNetworkResourcePoolConfigSpec**  configSpec);
	void EnableNetworkResourceManagement(bool enable);
	std::vector<std::string> FetchDVPortKeys(vw1__DistributedVirtualSwitchPortCriteria*  criteria);
	std::vector<vw1__DistributedVirtualPort> FetchDVPorts(vw1__DistributedVirtualSwitchPortCriteria*  criteria);
	Task MergeDvs_Task(DistributedVirtualSwitch dvs);
	Task MoveDVPort_Task(int sizeportKey, char** portKey, char* destinationPortgroupKey);
	Task PerformDvsProductSpecOperation_Task(char* operation, vw1__DistributedVirtualSwitchProductSpec*  productSpec);
	std::vector<int> QueryUsedVlanIdInDvs();
	Task ReconfigureDVPort_Task(int sizeport, vw1__DVPortConfigSpec**  port);
	Task ReconfigureDvs_Task(vw1__DVSConfigSpec*  spec);
	Task RectifyDvsHost_Task(int sizehosts, HostSystem* hosts);
	void RefreshDVPortState(int sizeportKeys, char** portKeys);
	void RemoveNetworkResourcePool(int sizekey, char** key);
	void UpdateDvsCapability(vw1__DVSCapability*  capability);
	void UpdateNetworkResourcePool(int sizeconfigSpec, vw1__DVSNetworkResourcePoolConfigSpec**  configSpec);

public:
	// Property getter
	vw1__DVSCapability get_capability() const;
	vw1__DVSConfigInfo get_config() const;
	std::vector<vw1__DVSNetworkResourcePool> get_networkResourcePool() const;
	std::vector<DistributedVirtualPortgroup> get_portgroup() const;
	vw1__DVSSummary get_summary() const;
	std::string get_uuid() const;
};

class EventHistoryCollector : public HistoryCollector
{
public:
	EventHistoryCollector(vw1__ManagedObjectReference* mor = 0);
	virtual ~EventHistoryCollector();

	static const char* type;
	std::vector<vw1__Event> ReadNextEvents(int maxCount);
	std::vector<vw1__Event> ReadPreviousEvents(int maxCount);

public:
	// Property getter
	std::vector<vw1__Event> get_latestPage() const;
};

class Folder : public ManagedEntity
{
public:
	Folder(vw1__ManagedObjectReference* mor = 0);
	virtual ~Folder();

	static const char* type;
	Task AddStandaloneHost_Task(vw1__HostConnectSpec*  spec, vw1__ComputeResourceConfigSpec*  compResSpec, bool addConnected, char* license);
	ClusterComputeResource CreateCluster(char* name, vw1__ClusterConfigSpec*  spec);
	ClusterComputeResource CreateClusterEx(char* name, vw1__ClusterConfigSpecEx*  spec);
	Datacenter CreateDatacenter(char* name);
	Task CreateDVS_Task(vw1__DVSCreateSpec*  spec);
	Folder CreateFolder(char* name);
	StoragePod CreateStoragePod(char* name);
	Task CreateVM_Task(vw1__VirtualMachineConfigSpec*  config, ResourcePool pool, HostSystem host);
	Task MoveIntoFolder_Task(int sizelist, ManagedEntity* list);
	Task RegisterVM_Task(char* path, char* name, bool asTemplate, ResourcePool pool, HostSystem host);
	Task UnregisterAndDestroy_Task();

public:
	// Property getter
	std::vector<ManagedEntity> get_childEntity() const;
	std::vector<std::string> get_childType() const;
};

class HostActiveDirectoryAuthentication : public HostDirectoryStore
{
public:
	HostActiveDirectoryAuthentication(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostActiveDirectoryAuthentication();

	static const char* type;
	Task ImportCertificateForCAM_Task(char* certPath, char* camServer);
	Task JoinDomain_Task(char* domainName, char* userName, char* password);
	Task JoinDomainWithCAM_Task(char* domainName, char* camServer);
	Task LeaveCurrentDomain_Task(bool force);

public:
	// Property getter
};

class HostProfile : public Profile
{
public:
	HostProfile(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostProfile();

	static const char* type;
	vw1__ProfileExecuteResult ExecuteHostProfile(HostSystem host, int sizedeferredParam, vw1__ProfileDeferredPolicyOptionParameter**  deferredParam);
	void UpdateHostProfile(vw1__HostProfileConfigSpec*  config);
	void UpdateReferenceHost(HostSystem host);

public:
	// Property getter
	HostSystem get_referenceHost() const;
};

class HostProfileManager : public ProfileManager
{
public:
	HostProfileManager(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostProfileManager();

	static const char* type;
	Task ApplyHostConfig_Task(HostSystem host, vw1__HostConfigSpec*  configSpec, int sizeuserInput, vw1__ProfileDeferredPolicyOptionParameter**  userInput);
	Task CheckAnswerFileStatus_Task(int sizehost, HostSystem* host);
	vw1__ApplyProfile CreateDefaultProfile(char* profileType, char* profileTypeName, Profile profile);
	Task ExportAnswerFile_Task(HostSystem host);
	vw1__HostProfileManagerConfigTaskList GenerateConfigTaskList(vw1__HostConfigSpec*  configSpec, HostSystem host);
	std::vector<vw1__AnswerFileStatusResult> QueryAnswerFileStatus(int sizehost, HostSystem* host);
	std::vector<vw1__ProfileMetadata> QueryHostProfileMetadata(int sizeprofileName, char** profileName, Profile profile);
	vw1__ProfileProfileStructure QueryProfileStructure(Profile profile);
	vw1__AnswerFile RetrieveAnswerFile(HostSystem host);
	Task UpdateAnswerFile_Task(HostSystem host, vw1__AnswerFileCreateSpec*  configSpec);

public:
	// Property getter
};

class HostSystem : public ManagedEntity
{
public:
	HostSystem(vw1__ManagedObjectReference* mor = 0);
	virtual ~HostSystem();

	static const char* type;
	vw1__HostServiceTicket AcquireCimServicesTicket();
	Task DisconnectHost_Task();
	void EnterLockdownMode();
	Task EnterMaintenanceMode_Task(int timeout, bool*  evacuatePoweredOffVms);
	void ExitLockdownMode();
	Task ExitMaintenanceMode_Task(int timeout);
	Task PowerDownHostToStandBy_Task(int timeoutSec, bool*  evacuatePoweredOffVms);
	Task PowerUpHostFromStandBy_Task(int timeoutSec);
	vw1__HostConnectInfo QueryHostConnectionInfo();
	__int64 QueryMemoryOverhead(__int64 memorySize, int*  videoRamSize, int numVcpus);
	__int64 QueryMemoryOverheadEx(vw1__VirtualMachineConfigInfo*  vmConfigInfo);
	Task RebootHost_Task(bool force);
	Task ReconfigureHostForDAS_Task();
	Task ReconnectHost_Task(vw1__HostConnectSpec*  cnxSpec, vw1__HostSystemReconnectSpec*  reconnectSpec);
	__int64 RetrieveHardwareUptime();
	Task ShutdownHost_Task(bool force);
	void UpdateFlags(vw1__HostFlagInfo*  flagInfo);
	void UpdateIpmi(vw1__HostIpmiInfo*  ipmiInfo);
	void UpdateSystemResources(vw1__HostSystemResourceInfo*  resourceInfo);

public:
	// Property getter
	vw1__HostCapability get_capability() const;
	vw1__HostConfigInfo get_config() const;
	vw1__HostConfigManager get_configManager() const;
	std::vector<Datastore> get_datastore() const;
	HostDatastoreBrowser get_datastoreBrowser() const;
	vw1__HostHardwareInfo get_hardware() const;
	vw1__HostLicensableResourceInfo get_licensableResource() const;
	std::vector<Network> get_network() const;
	vw1__HostRuntimeInfo get_runtime() const;
	vw1__HostListSummary get_summary() const;
	vw1__HostSystemResourceInfo get_systemResources() const;
	std::vector<VirtualMachine> get_vm() const;
};

class ManagedObjectView : public View
{
public:
	ManagedObjectView(vw1__ManagedObjectReference* mor = 0);
	virtual ~ManagedObjectView();

	static const char* type;

public:
	// Property getter
	std::vector<View> get_view() const;
};

class StoragePod : public Folder
{
public:
	StoragePod(vw1__ManagedObjectReference* mor = 0);
	virtual ~StoragePod();

	static const char* type;

public:
	// Property getter
	vw1__PodStorageDrsEntry get_podStorageDrsEntry() const;
	vw1__StoragePodSummary get_summary() const;
};

class VmwareDistributedVirtualSwitch : public DistributedVirtualSwitch
{
public:
	VmwareDistributedVirtualSwitch(vw1__ManagedObjectReference* mor = 0);
	virtual ~VmwareDistributedVirtualSwitch();

	static const char* type;

public:
	// Property getter
};

class ClusterComputeResource : public ComputeResource
{
public:
	ClusterComputeResource(vw1__ManagedObjectReference* mor = 0);
	virtual ~ClusterComputeResource();

	static const char* type;
	Task AddHost_Task(vw1__HostConnectSpec*  spec, bool asConnected, ResourcePool resourcePool, char* license);
	void ApplyRecommendation(char* key);
	void CancelRecommendation(char* key);
	vw1__ClusterEnterMaintenanceResult ClusterEnterMaintenanceMode(int sizehost, HostSystem* host, int sizeoption, vw1__OptionValue**  option);
	Task MoveHostInto_Task(HostSystem host, ResourcePool resourcePool);
	Task MoveInto_Task(int sizehost, HostSystem* host);
	std::vector<vw1__ClusterHostRecommendation> RecommendHostsForVm(VirtualMachine vm, ResourcePool pool);
	Task ReconfigureCluster_Task(vw1__ClusterConfigSpec*  spec, bool modify);
	void RefreshRecommendation();
	vw1__ClusterDasAdvancedRuntimeInfo RetrieveDasAdvancedRuntimeInfo();

public:
	// Property getter
	std::vector<vw1__ClusterActionHistory> get_actionHistory() const;
	vw1__ClusterConfigInfo get_configuration() const;
	std::vector<vw1__ClusterDrsFaults> get_drsFault() const;
	std::vector<vw1__ClusterDrsRecommendation> get_drsRecommendation() const;
	std::vector<vw1__ClusterDrsMigration> get_migrationHistory() const;
	std::vector<vw1__ClusterRecommendation> get_recommendation() const;
};

class ContainerView : public ManagedObjectView
{
public:
	ContainerView(vw1__ManagedObjectReference* mor = 0);
	virtual ~ContainerView();

	static const char* type;

public:
	// Property getter
	ManagedEntity get_container() const;
	bool get_recursive() const;
	std::vector<std::string> get_type() const;
};

class InventoryView : public ManagedObjectView
{
public:
	InventoryView(vw1__ManagedObjectReference* mor = 0);
	virtual ~InventoryView();

	static const char* type;
	std::vector<ManagedEntity> CloseInventoryViewFolder(int sizeentity, ManagedEntity* entity);
	std::vector<ManagedEntity> OpenInventoryViewFolder(int sizeentity, ManagedEntity* entity);

public:
	// Property getter
};

class ListView : public ManagedObjectView
{
public:
	ListView(vw1__ManagedObjectReference* mor = 0);
	virtual ~ListView();

	static const char* type;
	std::vector<vw1__ManagedObjectReference> ModifyListView(int sizeadd, vw1__ManagedObjectReference**  add, int sizeremove, vw1__ManagedObjectReference**  remove);
	std::vector<vw1__ManagedObjectReference> ResetListView(int sizeobj, vw1__ManagedObjectReference**  obj);
	void ResetListViewFromView(View view);

public:
	// Property getter
};

class Vim
{
	binding_proxy* _proxy;
	vw1__ManagedObjectReference _mor;
	ServiceInstance* _si;
	Vim(binding_proxy* proxy = 0) : _proxy(proxy) {}
	friend Vim& VimInstance(binding_proxy* proxy);

public:
	ServiceInstance& get_service_instance()
	{
		static char type[] =  "ServiceInstance";
		static char item[] =  "ServiceInstance";
		if (!_si)
		{
			_mor.type = type;
			_mor.__item = item;
			_si = new ServiceInstance(&_mor);
		}
		return *_si;
	}
	binding_proxy* proxy() { return _proxy; }
};

Vim& VimInstance(binding_proxy* proxy = 0);
PropertyCollector& get_collector();
} // namespace vim
#endif // VIM_WRAPPER_H

