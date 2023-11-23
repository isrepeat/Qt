#pragma once
#include <thread>
#include <chrono>
#include <future>
#include <memory>
#include <utility>
#include <functional>
#include <type_traits>
#include <condition_variable>

using namespace std::chrono_literals;

namespace H {
	//
	// Timer
	//
	class Timer {
	public:
		template <typename Duration>
		static void Once(Duration timeout, std::function<void()> callback) { // Not thread safe callback call
			std::thread([=] {
				std::this_thread::sleep_for(timeout);
				callback();
				}).detach();
		}

		template <typename Duration>
		static bool Once(std::unique_ptr<std::future<void>>& futureToken, Duration timeout, std::function<void()> callback) { // Thread safe callback call
			if (futureToken)
				return false; // Guard from double set. Don't init new future until previous not finished.

			futureToken = std::make_unique<std::future<void>>(std::async(std::launch::async, [&futureToken, timeout, callback] {
				std::this_thread::sleep_for(timeout);
				callback();
				futureToken = nullptr;
				}));

			return true;
		}


		Timer() = default;
		Timer(std::chrono::milliseconds timeout, std::function<void()> callback, bool autoRestart = false) {
			Start(timeout, callback, autoRestart);
		}
		~Timer() {
			StopThread();
		}

		void Start(std::chrono::milliseconds timeout, std::function<void()> callback, bool autoRestart = false) {
			StopThread();
			{
				std::lock_guard lk1{ mx };
				this->timeout = timeout;
				this->callback = callback;
				this->autoRestart = autoRestart;
			}
			StartThread();
		}
		void Stop() {
			StopThread();
		}

		void Reset(std::chrono::milliseconds timeout) {
			StopThread();
			StartThread();
		}

	private:
		void StopThread() {
			stop = true;
			cv.notify_all();
			if (threadTimer.joinable())
				threadTimer.join();
		}

		void StartThread() {
			stop = false;
			threadTimer = std::thread([this] {
				std::lock_guard lk1{ mx }; // guard from changing timeout and callback
				while (!stop) {
					std::mutex tmpMx;
					std::unique_lock lk2{ tmpMx };
					if (!cv.wait_for(lk2, timeout, [this] { return static_cast<bool>(stop); })) {
						if (callback) {
							callback();
						}
					}
					if (!autoRestart) {
						stop = true;
						break;
					}
				}
				});
		}

	private:
		std::mutex mx;
		std::thread threadTimer;
		std::condition_variable cv;

		std::function<void()> callback;
		std::chrono::milliseconds timeout;

		std::atomic<bool> stop = false;
		std::atomic<bool> autoRestart = false;
	};


	//
	// Flags
	//
	// TODO: add other binary operators with flags
	template <typename Enum>
	class Flags {
	public:
		using UnderlyingType = std::underlying_type_t<Enum>; // for most cases it is 'int'

		Flags() = default;
		Flags(int i) : i(i) {}
		Flags(const Enum& enm) : i((UnderlyingType)enm) {}

		Flags operator |= (Enum enm) const {
			i |= (UnderlyingType)enm;
			return *this;
		}
		Flags operator | (Enum enm) const {
			return Flags(i | (UnderlyingType)enm);
		}
		Flags operator & (Enum enm) const {
			return Flags(i & (UnderlyingType)enm);
		}
		Flags operator & (UnderlyingType mask) const {
			return Flags(i & mask);
		}

		operator UnderlyingType() const {
			return i;
		}
		//operator bool() const { // for compare with boolean values (anyway need explicitly cast with boolean compare, so you need override boolean operators ==, != ...)
		//	return (bool)i;
		//}

	private:
		UnderlyingType i = 0;
	};

	//
	// Function Traits
	//
	template <typename ReturnType, typename ClassType, typename... Args>
	struct _FunctionTraitsBase {
		enum {
			ArgumentCount = sizeof...(Args)
		};
		static constexpr bool IsPointerToMemberFunction = !std::is_same_v<ClassType, void>;

		using Ret = ReturnType;
		using Class = ClassType;
		using Function = Ret(Class::*) (Args...);
		using Arguments = std::tuple<Args...>;

		template <size_t i>
		struct arg {
			using type = typename std::tuple_element<i, std::tuple<Args...>>::type;
			// the i-th argument is equivalent to the i-th tuple element of a tuple
			// composed of those arguments.
		};
	};


	// Matches when T=lambda or T=Functor
	// For generic types, directly use the result of the signature of its 'operator()'
	template <typename T>
	struct FunctionTraits : public FunctionTraits<decltype(&T::operator())>
	{};

	// For Functor L-value or R-value
	template <typename R, typename C, typename... A>
	struct FunctionTraits<R(C::*)(A...)> : public _FunctionTraitsBase<R, C, A...>
	{};

	// For lambdas (need const)
	template <typename R, typename C, typename... A>
	struct FunctionTraits<R(C::*)(A...) const> : public _FunctionTraitsBase<R, C, A...>
	{};

	// For C-style functions
	template <typename R, typename... A>
	struct FunctionTraits<R(*)(A...)> : public _FunctionTraitsBase<R, void, A...>
	{};



	//
	// move_lambda, movable_function (https://gist.github.com/jbandela/4208898)
	//
	template<class T>
	using moved_value = std::reference_wrapper<T>;

	template<class T, class F>
	struct move_lambda {
	private:
		T val;
		F fn;
	public:
		move_lambda(T&& v, F f)
			: val(std::move(v))
			, fn(f)
		{}
		move_lambda(move_lambda&& other) = default;
		move_lambda& operator=(move_lambda&& other) = default;

		template<class... Args>
		auto operator()(Args&& ...args) -> decltype(this->fn(moved_value<T>(this->val), std::forward<Args>(args)...)) {
			moved_value<T> mv(val);
			return fn(mv, std::forward<Args>(args)...);
		}

		move_lambda() = delete;
		move_lambda(const move_lambda&) = delete;
		move_lambda& operator=(const move_lambda&) = delete;
	};

	template<class T, class F>
	move_lambda<T, F> create_move_lambda(T&& t, F f) {
		return move_lambda<T, F>(std::move(t), f);
	}

	// Unfortunately, std::function does not seem to support move-only callables
	// See § 20.8.11.2.1 point 7 where it requires F be CopyConstructible 
	// From draft at http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2011/n3242.pdf
	// Here is our movable replacement for std::function
	template< class ReturnType, class... ParamTypes>
	struct movable_function_base {
		virtual ReturnType callFunc(ParamTypes&&... p) = 0;
		virtual ~movable_function_base() = default;
	};


	template<class F, class ReturnType, class... ParamTypes>
	struct movable_function_imp : public movable_function_base<ReturnType, ParamTypes...> {
		F fn;
		virtual ReturnType callFunc(ParamTypes&&... p) {
			return fn(std::forward<ParamTypes>(p)...);
		}
		explicit movable_function_imp(F&& f)
			: fn(std::move(f))
		{};

		movable_function_imp() = delete;
		movable_function_imp(const movable_function_imp&) = delete;
		movable_function_imp& operator=(const movable_function_imp&) = delete;
	};


	template<class FuncType>
	struct movable_function {};

	template<class ReturnType, class... ParamTypes>
	struct movable_function<ReturnType(ParamTypes...)> {
		std::unique_ptr<movable_function_base<ReturnType, ParamTypes...>> ptr;

		template<class F>
		/*explicit*/ movable_function(F&& f)
			: ptr(std::make_unique<movable_function_imp<F, ReturnType, ParamTypes...>>(std::move(f)))
		{}

		movable_function(movable_function&& other) = default;
		movable_function& operator=(movable_function&& other) = default;

		template<class... Args>
		auto operator()(Args&& ...args) -> ReturnType {
			return ptr->callFunc(std::forward<Args>(args)...);
		}

		explicit operator bool() const {
			return static_cast<bool>(this->ptr);
		}

		movable_function() = default; // delete;
		movable_function(const movable_function&) = delete;
		movable_function& operator=(const movable_function&) = delete;
	};

	//
	// Scope
	//
	template<class Fn>
	class Scope {
	public:
		Scope()
			: m_valid(false)
		{}

		Scope(Fn fn)
			: m_valid(true)
			, m_fn(std::move(fn))
		{}

		Scope(const Scope&) = delete;

		Scope(Scope&& other)
			: Scope()
		{
			swap(*this, other);
		}

		~Scope() {
			try {
				this->EndScope();
			}
			catch (...) {
			}
		}

		Scope& operator=(Scope other) {
			swap(*this, other);
			return *this;
		}

		void EndScope() {
			if (!m_valid) {
				return;
			}

			m_fn();
			m_valid = false;
		}

		friend void swap(Scope& a, Scope& b) {
			using std::swap;
			swap(a.m_valid, b.m_valid);
			swap(a.m_fn, b.m_fn);
		}

	private:
		bool m_valid;
		Fn m_fn;
	};


	template<class Fn>
	Scope<Fn> MakeScope(Fn fn) {
		return Scope<Fn>(std::move(fn));
	}

	template<class T, class F>
	Scope<movable_function<void()>> MakeScope(move_lambda<T, F> movedLambda) {
		return Scope<movable_function<void()>>(std::move(movedLambda));
	}
};
