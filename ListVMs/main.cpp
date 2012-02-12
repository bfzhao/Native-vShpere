#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
#include "soapVimBindingProxy.h"
#include "vim_wrapper.h"
using namespace vim;
using namespace std;

void list_folder(const ManagedEntity& me);
void list_host(const HostSystem& hs)
{
	vector<VirtualMachine> vms = hs.get_vm();
	for (size_t i = 0; i < vms.size(); ++i)
	{
		VirtualMachine vm(vms[i]);
		cout << vm.get_name() << " (" << vm.get_runtime().powerState << ")\n";
	}
}
void list_computerresource(const ManagedEntity& me)
{
	ComputeResource cr(me);
	vector<HostSystem> hss = cr.get_host();
	for (size_t i = 0; i < hss.size(); ++i)
		list_host(hss[i]);
}
void list_datastore(const ManagedEntity& me)
{

}
void list_datacenter(const ManagedEntity& me)
{
	Datacenter dc(me);
	cout << "\tDataCenter: " << dc.get_name() << endl;
	std::vector<ManagedEntity> mes = dc.get_hostFolder().get_childEntity();
	for (size_t i = 0; i < mes.size(); ++i)
	{
		cout << i << ". " << mes[i].get_name() << ":" << endl;
		if (mes[i].is_type_of("Folder"))
			list_folder(mes[i]);
		else if (mes[i].is_type_of("ComputeResource"))
			list_computerresource(mes[i]);
		else
			list_datastore(mes[i]);
	}
}
void list_folder(const ManagedEntity& me)
{
	Folder fd(me);
	std::vector<ManagedEntity> mes = fd.get_childEntity();
	for (size_t i = 0; i < mes.size(); ++i)
	{
		cout << i << ". " << mes[i].get_name() << ":" << endl;
		if (mes[i].is_type_of("Datacenter"))
			list_datacenter(mes[i]);
		else if (mes[i].is_type_of("Folder"))
			list_folder(mes[i]);
		else if (mes[i].is_type_of("ComputeResource"))
			list_computerresource(mes[i]);
	}
}

int main(int argc, char* argv[])
{
	try
	{
		using namespace vim;
		char* ip = (argc > 1? argv[1] : "10.32.230.101");
		char* user = (argc > 2? argv[2] : "administrator");
		char* pwd = (argc > 3? argv[3] : "dangerous");
		binding_proxy proxy(ip, true);
		ServiceInstance si(VimInstance(&proxy).get_service_instance());
		vw1__ServiceContent sc = si.RetrieveServiceContent();
		SessionManager sm(sc.sessionManager);
		sm.Login(user, pwd, NULL);

		Folder rf(sc.rootFolder);
		std::vector<ManagedEntity> mes = rf.get_childEntity();
		for (size_t i = 0; i < mes.size(); ++i)
		{
			cout << i << ". " << mes[i].get_name() << ":" << endl;
			if (mes[i].is_type_of("Datacenter"))
				list_datacenter(mes[i]);
			else if (mes[i].is_type_of("Folder"))
				list_folder(mes[i]);
		}

		sm.Logout();
		return 0;
	}
	catch (const std::string& err)
	{
		std::cout << "exception: " << err;
		return 1;
	}
}