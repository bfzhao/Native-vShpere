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
#ifndef HELPER_H
#define HELPER_H

#include <exception>
#include <sstream>
#include <iomanip>

#include <Windows.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <process.h>

// reference count add-in, no COW supported
class ref_count
{
public:
	ref_count() : _count(new int(1)) {}
	~ref_count() { if (--*_count == 0) delete _count; }
	ref_count(const ref_count& u) : _count(u._count) { ++*_count; }
	bool attach(const ref_count& u)
	{
		bool need_release = false;
		++*u._count;
		if (--*_count == 0)
		{
			delete _count;
			need_release = true;
		}
		_count = u._count;
		return need_release;
	}
	bool only() const { return *_count == 1; }
	bool make_only()
	{
		if (only())
			return false;
		--*_count;
		_count = new int(1);
		return true;
	}
private:
	int* _count;
	ref_count& operator = (const ref_count& u);
};

// split a string into a list, leading, trailing and continuous separator is ignored
template <typename T>
inline std::vector<T> split(const T& src, typename T::value_type sep)
{
	std::vector<T> result;
	T::size_type start = 0;
	do 
	{
		start = src.find_first_not_of(sep, start);
		if (start == T::npos)
			break;

		T::size_type stop = src.find_first_of(sep, start);
		if (stop == T::npos)
		{
			result.push_back(T(src, start));
			break;
		}
		else
		{
			result.push_back(T(src, start, stop - start));
			start = stop + 1;
		}
	} while(start < src.size());
	return result;
}

namespace win32 {
// win32 exception root class
#define WIN_STD_EXCEPTION win32::sys_exception(__FILE__, __LINE__)
#define WIN_STD_EXCEPTION_WITH_ERR(code) win32::sys_exception(__FILE__, __LINE__, code)
class sys_exception : public std::exception
{
	DWORD _last_error;
	char* _message;
	mutable std::string _err_info;
	const char* _file;
	int _line_no;
public:
	sys_exception(const char* file, int line, DWORD error = 0) 
		: _last_error(error? error : GetLastError()), _message(0), _file(file), _line_no(line) {}
	virtual ~sys_exception() { LocalFree(_message); }
	const char* what() const
	{
		if (_err_info.size() == 0)
		{
			size_t sz = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL, _last_error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&_message,0, NULL);
			if (sz)
			{
				for (size_t i = sz - 1; isspace(_message[i]); --i)
				{
					if (_message[i] == '\r' || _message[i] == '\n')
						_message[i] = 0;
				}
			}
			std::stringstream ss;
			ss << "0x" << std::hex << _last_error << "(" << (_message? _message : "<UNKNOWN>")
				<< ") @ " << _file << ":" << std::dec <<_line_no;
			_err_info = ss.str();
		}
		return _err_info.c_str();
	}
	DWORD last_error() const { return _last_error; }
};

// general HANDLE wrapper
class sys_handle
{
public:
	explicit sys_handle(HANDLE handle = INVALID_HANDLE_VALUE) : _handle(handle) {}
	sys_handle& operator = (const sys_handle& rhs)
	{
		if (&rhs == this)
			return *this;

		if (_ref.attach(rhs._ref) && is_valid())
			CloseHandle(_handle);
		_handle = rhs._handle;
		return *this;
	}

	~sys_handle()
	{
		if (_ref.only() && is_valid())
			CloseHandle(_handle);
	}

	operator HANDLE() { return _handle; }
	operator HANDLE() const { return _handle; }
	HANDLE& get_raw_handle() { return _handle; }
	bool is_valid() const { return _handle != INVALID_HANDLE_VALUE; }
private:
	HANDLE _handle;
	ref_count _ref;
};

// Simple sync obj interface
class sync_obj
{
public:
	virtual void lock() = 0;
	virtual void unlock() = 0;
};

// critical section obj
class critical_section : public sync_obj
{
	CRITICAL_SECTION _cs;
public:
	critical_section() { InitializeCriticalSection(&_cs); }
	~critical_section() { DeleteCriticalSection(&_cs); }
	virtual void lock() { EnterCriticalSection(&_cs); }
	virtual void unlock() { LeaveCriticalSection(&_cs); }
};

// Notifier
class notifer
{
	sys_handle _e;
	bool _exist;

	notifer(HANDLE h) : _e(h), _exist(true) {}
public:
	notifer(const wchar_t* name = 0, LPSECURITY_ATTRIBUTES ea = 0) : _e(CreateEventW(ea, TRUE, FALSE, name)), _exist(false)
	{
		if (!_e)
			throw WIN_STD_EXCEPTION;
		else
			_exist = (GetLastError() == ERROR_ALREADY_EXISTS);
	}
	bool is_exist() const { return _exist; }
	void signal()
	{
		if (!SetEvent(_e))
			throw WIN_STD_EXCEPTION;
	}
	void reset()
	{
		if (!ResetEvent(_e))
			throw WIN_STD_EXCEPTION;
	}
	notifer get_copy_for_another_process(HANDLE process) const
	{
		win32::sys_handle cur_process(GetCurrentProcess());
		HANDLE copy;
		if (!DuplicateHandle(cur_process, _e, process, &copy, 0, FALSE, DUPLICATE_SAME_ACCESS))
			throw WIN_STD_EXCEPTION;
		return notifer(copy);
	}
	bool wait_signalled(DWORD dwMilliseconds = INFINITE) const
	{
		DWORD ret = WaitForSingleObject(_e, dwMilliseconds);
		if (ret == WAIT_OBJECT_0)
			return true;
		return false;
	}
};

// Scoped lock
class local_locker
{
	bool _owned;
	critical_section* _lock_obj;
	local_locker(const local_locker&);
	local_locker& operator =(const local_locker&);
public:
	local_locker(critical_section* locker = 0) : _lock_obj(locker)
	{
		_owned = false;
		if (!_lock_obj)
		{
			_lock_obj = new critical_section;
			_owned = true;
		}
		_lock_obj->lock();
	}
	~local_locker()
	{
		_lock_obj->unlock();
		if (_owned)
			delete _lock_obj;
	}
};

// windows thread
class win_thread
{
	std::string _exception_msg;
	unsigned int _id;
	void* _security;
	unsigned int _stack_size;
	HANDLE _handle;
	bool _force_kill;
	win_thread* _owner;
	notifer _stop_notifer;

public:
	win_thread(void *security = 0, unsigned stack_size = 0, bool force_kill_on_destroy = true) : 
	  _security(security), _stack_size(stack_size), _force_kill(force_kill_on_destroy), _owner(0)
	  {
		  _handle = reinterpret_cast<HANDLE>(_beginthreadex(_security, _stack_size, 
			  &win_thread::the_worker, static_cast<void*>(this), CREATE_SUSPENDED, &_id));
		  if (!_handle)
			  throw errno;
	  }
	  virtual ~win_thread()
	  {
		  signal_terminate();
		  if (!wait_finish(10 * 1000))
		  {
			  if (_force_kill)
				  force_kill();
		  }
	  }
	  void start() 
	  {
		  _owner = get_owner();
		  if (ResumeThread(_handle) == static_cast<DWORD>(-1))
			  throw WIN_STD_EXCEPTION;
	  }

	  void signal_terminate()
	  {
		  _stop_notifer.signal();
	  }
	  void force_kill(DWORD retcode = 0xffffffff)
	  {
		  TerminateThread(_handle, retcode);
	  }
	  bool should_terminate() const
	  {
		  return _stop_notifer.wait_signalled(0);
	  }
	  bool wait_finish(DWORD dwMilliseconds = INFINITE) const
	  {
		  DWORD ret = WaitForSingleObject(_handle, dwMilliseconds);
		  if (ret == WAIT_OBJECT_0)
			  return true;
		  return false;
	  }

	  bool is_finished() const
	  {
		  DWORD ret = WaitForSingleObject(_handle, 0);
		  if (ret == WAIT_OBJECT_0)
			  return true;
		  return false;
	  }

	  DWORD get_exit_code() const
	  {
		  assert(is_finished());
		  DWORD code = 0;
		  if (GetExitCodeThread(_handle, &code))
			  return code;
		  throw WIN_STD_EXCEPTION;
	  }

	  unsigned int id() const { return _id; }
	  operator HANDLE() const { return _handle; }
protected:
	virtual unsigned do_task() = 0;
	virtual win_thread* get_owner() = 0;

private:
	void set_exception_msg(const char* msg)
	{
		_exception_msg = msg;
	}
	static unsigned __stdcall the_worker(void* param)
	{
		assert(param);
		win_thread* the_thread = reinterpret_cast<win_thread*>(param);
		try
		{
			if (the_thread->_owner)
				return the_thread->_owner->do_task();
			else
				throw std::exception("no thread owner object specified");
		}
		catch (const std::exception& e)
		{
			the_thread->set_exception_msg(e.what());
		}
		catch (...)
		{
			the_thread->set_exception_msg("unknown exception from the thread");
		}
		return static_cast<unsigned>(-1);
	}
};
}
#endif // HELPER_H
