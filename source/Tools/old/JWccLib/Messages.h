#pragma once

#include <string>
#include <vector>
#include <memory>

using namespace std;

namespace WadeSpace
{
	typedef enum 
	{
		FATAL_ERROR_MSG,
		ERROR_MSG,
		WARNING_MSG,
		INFO_MSG
	} CompilerMessagesType;
	typedef struct
	{
		CompilerMessagesType type;
		string message;
	} MessageData;
	class Messages
	{
		shared_ptr<vector<MessageData>> messages;
	public:
		Messages() { messages = make_shared<vector<MessageData>>(); }
		Messages(const Messages& other) = default;
		virtual ~Messages() = default;
		Messages(Messages&& other) noexcept = default;
		Messages& operator=(const Messages& other) = default;
		Messages& operator=(Messages&& other) noexcept = default;
		[[nodiscard]] shared_ptr<vector<MessageData>> getMessages() const { return messages; }
		void setMessages(shared_ptr<vector<MessageData>> msgs) { messages = msgs; }
		[[nodiscard]] bool hasMessages() const { return messages != nullptr; }
		void addMessage(CompilerMessagesType type, string message) { messages->push_back({type, message}); }
	};
}
