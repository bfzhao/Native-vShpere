//
// Copyright (c) 2011, Bingfeng Zhao . All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of EMC Inc. nor the names of its contributors
//       may be used to endorse or promote products derived from this
//       software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
#include "soapVimBindingProxy.h"
#include "vim_wrapper.h"
#include "vim_browser.h"
#include "helper.h"
using namespace vim;

namespace vim_browser {
bool is_higher_version_than(const char* ver)
{
	// TODO: The version of ESX you used now. should be detected automatically 
	if (strcmp(ver, "4.1") == 0)
		return false;
	return true;
}
}

size_t get_choose(const std::string& prompt)
{
	std::string choose;
	while (1)
	{
		std::cout << prompt << ": ";
		std::getline(std::cin, choose);

		std::vector<std::string> vs = split(choose, ' ');
		if (vs.size() == 0 || !isdigit(vs[0][0]))
			continue;

		if (vs.size() == 1)
		{
			int i = atoi(vs[0].c_str());
			if (i >= 0)
				return (i<<16);
		}
		else
		{
			int item = atoi(vs[0].c_str()) << 16;
			int sub_item = atoi(vs[1].c_str());
			if (item >= 0 && sub_item > 0)
				return item + sub_item;
		}
	}
}

void updater(const std::string& key, const std::string& value, void* context);
struct tuple {
	std::string name;
	std::string type;
	std::string value;
};

bool operator<(const tuple& lhs, const tuple& rhs) { return lhs.name < rhs.name; }
bool operator==(const tuple& lhs, const tuple& rhs) { return lhs.name == rhs.name; }
class cli_browser : public win32::win_thread
{
	vim::ServiceInstance _si;
	win32::notifer _notif;

	std::vector<tuple> _members;
	std::string _class_name;

	void handle_array(std::ostream& os, const std::string& value, size_t size)
	{
		if (value.empty())
			os << std::endl;
		else if (value[0] != '@' )
			os << value << std::endl;
		else
		{
			std::vector<std::string> vs = split<std::string>(value.data() + 1, '|');
			for (size_t i = 0; i < vs.size(); ++i)
			{
				if (i != 0)
					for (size_t j = 0; j < size; ++j) os << ' ';
				os << std::right << std::setw(2) << i + 1 << ". " << vs[i] << std::endl;
			}
			if (vs.size() == 0)
				os << std::endl;
		}
	}

public:
	cli_browser() : _si(VimInstance().get_service_instance()) {}
	void notify() { _notif.signal(); }
	void set_class_name(const std::string& name) { _class_name = name; }
	void add_member(const std::string& name, const std::string& type, const std::string& value)
	{
		tuple t = {name, type, value};
		_members.push_back(t);
	}

protected:
	virtual unsigned do_task()
	{
		try
		{
			std::vector<const vim_browser::closure*> browse_histroy;
			vim::ServiceInstance si = VimInstance().get_service_instance();
			const vim_browser::closure* c = new vim_browser::ServiceInstance_closure(si);
			c->browse(updater, this);
			while (1)
			{
				_notif.wait_signalled();
				auto local_copy = _members;
				_members.clear();

				size_t max_name = 0, max_type = 0;
				for (size_t i = 0; i < local_copy.size(); ++i)
				{
					if (local_copy[i].name.size() > max_name)
						max_name = local_copy[i].name.size();
					if (local_copy[i].type.size() > max_type)
						max_type = local_copy[i].type.size();
				}

				auto sorted_local_copy = local_copy;
				std::sort(sorted_local_copy.begin(), sorted_local_copy.end());
				std::cout << "** " << _class_name << " **" << std::endl;
				for (size_t i = 0; i < sorted_local_copy.size(); ++i)
				{
					std::cout << std::right << std::setw(2) << i + 1 << ". "
						<< std::left << std::setw(max_name + 1) << sorted_local_copy[i].name 
						<< std::left << std::setw(max_type + 1) << sorted_local_copy[i].type; 
					handle_array(std::cout, sorted_local_copy[i].value, 6 + max_name + max_type);
				}
				std::cout << std::endl;

				bool quit = false;
				while (1)
				{
					std::stringstream ss;
					ss << "<";
					for (auto i = browse_histroy.begin(); i != browse_histroy.end(); ++i)
						ss << '/' << (*i)->type();
					ss << '/' << c->type() << ">";

					size_t index = 0;
					do
					{
						index = get_choose(ss.str());
					}while ((index >> 16)> local_copy.size());

					if (index == 0)
					{
						if (browse_histroy.size() == 0)
						{
							quit = true;
							break;
						}
						delete c;
						c = *browse_histroy.rbegin();
						browse_histroy.pop_back();
						c->browse(updater, this);
						break;
					}
					else
					{
						size_t act_index = 0;
						for (size_t i = 0; i < local_copy.size(); ++i)
						{
							if (sorted_local_copy[(index >> 16) - 1] == local_copy[i])
							{
								act_index = i + 1;
								break;
							}
						}

						vim_browser::closure* nc = c->get_sub_closure((act_index << 16) + (index & 0xFFFF));
						if (nc)
						{
							browse_histroy.push_back(c);
							nc->browse(updater, this);
							c = nc;
							break;
						}
						else
						{
							std::cout << "Cannot browse this" << std::endl;
						}
					}
				}

				if (quit)
					break;
			}

			return 0;
		}
		catch (const std::string& err)
		{
			std::cout << "exception: " << err << std::endl;
			return 1;
		}
	}
	virtual win_thread* get_owner() { return this; }
};

void updater(const std::string& key, const std::string& value, void* context)
{
	static std::string name, type;
	cli_browser* browser = reinterpret_cast<cli_browser*>(context);
	if (key == "Begin")
		browser->set_class_name(value);
	else if (key == "Base")
		;
	else if (key == "End")
		browser->notify();
	else if (key == "Name")
		name = value;
	else if (key == "Type")
		type = value;
	else if (key == "Value")
		browser->add_member(name, type, value);
	else
		assert(!"should not be here");
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

		// browse the VIM object system
		cli_browser browser;
		browser.start();
		browser.wait_finish();

		sm.Logout();
		return 0;
	}
	catch (const std::string& err)
	{
		std::cout << "exception: " << err;
		return 1;
	}
}
