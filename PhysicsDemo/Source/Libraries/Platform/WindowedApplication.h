#pragma once

#include "Application.h"
#include "Window.h"

#include <memory>

namespace jm::Platform
{
	class DrawDevice;
	class Window;

	class MessageHandler
	{
	public:
		virtual ~MessageHandler() {}

		virtual bool ProcessSystemMessage(MSG& systemInput) = 0;
	};

	class LoopedApplication
	{

	public:

		LoopedApplication(const RuntimeContext& context)
			: Context{context}
			, Running(true)
		{}

		virtual ~LoopedApplication() {}

		int Run();

	protected:

		const RuntimeContext Context;

		bool Running;

		virtual void OnStartLoop() = 0;

		virtual void OnStopLoop() = 0;

		virtual void BeforeRunLoop() = 0;

		virtual void RunLoop() = 0;

		virtual void AfterRunLoop() = 0;

		virtual int ExitCode() const = 0;
	};

	class WindowedApplication : public LoopedApplication
	{
	public:

		WindowedApplication(const RuntimeContext& context, WindowParameters parameters);

		virtual ~WindowedApplication();

	protected:

		virtual void BeforeRunLoop() override;
		virtual void AfterRunLoop() override;
		virtual int ExitCode() const override;

		void AddMessageHandler(MessageHandler* handler);
		void RemoveMessageHandler(MessageHandler* handler);

		std::unique_ptr<Window> window;

	private:

		std::vector<MessageHandler*> handlers;
		int currentErrorCode = 0;
	};
}
