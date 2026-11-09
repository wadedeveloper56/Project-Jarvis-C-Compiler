#include "pch.h"
#include "CppUnitTest.h"
#define NOMINMAX
#include <Windows.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace fs = std::filesystem;

namespace JWlink2Test {
namespace {
using Bytes = std::vector<std::uint8_t>;

struct TestRelocation {
	std::uint32_t offset;
	std::uint32_t symbolIndex;
	std::uint16_t type;
};

struct TestSection {
	std::string name;
	Bytes data;
	std::uint32_t characteristics = 0x60000020;
	std::vector<TestRelocation> relocations;
};

struct TestSymbol {
	std::string name;
	std::uint32_t value = 0;
	std::int32_t section = 0;
	std::uint8_t storageClass = 2;
	std::vector<std::array<std::uint8_t, 18>> auxiliary;
	std::vector<std::array<std::uint8_t, 20>> bigObjAuxiliary;
};

void put16(Bytes& bytes, std::size_t offset, std::uint16_t value) {
	if (bytes.size() < offset + 2) bytes.resize(offset + 2);
	bytes[offset] = static_cast<std::uint8_t>(value);
	bytes[offset + 1] = static_cast<std::uint8_t>(value >> 8);
}

void put32(Bytes& bytes, std::size_t offset, std::uint32_t value) {
	if (bytes.size() < offset + 4) bytes.resize(offset + 4);
	for (std::size_t i = 0; i < 4; ++i) bytes[offset + i] = static_cast<std::uint8_t>(value >> (i * 8));
}

void put64(Bytes& bytes, std::size_t offset, std::uint64_t value) {
	if (bytes.size() < offset + 8) bytes.resize(offset + 8);
	for (std::size_t i = 0; i < 8; ++i) bytes[offset + i] = static_cast<std::uint8_t>(value >> (i * 8));
}

std::uint16_t get16(const Bytes& bytes, std::size_t offset) {
	Assert::IsTrue(offset + 2 <= bytes.size(), L"PE field exceeds the file");
	return static_cast<std::uint16_t>(bytes[offset] | (static_cast<std::uint16_t>(bytes[offset + 1]) << 8));
}

std::uint32_t get32(const Bytes& bytes, std::size_t offset) {
	Assert::IsTrue(offset + 4 <= bytes.size(), L"PE field exceeds the file");
	std::uint32_t value = 0;
	for (std::size_t i = 0; i < 4; ++i) value |= static_cast<std::uint32_t>(bytes[offset + i]) << (8 * i);
	return value;
}

std::uint64_t get64(const Bytes& bytes, std::size_t offset) {
	Assert::IsTrue(offset + 8 <= bytes.size(), L"PE field exceeds the file");
	std::uint64_t value = 0;
	for (std::size_t i = 0; i < 8; ++i) value |= static_cast<std::uint64_t>(bytes[offset + i]) << (8 * i);
	return value;
}

void appendString(Bytes& bytes, const std::string& value) {
	bytes.insert(bytes.end(), value.begin(), value.end());
	bytes.push_back(0);
}

std::uint32_t addString(Bytes& table, const std::string& value) {
	const auto offset = static_cast<std::uint32_t>(table.size());
	appendString(table, value);
	return offset;
}

Bytes makeObject(std::vector<TestSection> sections, std::vector<TestSymbol> symbols, std::uint16_t machine = 0x8664) {
	Bytes bytes(20 + sections.size() * 40, 0);
	put16(bytes, 0, machine);
	put16(bytes, 2, static_cast<std::uint16_t>(sections.size()));
	put16(bytes, 16, 0);
	Bytes strings(4, 0);
	std::vector<std::size_t> sectionNameOffsets(sections.size(), 0);
	for (std::size_t i = 0; i < sections.size(); ++i) {
		if (sections[i].name.size() > 8) sectionNameOffsets[i] = addString(strings, sections[i].name);
	}
	std::vector<std::uint32_t> rawOffsets(sections.size(), 0);
	for (std::size_t i = 0; i < sections.size(); ++i) {
		if (!sections[i].data.empty()) {
			rawOffsets[i] = static_cast<std::uint32_t>(bytes.size());
			bytes.insert(bytes.end(), sections[i].data.begin(), sections[i].data.end());
		}
	}
	std::vector<std::uint32_t> relocationOffsets(sections.size(), 0);
	for (std::size_t i = 0; i < sections.size(); ++i) {
		if (!sections[i].relocations.empty()) {
			relocationOffsets[i] = static_cast<std::uint32_t>(bytes.size());
			for (const auto& relocation : sections[i].relocations) {
				const auto entry = bytes.size();
				bytes.resize(entry + 10, 0);
				put32(bytes, entry, relocation.offset);
				put32(bytes, entry + 4, relocation.symbolIndex);
				put16(bytes, entry + 8, relocation.type);
			}
		}
	}
	const auto symbolOffset = static_cast<std::uint32_t>(bytes.size());
	std::uint32_t symbolCount = 0;
	for (const auto& symbol : symbols) symbolCount += static_cast<std::uint32_t>(1 + symbol.auxiliary.size());
	put32(bytes, 8, symbolOffset);
	put32(bytes, 12, symbolCount);
	for (const auto& symbol : symbols) {
		const auto entry = bytes.size();
		bytes.resize(entry + 18, 0);
		if (symbol.name.size() <= 8) {
			std::copy(symbol.name.begin(), symbol.name.end(), bytes.begin() + entry);
		} else {
			put32(bytes, entry, 0);
			put32(bytes, entry + 4, addString(strings, symbol.name));
		}
		put32(bytes, entry + 8, symbol.value);
		put16(bytes, entry + 12, static_cast<std::uint16_t>(symbol.section));
		bytes[entry + 16] = symbol.storageClass;
		bytes[entry + 17] = static_cast<std::uint8_t>(symbol.auxiliary.size());
		for (const auto& auxiliary : symbol.auxiliary) bytes.insert(bytes.end(), auxiliary.begin(), auxiliary.end());
	}
	put32(bytes, bytes.size(), static_cast<std::uint32_t>(strings.size()));
	bytes.insert(bytes.end(), strings.begin() + 4, strings.end());
	for (std::size_t i = 0; i < sections.size(); ++i) {
		const auto header = 20 + i * 40;
		if (sectionNameOffsets[i] == 0) {
			std::copy(sections[i].name.begin(), sections[i].name.end(), bytes.begin() + header);
		} else {
			const auto longName = "/" + std::to_string(sectionNameOffsets[i]);
			std::copy(longName.begin(), longName.end(), bytes.begin() + header);
		}
		put32(bytes, header + 8, static_cast<std::uint32_t>(sections[i].data.size()));
		put32(bytes, header + 16, static_cast<std::uint32_t>(sections[i].data.size()));
		put32(bytes, header + 20, rawOffsets[i]);
		put32(bytes, header + 24, relocationOffsets[i]);
		put16(bytes, header + 32, static_cast<std::uint16_t>(sections[i].relocations.size()));
		put32(bytes, header + 36, sections[i].characteristics);
	}
	return bytes;
}

std::array<std::uint8_t, 18> sectionAux(std::uint32_t dataLength, std::uint16_t associated, std::uint8_t selection) {
	std::array<std::uint8_t, 18> auxiliary{};
	for (std::size_t i = 0; i < 4; ++i) auxiliary[i] = static_cast<std::uint8_t>(dataLength >> (i * 8));
	auxiliary[12] = static_cast<std::uint8_t>(associated);
	auxiliary[13] = static_cast<std::uint8_t>(associated >> 8);
	auxiliary[14] = selection;
	return auxiliary;
}

std::array<std::uint8_t, 20> bigObjSectionAux(std::uint32_t associated, std::uint8_t selection) {
	std::array<std::uint8_t, 20> auxiliary{};
	auxiliary[12] = static_cast<std::uint8_t>(associated);
	auxiliary[13] = static_cast<std::uint8_t>(associated >> 8);
	auxiliary[18] = static_cast<std::uint8_t>(associated >> 16);
	auxiliary[19] = static_cast<std::uint8_t>(associated >> 24);
	auxiliary[16] = selection;
	return auxiliary;
}

std::array<std::uint8_t, 18> weakAux(std::uint32_t tagIndex, std::uint32_t characteristics) {
	std::array<std::uint8_t, 18> auxiliary{};
	for (std::size_t i = 0; i < 4; ++i) auxiliary[i] = static_cast<std::uint8_t>(tagIndex >> (i * 8));
	for (std::size_t i = 0; i < 4; ++i) auxiliary[4 + i] = static_cast<std::uint8_t>(characteristics >> (i * 8));
	return auxiliary;
}

Bytes makeEntryObject(const std::string& entry = "main") {
	return makeObject({TestSection{".text", {0xc3}}}, {TestSymbol{entry, 0, 1, 2, {}}});
}

TestSection makeDirectiveSection(const std::string& directives) {
	return TestSection{".drectve", Bytes(directives.begin(), directives.end()), 0x00100a00};
}

Bytes makeBigObjObject(const std::vector<TestSection>& sections, const std::vector<TestSymbol>& symbols) {
	Bytes bytes(56 + sections.size() * 40, 0);
	put16(bytes, 0, 0);
	put16(bytes, 2, 0xffff);
	put16(bytes, 4, 2);
	put16(bytes, 6, 0x8664);
	const std::array<std::uint8_t, 16> classId{0xc7, 0xa1, 0xba, 0xd1, 0xee, 0xba, 0xa9, 0x4b, 0xaf, 0x20, 0xfa, 0xf6, 0x6a, 0xa4, 0xdc, 0xb8};
	std::copy(classId.begin(), classId.end(), bytes.begin() + 12);
	put32(bytes, 44, static_cast<std::uint32_t>(sections.size()));

	Bytes strings(4, 0);
	std::vector<std::size_t> sectionNameOffsets(sections.size(), 0);
	for (std::size_t i = 0; i < sections.size(); ++i) {
		if (sections[i].name.size() > 8) sectionNameOffsets[i] = addString(strings, sections[i].name);
	}
	std::vector<std::uint32_t> rawOffsets(sections.size(), 0);
	std::vector<std::uint32_t> relocationOffsets(sections.size(), 0);
	for (std::size_t i = 0; i < sections.size(); ++i) {
		if (!sections[i].data.empty()) {
			rawOffsets[i] = static_cast<std::uint32_t>(bytes.size());
			bytes.insert(bytes.end(), sections[i].data.begin(), sections[i].data.end());
		}
	}
	for (std::size_t i = 0; i < sections.size(); ++i) {
		if (!sections[i].relocations.empty()) {
			relocationOffsets[i] = static_cast<std::uint32_t>(bytes.size());
			for (const auto& relocation : sections[i].relocations) {
				const auto entry = bytes.size();
				bytes.resize(entry + 10, 0);
				put32(bytes, entry, relocation.offset);
				put32(bytes, entry + 4, relocation.symbolIndex);
				put16(bytes, entry + 8, relocation.type);
			}
		}
	}
	const auto symbolOffset = static_cast<std::uint32_t>(bytes.size());
	std::uint32_t symbolCount = 0;
	for (const auto& symbol : symbols) symbolCount += static_cast<std::uint32_t>(1 + symbol.bigObjAuxiliary.size());
	put32(bytes, 48, symbolOffset);
	put32(bytes, 52, symbolCount);
	for (const auto& symbol : symbols) {
		const auto entry = bytes.size();
		bytes.resize(entry + 20, 0);
		if (symbol.name.size() <= 8) {
			std::copy(symbol.name.begin(), symbol.name.end(), bytes.begin() + entry);
		} else {
			put32(bytes, entry, 0);
			put32(bytes, entry + 4, addString(strings, symbol.name));
		}
		put32(bytes, entry + 8, symbol.value);
		put32(bytes, entry + 12, static_cast<std::uint32_t>(symbol.section));
		put16(bytes, entry + 16, 0);
		bytes[entry + 18] = symbol.storageClass;
		bytes[entry + 19] = static_cast<std::uint8_t>(symbol.bigObjAuxiliary.size());
		for (const auto& auxiliary : symbol.bigObjAuxiliary) bytes.insert(bytes.end(), auxiliary.begin(), auxiliary.end());
	}
	put32(bytes, bytes.size(), static_cast<std::uint32_t>(strings.size()));
	bytes.insert(bytes.end(), strings.begin() + 4, strings.end());
	for (std::size_t i = 0; i < sections.size(); ++i) {
		const auto header = 56 + i * 40;
		if (sectionNameOffsets[i] == 0) {
			std::copy(sections[i].name.begin(), sections[i].name.end(), bytes.begin() + header);
		} else {
			const auto longName = "/" + std::to_string(sectionNameOffsets[i]);
			std::copy(longName.begin(), longName.end(), bytes.begin() + header);
		}
		put32(bytes, header + 8, static_cast<std::uint32_t>(sections[i].data.size()));
		put32(bytes, header + 16, static_cast<std::uint32_t>(sections[i].data.size()));
		put32(bytes, header + 20, rawOffsets[i]);
		put32(bytes, header + 24, relocationOffsets[i]);
		put16(bytes, header + 32, static_cast<std::uint16_t>(sections[i].relocations.size()));
		put32(bytes, header + 36, sections[i].characteristics);
	}
	return bytes;
}

Bytes makeBigObjComdatObject(const std::string& key, const Bytes& content) {
	TestSymbol sectionSymbol{".text$comdat", 0, 1, 3, {}};
	sectionSymbol.bigObjAuxiliary.push_back(bigObjSectionAux(0, 2));
	return makeBigObjObject({TestSection{".text$comdat", content, 0x60001020}}, {sectionSymbol, TestSymbol{key, 0, 1, 2, {}}});
}

Bytes makeComdatObject(const std::string& key, const Bytes& content, std::uint8_t selection) {
	TestSection section{".text$comdat", content, 0x60001020};
	TestSymbol sectionSymbol{".text$comdat", 0, 1, 3, {sectionAux(static_cast<std::uint32_t>(content.size()), 0, selection)}};
	TestSymbol keySymbol{key, 0, 1, 2, {}};
	return makeObject({section}, {sectionSymbol, keySymbol});
}

Bytes makeAssociativeComdatObject(const Bytes& parentData, const Bytes& childData) {
	TestSection parent{".text$parent", parentData, 0x60001020};
	TestSection child{".xdata$child", childData, 0x40001040};
	TestSymbol parentSection{".text$parent", 0, 1, 3, {sectionAux(static_cast<std::uint32_t>(parentData.size()), 0, 2)}};
	TestSymbol parentKey{"parentKey", 0, 1, 2, {}};
	TestSymbol childSection{".xdata$child", 0, 2, 3, {sectionAux(static_cast<std::uint32_t>(childData.size()), 1, 5)}};
	return makeObject({parent, child}, {parentSection, parentKey, childSection});
}

Bytes makeImportObject(const std::string& publicName, const std::string& dllName, const std::string& importName, std::uint16_t hint, std::uint16_t typeInfo) {
	Bytes strings;
	appendString(strings, publicName);
	appendString(strings, dllName);
	if (!importName.empty()) appendString(strings, importName);
	Bytes bytes(20, 0);
	put16(bytes, 0, 0);
	put16(bytes, 2, 0xffff);
	put16(bytes, 6, 0x8664);
	put32(bytes, 12, static_cast<std::uint32_t>(strings.size()));
	put16(bytes, 16, hint);
	put16(bytes, 18, typeInfo);
	bytes.insert(bytes.end(), strings.begin(), strings.end());
	return bytes;
}

void appendArchiveMember(Bytes& archive, const std::string& name, const Bytes& content) {
	auto appendField = [&](const std::string& value, std::size_t width) {
		std::string field = value;
		field.resize(width, ' ');
		archive.insert(archive.end(), field.begin(), field.end());
	};
	appendField(name + (name.find('/') == std::string::npos ? "/" : ""), 16);
	appendField("0", 12);
	appendField("0", 6);
	appendField("0", 6);
	appendField("100644", 8);
	appendField(std::to_string(content.size()), 10);
	archive.push_back('`');
	archive.push_back('\n');
	archive.insert(archive.end(), content.begin(), content.end());
	if ((archive.size() & 1) != 0) archive.push_back('\n');
}

Bytes makeArchive(const std::vector<std::pair<std::string, Bytes>>& members) {
	const std::string magic = "!<arch>\n";
	Bytes bytes(magic.begin(), magic.end());
	for (const auto& member : members) appendArchiveMember(bytes, member.first, member.second);
	return bytes;
}

class Workspace {
public:
	Workspace() {
		wchar_t temp[MAX_PATH]{};
		const auto tempLength = GetTempPathW(MAX_PATH, temp);
		Assert::IsTrue(tempLength != 0 && tempLength < MAX_PATH, L"could not locate the temporary directory");
		static std::atomic<unsigned long long> sequence{0};
		const auto suffix = L"JWlink2Test-" + std::to_wstring(GetCurrentProcessId()) + L"-" + std::to_wstring(GetTickCount64()) + L"-" + std::to_wstring(sequence.fetch_add(1));
		path = fs::path(temp) / suffix;
		fs::create_directories(path);
	}
	~Workspace() {
		std::error_code ignored;
		fs::remove_all(path, ignored);
	}
	fs::path path;
	fs::path file(const std::wstring& name) const { return path / name; }
	void write(const std::wstring& name, const Bytes& bytes) const {
		std::ofstream output(file(name), std::ios::binary);
		Assert::IsTrue(static_cast<bool>(output), L"could not create fixture file");
		output.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
		Assert::IsTrue(static_cast<bool>(output), L"could not write fixture file");
	}
	void writeText(const std::wstring& name, const std::wstring& text) const {
		std::wofstream output(file(name));
		Assert::IsTrue(static_cast<bool>(output), L"could not create text fixture");
		output << text;
		Assert::IsTrue(static_cast<bool>(output), L"could not write text fixture");
	}
	Bytes read(const std::wstring& name) const {
		std::ifstream input(file(name), std::ios::binary);
		Assert::IsTrue(static_cast<bool>(input), L"expected output file was not created");
		return Bytes(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
	}
};

void ModuleAnchor() {}

fs::path toolPath(const wchar_t* executableName) {
	HMODULE module = nullptr;
	const auto address = reinterpret_cast<LPCWSTR>(&ModuleAnchor);
	Assert::IsTrue(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, address, &module) != 0,
		L"could not locate the JWlink2Test module");
	wchar_t path[32768]{};
	const auto length = GetModuleFileNameW(module, path, static_cast<DWORD>(std::size(path)));
	Assert::IsTrue(length != 0 && length < std::size(path), L"could not read the JWlink2Test module path");
	auto directory = fs::path(path).parent_path();
	while (!directory.empty()) {
		for (const auto& configuration : {L"Debug", L"Release"}) {
			const auto candidate = directory / L"bin" / L"tools" / configuration / L"x64" / executableName;
			if (fs::exists(candidate)) return candidate;
		}
		directory = directory.parent_path();
	}
	Assert::Fail(L"could not find the required x64 linker build output from the test module location");
	return {};
}

fs::path jwlink2Path() { return toolPath(L"JWlink2.exe"); }
fs::path jwlinkPath() { return toolPath(L"JWlink.exe"); }

std::wstring quoteArgument(const std::wstring& value) {
	std::wstring quoted = L"\"";
	std::size_t slashes = 0;
	for (const auto ch : value) {
		if (ch == L'\\') {
			++slashes;
		} else if (ch == L'\"') {
			quoted.append(slashes * 2 + 1, L'\\');
			quoted.push_back(ch);
			slashes = 0;
		} else {
			quoted.append(slashes, L'\\');
			quoted.push_back(ch);
			slashes = 0;
		}
	}
	quoted.append(slashes * 2, L'\\');
	quoted.push_back(L'\"');
	return quoted;
}

struct ProcessResult {
	DWORD exitCode = 0;
	std::string output;
};

ProcessResult runExecutable(const Workspace& workspace, const fs::path& executable, const std::vector<std::wstring>& arguments) {
	SECURITY_ATTRIBUTES security{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
	HANDLE readPipe = nullptr;
	HANDLE writePipe = nullptr;
	Assert::IsTrue(CreatePipe(&readPipe, &writePipe, &security, 0) != 0, L"could not create process output pipe");
	SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0);
	STARTUPINFOW startup{};
	startup.cb = sizeof(startup);
	startup.dwFlags = STARTF_USESTDHANDLES;
	startup.hStdOutput = writePipe;
	startup.hStdError = writePipe;
	startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	PROCESS_INFORMATION process{};
	std::wstring command = quoteArgument(executable.wstring());
	for (const auto& argument : arguments) {
		command.push_back(L' ');
		command += quoteArgument(argument);
	}
	std::vector<wchar_t> mutableCommand(command.begin(), command.end());
	mutableCommand.push_back(L'\0');
	const auto created = CreateProcessW(nullptr, mutableCommand.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, workspace.path.c_str(), &startup, &process);
	CloseHandle(writePipe);
	Assert::IsTrue(created != 0, L"could not start linker process");
	ProcessResult result;
	char buffer[2048];
	DWORD count = 0;
	while (ReadFile(readPipe, buffer, sizeof(buffer), &count, nullptr) && count != 0) result.output.append(buffer, count);
	CloseHandle(readPipe);
	const auto wait = WaitForSingleObject(process.hProcess, 10000);
	if (wait == WAIT_TIMEOUT) {
		TerminateProcess(process.hProcess, 0xffff);
		WaitForSingleObject(process.hProcess, INFINITE);
	}
	Assert::IsTrue(wait == WAIT_OBJECT_0, L"linker did not exit within 10 seconds");
	GetExitCodeProcess(process.hProcess, &result.exitCode);
	CloseHandle(process.hThread);
	CloseHandle(process.hProcess);
	return result;
}

ProcessResult runLinker(const Workspace& workspace, const std::vector<std::wstring>& arguments) {
	return runExecutable(workspace, jwlink2Path(), arguments);
}

std::vector<std::wstring> outputArgs(const Workspace& workspace, const std::wstring& outputName = L"linked.exe") {
	return {L"/OUT:" + workspace.file(outputName).wstring(), L"/ENTRY:main"};
}

ProcessResult linkObject(const Workspace& workspace, const Bytes& object, std::vector<std::wstring> args = {}) {
	workspace.write(L"input.obj", object);
	if (args.empty()) args = outputArgs(workspace);
	args.push_back(workspace.file(L"input.obj").wstring());
	return runLinker(workspace, args);
}

void assertSuccess(const ProcessResult& result) {
	const std::wstring diagnostic(result.output.begin(), result.output.end());
	Assert::IsTrue(result.exitCode == 0, diagnostic.c_str());
}

void assertFailureContains(const ProcessResult& result, const std::string& expected) {
	Assert::IsTrue(result.exitCode != 0, L"link should fail");
	const std::wstring diagnostic(result.output.begin(), result.output.end());
	Assert::IsTrue(result.output.find(expected) != std::string::npos, diagnostic.c_str());
}

std::size_t peHeader(const Bytes& image) {
	Assert::IsTrue(image.size() >= 0x40, L"PE image is shorter than its DOS header");
	Assert::AreEqual<std::uint16_t>(0x5a4d, get16(image, 0), L"DOS signature");
	const auto offset = static_cast<std::size_t>(get32(image, 0x3c));
	Assert::IsTrue(offset <= image.size() && image.size() - offset >= 24, L"PE header is outside the file");
	Assert::AreEqual<std::uint32_t>(0x00004550, get32(image, offset), L"PE signature");
	return offset;
}

std::size_t optionalHeader(const Bytes& image) {
	const auto pe = peHeader(image);
	const auto coff = pe + 4;
	const auto optional = coff + 20;
	const auto optionalSize = get16(image, coff + 16);
	Assert::IsTrue(optional <= image.size() && optionalSize <= image.size() - optional, L"optional header is outside the file");
	Assert::IsTrue(optionalSize >= 2, L"optional header is missing its format field");
	Assert::AreEqual<std::uint16_t>(0x20b, get16(image, optional), L"PE32+ optional header");
	return optional;
}

std::size_t sectionTable(const Bytes& image) {
	const auto pe = peHeader(image);
	const auto coff = pe + 4;
	return optionalHeader(image) + get16(image, coff + 16);
}

std::uint16_t peFileCharacteristics(const Bytes& image) {
	return get16(image, peHeader(image) + 4 + 18);
}

bool hasSection(const Bytes& image, const std::string& name) {
	const auto pe = peHeader(image);
	const auto sectionCount = get16(image, pe + 4 + 2);
	const auto table = sectionTable(image);
	for (std::size_t i = 0; i < sectionCount; ++i) {
		const auto header = table + i * 40;
		const std::string sectionName(reinterpret_cast<const char*>(image.data() + header), strnlen_s(reinterpret_cast<const char*>(image.data() + header), 8));
		if (sectionName == name) return true;
	}
	return false;
}

std::size_t sectionHeader(const Bytes& image, const std::string& name) {
	const auto pe = peHeader(image);
	const auto sectionCount = get16(image, pe + 4 + 2);
	const auto table = sectionTable(image);
	for (std::size_t i = 0; i < sectionCount; ++i) {
		const auto header = table + i * 40;
		std::string sectionName(reinterpret_cast<const char*>(image.data() + header), strnlen_s(reinterpret_cast<const char*>(image.data() + header), 8));
		if (sectionName == name) return header;
	}
	Assert::Fail(L"requested PE section is missing");
	return 0;
}

std::size_t rvaOffset(const Bytes& image, std::uint32_t rva) {
	const auto pe = peHeader(image);
	const auto table = sectionTable(image);
	const auto count = get16(image, pe + 4 + 2);
	for (std::size_t i = 0; i < count; ++i) {
		const auto section = table + i * 40;
		const auto virtualSize = get32(image, section + 8);
		const auto sectionRva = get32(image, section + 12);
		const auto rawSize = get32(image, section + 16);
		if (rva >= sectionRva && static_cast<std::uint64_t>(rva) < static_cast<std::uint64_t>(sectionRva) + std::max(virtualSize, rawSize)) {
			const auto delta = rva - sectionRva;
			Assert::IsTrue(delta < rawSize, L"RVA does not refer to initialized file data");
			return get32(image, section + 20) + delta;
		}
	}
	Assert::Fail(L"RVA does not map to a PE section");
	return 0;
}

std::string cStringAtRva(const Bytes& image, std::uint32_t rva) {
	const auto offset = rvaOffset(image, rva);
	std::size_t end = offset;
	while (end < image.size() && image[end] != 0) ++end;
	Assert::IsTrue(end < image.size(), L"RVA string is not terminated");
	return std::string(reinterpret_cast<const char*>(image.data() + offset), end - offset);
}

void assertHasBaseReloc(const Bytes& image, std::uint32_t targetRva, std::uint16_t expectedType) {
	const auto optional = optionalHeader(image);
	const auto relocRva = get32(image, optional + 152);
	const auto relocSize = get32(image, optional + 156);
	Assert::IsTrue(relocRva != 0 && relocSize >= 8, L"base-relocation directory should be present");
	const auto directory = rvaOffset(image, relocRva);
	const auto page = targetRva & ~std::uint32_t{0xfff};
	std::size_t cursor = directory;
	const auto limit = directory + relocSize;
	while (cursor + 8 <= limit) {
		const auto blockPage = get32(image, cursor);
		const auto blockSize = get32(image, cursor + 4);
		Assert::IsTrue(blockSize >= 8 && cursor + blockSize <= limit, L"base-relocation block size must be valid");
		if (blockPage == page) {
			for (std::size_t entry = cursor + 8; entry + 2 <= cursor + blockSize; entry += 2) {
				const auto value = get16(image, entry);
				if ((value >> 12) == expectedType && (blockPage + (value & 0x0fff)) == targetRva) return;
			}
		}
		cursor += blockSize;
	}
	Assert::Fail(L"expected base-relocation entry is missing");
}

void assertNoOutput(const Workspace& workspace, const std::wstring& outputName = L"linked.exe") {
	Assert::IsFalse(fs::exists(workspace.file(outputName)), L"failed link must not create an output image");
}
}

TEST_CLASS(JWlink2LinkerTests) {
public:
	TEST_METHOD(BigObjSupportsWideSectionNumbersSymbolsAndRelocations) {
		Workspace workspace;
		std::vector<TestSection> sections(32768, TestSection{".data", {0x5a}, 0xc0000040});
		TestSection text{".text", Bytes(4, 0)};
		text.relocations = {{0, 1, 4}};
		sections.push_back(std::move(text));
		const auto object = makeBigObjObject(std::move(sections), {
			TestSymbol{"main", 0, 32769, 2, {}}, TestSymbol{"targetFunction", 2, 32769, 2, {}}
		});
		assertSuccess(linkObject(workspace, object));
		const auto image = workspace.read(L"linked.exe");
		const auto textHeader = sectionHeader(image, ".text");
		Assert::AreEqual<std::uint32_t>(get32(image, textHeader + 12), get32(image, optionalHeader(image) + 16), L"32-bit section index resolves the entry point");
		Assert::AreEqual<std::uint32_t>(static_cast<std::uint32_t>(-2), get32(image, get32(image, textHeader + 20)), L"20-byte BigObj symbol records resolve AMD64 relocations");
		Assert::AreEqual<std::uint32_t>(32768, get32(image, sectionHeader(image, ".data") + 8), L"all BigObj sections are parsed and merged");
	}

	TEST_METHOD(BigObjSectionAuxiliaryRecordSelectsComdatWinner) {
		Workspace workspace;
		workspace.write(L"entry.obj", makeEntryObject());
		workspace.write(L"first.obj", makeBigObjComdatObject("bigObjComdat", {0x41}));
		workspace.write(L"second.obj", makeBigObjComdatObject("bigObjComdat", {0x42}));
		auto args = outputArgs(workspace);
		for (const auto& file : {L"entry.obj", L"first.obj", L"second.obj"}) args.push_back(workspace.file(file).wstring());
		assertSuccess(runLinker(workspace, args));
		const auto image = workspace.read(L"linked.exe");
		const auto text = sectionHeader(image, ".text");
		const auto raw = get32(image, text + 20);
		Assert::IsTrue(std::find(image.begin() + raw, image.begin() + raw + get32(image, text + 16), 0x41) != image.begin() + raw + get32(image, text + 16), L"first BigObj COMDAT candidate is selected");
		Assert::IsTrue(std::find(image.begin() + raw, image.begin() + raw + get32(image, text + 16), 0x42) == image.begin() + raw + get32(image, text + 16), L"losing BigObj COMDAT candidate is discarded");
	}

	TEST_METHOD(BigObjArchiveMemberCanSatisfyExternalSymbol) {
		Workspace workspace;
		TestSection text{".text", Bytes(4, 0)};
		text.relocations = {{0, 1, 4}};
		workspace.write(L"input.obj", makeObject({text}, {TestSymbol{"main", 0, 1, 2, {}}, TestSymbol{"bigObjHelper", 0, 0, 2, {}}}));
		workspace.write(L"bigobj.lib", makeArchive({{"bighelper.obj", makeBigObjObject({TestSection{".text", {0xc3}}}, {TestSymbol{"bigObjHelper", 0, 1, 2, {}}})}}));
		auto args = outputArgs(workspace);
		args.push_back(workspace.file(L"input.obj").wstring());
		args.push_back(workspace.file(L"bigobj.lib").wstring());
		assertSuccess(runLinker(workspace, args));
		Assert::IsTrue(sectionHeader(workspace.read(L"linked.exe"), ".text") != 0, L"BigObj archive member is selected and linked");
	}

	TEST_METHOD(InvalidBigObjClassIdentifierIsRejected) {
		Workspace workspace;
		auto object = makeBigObjObject({TestSection{".text", {0xc3}}}, {TestSymbol{"main", 0, 1, 2, {}}});
		object[12] ^= 0x01;
		const auto result = linkObject(workspace, object);
		assertFailureContains(result, "unsupported anonymous COFF object signature");
		assertNoOutput(workspace);
	}

	TEST_METHOD(ValidAmd64ObjectProducesPe32PlusWithDefaultBaseAndEntry) {
		Workspace workspace;
		assertSuccess(linkObject(workspace, makeEntryObject()));
		const auto image = workspace.read(L"linked.exe");
		const auto optional = optionalHeader(image);
		Assert::AreEqual<std::uint16_t>(0x8664, get16(image, 0x84), L"AMD64 machine");
		Assert::AreEqual<std::uint64_t>(0x140000000, get64(image, optional + 24), L"default image base");
		Assert::AreEqual<std::uint32_t>(0x1000, get32(image, optional + 16), L"default entry point RVA");
		Assert::AreEqual<std::uint16_t>(3, get16(image, optional + 68), L"default console subsystem");
		Assert::IsTrue((get16(image, 0x96) & 0x2000) == 0, L"supported output is a PE executable, not a DLL");
	}

	TEST_METHOD(DllOutputIsOutsideSupportedBoundary) {
		Workspace workspace;
		workspace.write(L"input.obj", makeEntryObject());
		auto args = outputArgs(workspace);
		args.insert(args.begin(), L"/DLL");
		args.push_back(workspace.file(L"input.obj").wstring());
		assertFailureContains(runLinker(workspace, args), "unsupported option '/DLL'");
		assertNoOutput(workspace);
	}

	TEST_METHOD(RepresentativeObjectMatchesJWlinkPeBehavior) {
		Workspace workspace;
		const auto object = makeObject(
			{TestSection{".text", {0xc3, 0x2a}}, makeDirectiveSection("-entry:main ")},
			{TestSymbol{"main", 0, 1, 2, {}}});
		workspace.write(L"input.obj", object);
		const auto jwlink2Args = std::vector<std::wstring>{L"/OUT:" + workspace.file(L"jwlink2.exe").wstring(), workspace.file(L"input.obj").wstring()};
		assertSuccess(runExecutable(workspace, jwlink2Path(), jwlink2Args));
		const auto jwlinkArgs = std::vector<std::wstring>{
			L"format", L"win", L"pe", L"option", L"quiet,start=main",
			L"file", workspace.file(L"input.obj").wstring(),
			L"name", workspace.file(L"jwlink.exe").wstring()
		};
		assertSuccess(runExecutable(workspace, jwlinkPath(), jwlinkArgs));

		const auto jwlink2Image = workspace.read(L"jwlink2.exe");
		const auto jwlinkImage = workspace.read(L"jwlink.exe");
		for (const auto* image : {&jwlink2Image, &jwlinkImage}) {
			const auto optional = optionalHeader(*image);
			const auto pe = peHeader(*image);
			Assert::AreEqual<std::uint16_t>(0x8664, get16(*image, pe + 4), L"both linkers target AMD64");
			Assert::IsTrue((peFileCharacteristics(*image) & 0x2000) == 0, L"both outputs are executables, not DLLs");
			Assert::AreEqual<std::uint16_t>(3, get16(*image, optional + 68), L"both outputs use the console subsystem");
			const auto entryRva = get32(*image, optional + 16);
			const auto entryOffset = rvaOffset(*image, entryRva);
			Assert::AreEqual<std::uint8_t>(0xc3, (*image)[entryOffset], L"both entry points execute the return instruction");
			const auto text = sectionHeader(*image, ".text");
			Assert::IsTrue(get32(*image, text + 16) >= 2, L"both images preserve initialized code bytes");
			const auto textOffset = get32(*image, text + 20);
			Assert::AreEqual<std::uint8_t>(0xc3, (*image)[textOffset], L"both images preserve the entry-point instruction");
			Assert::AreEqual<std::uint8_t>(0x2a, (*image)[textOffset + 1], L"both images preserve the second code-section byte");
		}
	}

	TEST_METHOD(Rel32ThroughRel32FiveAreAppliedWithCorrectPlaceBias) {
		Workspace workspace;
		TestSection text{".text", Bytes(24, 0)};
		for (std::uint16_t type = 4; type <= 9; ++type) text.relocations.push_back({static_cast<std::uint32_t>((type - 4) * 4), 0, type});
		const auto object = makeObject({text}, {TestSymbol{"main", 0, 1, 2, {}}});
		assertSuccess(linkObject(workspace, object));
		const auto image = workspace.read(L"linked.exe");
		const auto section = sectionHeader(image, ".text");
		const auto raw = get32(image, section + 20);
		const auto sectionRva = get32(image, section + 12);
		for (std::uint16_t type = 4; type <= 9; ++type) {
			const auto offset = static_cast<std::size_t>((type - 4) * 4);
			const auto extra = static_cast<std::int64_t>(type - 4);
			const auto expected = static_cast<std::int32_t>(-static_cast<std::int64_t>(offset + 4) - extra);
			Assert::AreEqual<std::uint32_t>(static_cast<std::uint32_t>(expected), get32(image, raw + offset), L"REL32-family displacement");
		}
		Assert::AreEqual<std::uint32_t>(sectionRva, get32(image, optionalHeader(image) + 16), L"entry remains at section start");
	}

	TEST_METHOD(AbsoluteRelocationsCreateDir64AndHighLowBaseRelocations) {
		Workspace workspace;
		TestSection data{".data", Bytes(12, 0), 0xc0000040};
		data.relocations = {{0, 0, 1}, {8, 0, 2}};
		TestSection text{".text", {0xc3}};
		const auto object = makeObject({data, text}, {TestSymbol{"main", 0, 2, 2, {}}});
		auto args = outputArgs(workspace);
		args.insert(args.begin() + 1, L"/BASE:0x10000");
		assertSuccess(linkObject(workspace, object, args));
		const auto image = workspace.read(L"linked.exe");
		const auto base = get64(image, optionalHeader(image) + 24);
		const auto dataHeader = sectionHeader(image, ".data");
		const auto dataRva = get32(image, dataHeader + 12);
		const auto dataRaw = get32(image, dataHeader + 20);
		const auto textRva = get32(image, sectionHeader(image, ".text") + 12);
		Assert::AreEqual<std::uint64_t>(base + textRva, get64(image, dataRaw), L"ADDR64 writes an absolute VA");
		Assert::AreEqual<std::uint32_t>(static_cast<std::uint32_t>(base + textRva), get32(image, dataRaw + 8), L"ADDR32 writes a 32-bit VA");
		assertHasBaseReloc(image, dataRva, 10);
		assertHasBaseReloc(image, dataRva + 8, 3);
	}

	TEST_METHOD(Addr32NbWritesAnImageRelativeAddress) {
		Workspace workspace;
		TestSection text{".text", Bytes(8, 0)};
		text.relocations = {{0, 0, 3}};
		const auto object = makeObject({text}, {TestSymbol{"main", 4, 1, 2, {}}});
		assertSuccess(linkObject(workspace, object));
		const auto image = workspace.read(L"linked.exe");
		const auto section = sectionHeader(image, ".text");
		Assert::AreEqual<std::uint32_t>(get32(image, section + 12) + 4, get32(image, get32(image, section + 20)), L"ADDR32NB uses an RVA");
	}

	TEST_METHOD(FixedImageOmitsRelocationsAndMarksThemStripped) {
		Workspace workspace;
		TestSection data{".data", Bytes(8, 0), 0xc0000040};
		data.relocations = {{0, 0, 1}};
		const auto object = makeObject({data, TestSection{".text", {0xc3}}}, {TestSymbol{"main", 0, 2, 2, {}}});
		auto args = outputArgs(workspace);
		args.insert(args.begin() + 1, L"/FIXED");
		assertSuccess(linkObject(workspace, object, args));
		const auto image = workspace.read(L"linked.exe");
		Assert::IsTrue((get16(image, 0x96) & 1) != 0, L"relocations-stripped file characteristic");
		Assert::AreEqual<std::uint32_t>(0, get32(image, optionalHeader(image) + 152), L"fixed image relocation RVA");
		Assert::AreEqual<std::uint32_t>(0, get32(image, optionalHeader(image) + 156), L"fixed image relocation size");
		Assert::IsTrue((get16(image, optionalHeader(image) + 70) & 0x40) == 0, L"fixed image must not advertise ASLR");
	}

	TEST_METHOD(ImportLibraryGeneratesDescriptorIatAndFunctionThunk) {
		Workspace workspace;
		TestSection text{".text", {0xff, 0x25, 0, 0, 0, 0}};
		text.relocations = {{2, 1, 4}};
		const auto object = makeObject({text}, {TestSymbol{"main", 0, 1, 2, {}}, TestSymbol{"__imp_ExitProcess", 0, 0, 2, {}}});
		workspace.write(L"input.obj", object);
		const auto import = makeImportObject("ExitProcess", "KERNEL32.dll", "ExitProcess", 321, 4);
		workspace.write(L"kernel.lib", makeArchive({{"exit.obj", import}}));
		auto args = outputArgs(workspace);
		args.push_back(workspace.file(L"input.obj").wstring());
		args.push_back(workspace.file(L"kernel.lib").wstring());
		assertSuccess(runLinker(workspace, args));
		const auto image = workspace.read(L"linked.exe");
		const auto optional = optionalHeader(image);
		const auto importRva = get32(image, optional + 120);
		Assert::IsTrue(importRva != 0, L"import directory RVA");
		const auto descriptor = rvaOffset(image, importRva);
		const auto nameRva = get32(image, descriptor + 12);
		const auto lookupRva = get32(image, descriptor);
		const auto iatRva = get32(image, descriptor + 16);
		Assert::IsTrue(cStringAtRva(image, nameRva) == "KERNEL32.dll", L"import DLL name");
		const auto lookup = rvaOffset(image, lookupRva);
		const auto iat = rvaOffset(image, iatRva);
		Assert::AreEqual<std::uint64_t>(get64(image, lookup), get64(image, iat), L"lookup and address tables begin identically");
		const auto hintNameRva = static_cast<std::uint32_t>(get64(image, lookup));
		const auto hintName = rvaOffset(image, hintNameRva);
		Assert::AreEqual<std::uint16_t>(321, get16(image, hintName), L"import hint");
		Assert::IsTrue(std::string(reinterpret_cast<const char*>(image.data() + hintName + 2), 11) == "ExitProcess", L"import symbol name");
		Assert::IsTrue(get64(image, lookup + 8) == 0 && get64(image, iat + 8) == 0, L"import thunk tables are null-terminated");
		Assert::IsTrue(get32(image, optional + 208) == iatRva && get32(image, optional + 212) == 16, L"IAT data directory");
	}

	TEST_METHOD(OrdinalImportUsesOrdinalThunkEncoding) {
		Workspace workspace;
		TestSection text{".text", {0xff, 0x25, 0, 0, 0, 0}};
		text.relocations = {{2, 1, 4}};
		workspace.write(L"input.obj", makeObject({text}, {TestSymbol{"main", 0, 1, 2, {}}, TestSymbol{"__imp_OrdinalProc", 0, 0, 2, {}}}));
		workspace.write(L"ordinal.lib", makeArchive({{"ordinal.obj", makeImportObject("OrdinalProc", "ORDINAL.dll", "", 77, 0)}}));
		auto args = outputArgs(workspace);
		args.push_back(workspace.file(L"input.obj").wstring());
		args.push_back(workspace.file(L"ordinal.lib").wstring());
		assertSuccess(runLinker(workspace, args));
		const auto image = workspace.read(L"linked.exe");
		const auto importRva = get32(image, optionalHeader(image) + 120);
		const auto lookup = rvaOffset(image, get32(image, rvaOffset(image, importRva)));
		Assert::AreEqual<std::uint64_t>(0x800000000000004dULL, get64(image, lookup), L"ordinal import encoding");
	}

	TEST_METHOD(DataImportCreatesIatWithoutFunctionThunk) {
		Workspace workspace;
		TestSection data{".data", Bytes(8, 0), 0xc0000040};
		data.relocations = {{0, 1, 1}};
		workspace.write(L"input.obj", makeObject({data, TestSection{".text", {0xc3}}}, {TestSymbol{"main", 0, 2, 2, {}}, TestSymbol{"__imp_importedData", 0, 0, 2, {}}}));
		workspace.write(L"data.lib", makeArchive({{"data.obj", makeImportObject("importedData", "DATA.dll", "importedData", 17, 1)}}));
		auto args = outputArgs(workspace);
		args.push_back(workspace.file(L"input.obj").wstring());
		args.push_back(workspace.file(L"data.lib").wstring());
		assertSuccess(runLinker(workspace, args));
		const auto image = workspace.read(L"linked.exe");
		const auto importRva = get32(image, optionalHeader(image) + 120);
		const auto descriptor = rvaOffset(image, importRva);
		const auto iatRva = get32(image, descriptor + 16);
		Assert::IsTrue(iatRva != 0, L"data import IAT is present");
		Assert::IsTrue(get32(image, optionalHeader(image) + 16) != 0, L"entry RVA remains a valid section-based address");
	}

	TEST_METHOD(StaticArchiveExtractsOnlyNeededObjectMember) {
		Workspace workspace;
		TestSection text{".text", Bytes(4, 0)};
		text.relocations = {{0, 1, 4}};
		workspace.write(L"input.obj", makeObject({text}, {TestSymbol{"main", 0, 1, 2, {}}, TestSymbol{"helper", 0, 0, 2, {}}}));
		workspace.write(L"helpers.lib", makeArchive({{"helper.obj", makeObject({TestSection{".text", {0xc3}}}, {TestSymbol{"helper", 0, 1, 2, {}}})}}));
		auto args = outputArgs(workspace);
		args.push_back(workspace.file(L"input.obj").wstring());
		args.push_back(workspace.file(L"helpers.lib").wstring());
		assertSuccess(runLinker(workspace, args));
		Assert::IsTrue(sectionHeader(workspace.read(L"linked.exe"), ".text") != 0, L"archive member section should be linked");
	}

	TEST_METHOD(DefaultLibraryAndLibraryPathResolveArchive) {
		Workspace workspace;
		TestSection text{".text", Bytes(4, 0)};
		text.relocations = {{0, 1, 4}};
		workspace.write(L"input.obj", makeObject({text}, {TestSymbol{"main", 0, 1, 2, {}}, TestSymbol{"helper", 0, 0, 2, {}}}));
		fs::create_directories(workspace.path / L"lib");
		std::ofstream library(workspace.path / L"lib" / L"default.lib", std::ios::binary);
		const auto archive = makeArchive({{"helper.obj", makeObject({TestSection{".text", {0xc3}}}, {TestSymbol{"helper", 0, 1, 2, {}}})}});
		library.write(reinterpret_cast<const char*>(archive.data()), static_cast<std::streamsize>(archive.size()));
		library.close();
		auto args = outputArgs(workspace);
		args.insert(args.begin() + 1, L"/DEFAULTLIB:default");
		args.insert(args.begin() + 1, L"/LIBPATH:" + (workspace.path / L"lib").wstring());
		args.push_back(workspace.file(L"input.obj").wstring());
		assertSuccess(runLinker(workspace, args));
	}

	TEST_METHOD(CoffDirectiveEntryPointIsHonoredAndDirectiveSectionIsConsumed) {
		Workspace workspace;
		const auto object = makeObject(
			{TestSection{".text", {0xc3}}, makeDirectiveSection("-entry:mainCRTStartup ")},
			{TestSymbol{"mainCRTStartup", 0, 1, 2, {}}});
		workspace.write(L"input.obj", object);
		assertSuccess(runLinker(workspace, {L"/OUT:" + workspace.file(L"linked.exe").wstring(), workspace.file(L"input.obj").wstring()}));
		const auto image = workspace.read(L"linked.exe");
		Assert::AreEqual<std::uint32_t>(0x1000, get32(image, optionalHeader(image) + 16), L".drectve entry selects the startup symbol");
		Assert::IsFalse(hasSection(image, ".drectve"), L"linker directives are not emitted as an image section");
	}

	TEST_METHOD(CoffDefaultLibraryDirectiveLoadsLibraryWithQuotedName) {
		Workspace workspace;
		TestSection text{".text", Bytes(4, 0)};
		text.relocations = {{0, 1, 4}};
		const auto object = makeObject({text, makeDirectiveSection("-defaultlib:\"helpers.lib\"")},
			{TestSymbol{"main", 0, 1, 2, {}}, TestSymbol{"helper", 0, 0, 2, {}}});
		workspace.write(L"input.obj", object);
		workspace.write(L"helpers.lib", makeArchive({{"helper.obj", makeObject({TestSection{".text", {0xc3}}}, {TestSymbol{"helper", 0, 1, 2, {}}})}}));
		assertSuccess(runLinker(workspace, {L"/OUT:" + workspace.file(L"linked.exe").wstring(), L"/ENTRY:main", workspace.file(L"input.obj").wstring()}));
		Assert::IsTrue(hasSection(workspace.read(L"linked.exe"), ".text"), L"default-library member was linked");
	}

	TEST_METHOD(CoffDirectivesFromSelectedArchiveMembersAreProcessed) {
		Workspace workspace;
		TestSection entryText{".text", Bytes(4, 0)};
		entryText.relocations = {{0, 1, 4}};
		workspace.write(L"input.obj", makeObject({entryText}, {TestSymbol{"main", 0, 1, 2, {}}, TestSymbol{"helper", 0, 0, 2, {}}}));
		TestSection helperText{".text", Bytes(4, 0)};
		helperText.relocations = {{0, 1, 4}};
		const auto helper = makeObject({helperText, makeDirectiveSection("-defaultlib:secondary.lib")},
			{TestSymbol{"helper", 0, 1, 2, {}}, TestSymbol{"secondarySymbol", 0, 0, 2, {}}});
		const auto unused = makeObject({makeDirectiveSection("-unknown:ignored-unless-selected")}, {TestSymbol{"unusedSymbol", 0, 0, 2, {}}});
		workspace.write(L"outer.lib", makeArchive({{"helper.obj", helper}, {"unused.obj", unused}}));
		workspace.write(L"secondary.lib", makeArchive({{"secondary.obj", makeObject({TestSection{".data", {0x6b}, 0xc0000040}}, {TestSymbol{"secondarySymbol", 0, 1, 2, {}}})}}));
		auto args = outputArgs(workspace);
		args.push_back(workspace.file(L"input.obj").wstring());
		args.push_back(workspace.file(L"outer.lib").wstring());
		assertSuccess(runLinker(workspace, args));
		const auto image = workspace.read(L"linked.exe");
		const auto data = sectionHeader(image, ".data");
		Assert::AreEqual<std::uint8_t>(0x6b, image[get32(image, data + 20)], L"selected member default library is loaded and unused member directives are ignored");
	}

	TEST_METHOD(CoffIncludeDirectiveForcesArchiveMemberExtraction) {
		Workspace workspace;
		const auto object = makeObject({TestSection{".text", {0xc3}}, makeDirectiveSection("-include:forcedSymbol")},
			{TestSymbol{"main", 0, 1, 2, {}}});
		workspace.write(L"input.obj", object);
		workspace.write(L"forced.lib", makeArchive({{"forced.obj", makeObject({TestSection{".data", {0x5a}, 0xc0000040}}, {TestSymbol{"forcedSymbol", 0, 1, 2, {}}})}}));
		assertSuccess(runLinker(workspace, {L"/OUT:" + workspace.file(L"linked.exe").wstring(), L"/ENTRY:main", workspace.file(L"input.obj").wstring(), workspace.file(L"forced.lib").wstring()}));
		const auto image = workspace.read(L"linked.exe");
		const auto data = sectionHeader(image, ".data");
		Assert::AreEqual<std::uint8_t>(0x5a, image[get32(image, data + 20)], L"forced archive member contributes its data");
	}

	TEST_METHOD(CoffAlignDirectiveSetsSectionAndFileAlignment) {
		Workspace workspace;
		const auto object = makeObject({TestSection{".text", {0xc3}}, makeDirectiveSection("-ALIGN:0x200")},
			{TestSymbol{"main", 0, 1, 2, {}}});
		assertSuccess(linkObject(workspace, object));
		const auto image = workspace.read(L"linked.exe");
		const auto optional = optionalHeader(image);
		Assert::AreEqual<std::uint32_t>(0x200, get32(image, optional + 32), L"directive sets section alignment");
		Assert::AreEqual<std::uint32_t>(0x200, get32(image, optional + 36), L"small section alignment also sets file alignment");
		Assert::AreEqual<std::uint32_t>(0x200, get32(image, sectionHeader(image, ".text") + 12), L"first section RVA obeys alignment");

		Workspace commandLineWorkspace;
		const auto overridden = makeObject({TestSection{".text", {0xc3}}, makeDirectiveSection("-ALIGN:0x200")},
			{TestSymbol{"main", 0, 1, 2, {}}});
		commandLineWorkspace.write(L"input.obj", overridden);
		auto args = outputArgs(commandLineWorkspace);
		args.insert(args.begin() + 1, L"/ALIGN:0x1000");
		args.push_back(commandLineWorkspace.file(L"input.obj").wstring());
		assertSuccess(runLinker(commandLineWorkspace, args));
		const auto overriddenImage = commandLineWorkspace.read(L"linked.exe");
		Assert::AreEqual<std::uint32_t>(0x1000, get32(overriddenImage, optionalHeader(overriddenImage) + 32), L"command-line alignment overrides the object directive");
	}

	TEST_METHOD(CoffAlignCommDirectiveAlignsCommonSymbol) {
		Workspace workspace;
		const auto object = makeObject({TestSection{".text", {0xc3}}, makeDirectiveSection("-aligncomm:alignedCommon,32")},
			{TestSymbol{"main", 0, 1, 2, {}}, TestSymbol{"alignedCommon", 4, 0, 2, {}}});
		workspace.write(L"input.obj", object);
		auto args = outputArgs(workspace);
		args.insert(args.begin() + 1, L"/MAP");
		args.push_back(workspace.file(L"input.obj").wstring());
		assertSuccess(runLinker(workspace, args));
		std::ifstream map(workspace.file(L"linked.map"));
		const std::string contents((std::istreambuf_iterator<char>(map)), std::istreambuf_iterator<char>());
		const auto symbolPosition = contents.find(" alignedCommon\n");
		Assert::IsTrue(symbolPosition != std::string::npos, L"aligned common symbol is present in the map");
		const auto lineStart = contents.rfind('\n', symbolPosition);
		const auto addressStart = lineStart == std::string::npos ? 0 : lineStart + 1;
		const auto addressEnd = contents.find(' ', addressStart);
		const auto address = std::stoull(contents.substr(addressStart, addressEnd - addressStart), nullptr, 16);
		Assert::AreEqual<std::uint64_t>(0, address % 32, L"ALIGNCOMM address respects the requested alignment");
	}

	TEST_METHOD(CoffExportDirectiveBuildsExportDirectoryAndAlias) {
		Workspace workspace;
		const auto object = makeObject({TestSection{".text", {0xc3, 0xc3}}, makeDirectiveSection("-export:api=implementation,@7")},
			{TestSymbol{"main", 0, 1, 2, {}}, TestSymbol{"implementation", 1, 1, 2, {}}});
		assertSuccess(linkObject(workspace, object));
		const auto image = workspace.read(L"linked.exe");
		const auto optional = optionalHeader(image);
		const auto exportRva = get32(image, optional + 112);
		Assert::IsTrue(exportRva != 0 && get32(image, optional + 116) >= 40, L"export data directory is present");
		const auto directory = rvaOffset(image, exportRva);
		Assert::AreEqual<std::uint32_t>(7, get32(image, directory + 16), L"export ordinal base");
		Assert::AreEqual<std::uint32_t>(1, get32(image, directory + 20), L"number of exported functions");
		Assert::AreEqual<std::uint32_t>(1, get32(image, directory + 24), L"number of named exports");
		const auto functions = rvaOffset(image, get32(image, directory + 28));
		const auto names = rvaOffset(image, get32(image, directory + 32));
		const auto ordinals = rvaOffset(image, get32(image, directory + 36));
		const auto text = sectionHeader(image, ".text");
		Assert::AreEqual<std::uint32_t>(get32(image, text + 12) + 1, get32(image, functions), L"alias resolves to the internal symbol RVA");
		Assert::IsTrue(cStringAtRva(image, get32(image, names)) == "api", L"public export name");
		Assert::AreEqual<std::uint16_t>(0, get16(image, ordinals), L"export name maps to ordinal index zero");
	}

	TEST_METHOD(CommonSymbolAllocatesBssAndAppearsInMap) {
		Workspace workspace;
		const auto object = makeObject({TestSection{".text", {0xc3}}}, {TestSymbol{"main", 0, 1, 2, {}}, TestSymbol{"commonValue", 32, 0, 2, {}}});
		workspace.write(L"input.obj", object);
		auto args = outputArgs(workspace);
		args.insert(args.begin() + 1, L"/MAP");
		args.push_back(workspace.file(L"input.obj").wstring());
		assertSuccess(runLinker(workspace, args));
		const auto image = workspace.read(L"linked.exe");
		const auto bss = sectionHeader(image, ".bss");
		Assert::IsTrue(get32(image, bss + 8) >= 32, L"common symbol storage is allocated in BSS");
		std::ifstream map(workspace.file(L"linked.map"));
		const std::string contents((std::istreambuf_iterator<char>(map)), std::istreambuf_iterator<char>());
		Assert::IsTrue(contents.find("commonValue") != std::string::npos, L"map includes common symbol");
	}

	TEST_METHOD(ComdatAnyKeepsOneDuplicateDefinition) {
		Workspace workspace;
		workspace.write(L"entry.obj", makeEntryObject());
		workspace.write(L"first.obj", makeComdatObject("shared", {0x11}, 2));
		workspace.write(L"second.obj", makeComdatObject("shared", {0x22}, 2));
		auto args = outputArgs(workspace);
		args.insert(args.begin() + 1, L"/MAP");
		for (const auto& file : {L"entry.obj", L"first.obj", L"second.obj"}) args.push_back(workspace.file(file).wstring());
		assertSuccess(runLinker(workspace, args));
		std::ifstream map(workspace.file(L"linked.map"));
		const std::string contents((std::istreambuf_iterator<char>(map)), std::istreambuf_iterator<char>());
		const auto first = contents.find(" shared\n");
		Assert::IsTrue(first != std::string::npos && contents.find(" shared\n", first + 1) == std::string::npos, L"map contains exactly one selected COMDAT symbol");
	}

	TEST_METHOD(ComdatLargestChoosesLargestCandidate) {
		Workspace workspace;
		workspace.write(L"entry.obj", makeEntryObject());
		workspace.write(L"small.obj", makeComdatObject("largest", {0x10}, 6));
		workspace.write(L"large.obj", makeComdatObject("largest", {0x20, 0x21, 0x22}, 6));
		auto args = outputArgs(workspace);
		args.insert(args.begin() + 1, L"/MAP");
		for (const auto& file : {L"entry.obj", L"small.obj", L"large.obj"}) args.push_back(workspace.file(file).wstring());
		assertSuccess(runLinker(workspace, args));
		const auto image = workspace.read(L"linked.exe");
		const auto text = sectionHeader(image, ".text");
		const auto raw = get32(image, text + 20);
		const auto rawSize = get32(image, text + 16);
		const Bytes expected{0x20, 0x21, 0x22};
		Assert::IsTrue(std::search(image.begin() + raw, image.begin() + raw + rawSize, expected.begin(), expected.end()) != image.begin() + raw + rawSize,
			L"largest COMDAT bytes are retained");
	}

	TEST_METHOD(ComdatNewestChoosesLastCandidate) {
		Workspace workspace;
		workspace.write(L"entry.obj", makeEntryObject());
		workspace.write(L"old.obj", makeComdatObject("newest", {0x31}, 7));
		workspace.write(L"new.obj", makeComdatObject("newest", {0x32}, 7));
		auto args = outputArgs(workspace);
		for (const auto& file : {L"entry.obj", L"old.obj", L"new.obj"}) args.push_back(workspace.file(file).wstring());
		assertSuccess(runLinker(workspace, args));
		const auto image = workspace.read(L"linked.exe");
		const auto text = sectionHeader(image, ".text");
		const auto raw = get32(image, text + 20);
		Assert::IsTrue(std::find(image.begin() + raw, image.begin() + raw + get32(image, text + 16), 0x32) != image.begin() + raw + get32(image, text + 16), L"NEWEST COMDAT data is selected");
	}

	TEST_METHOD(AssociativeComdatFollowsSelectedParent) {
		Workspace workspace;
		workspace.write(L"entry.obj", makeEntryObject());
		workspace.write(L"parent1.obj", makeAssociativeComdatObject({0xc3}, {0x41, 0x42}));
		workspace.write(L"parent2.obj", makeAssociativeComdatObject({0xc3}, {0x51, 0x52}));
		auto args = outputArgs(workspace);
		for (const auto& file : {L"entry.obj", L"parent1.obj", L"parent2.obj"}) args.push_back(workspace.file(file).wstring());
		assertSuccess(runLinker(workspace, args));
		const auto image = workspace.read(L"linked.exe");
		Assert::IsTrue(sectionHeader(image, ".xdata") != 0, L"selected parent's associative section is retained");
	}

	TEST_METHOD(WeakExternalResolvesFallbackFromLibrary) {
		Workspace workspace;
		TestSection text{".text", Bytes(8, 0)};
		text.relocations = {{0, 1, 1}};
		TestSymbol weak{"weakRef", 0, 0, 105, {weakAux(3, 3)}};
		TestSymbol fallback{"fallback", 0, 0, 2, {}};
		workspace.write(L"input.obj", makeObject({text}, {TestSymbol{"main", 0, 1, 2, {}}, weak, fallback}));
		workspace.write(L"fallback.lib", makeArchive({{"fallback.obj", makeObject({TestSection{".data", {0x77}, 0xc0000040}}, {TestSymbol{"fallback", 0, 1, 2, {}}})}}));
		auto args = outputArgs(workspace);
		args.push_back(workspace.file(L"input.obj").wstring());
		args.push_back(workspace.file(L"fallback.lib").wstring());
		assertSuccess(runLinker(workspace, args));
		const auto image = workspace.read(L"linked.exe");
		Assert::AreEqual<std::uint64_t>(0x140002000, get64(image, get32(image, sectionHeader(image, ".text") + 20)), L"weak alias relocates to selected fallback");
	}

	TEST_METHOD(WeakSearchNoneResolvesToZero) {
		Workspace workspace;
		TestSection text{".text", Bytes(8, 0)};
		text.relocations = {{0, 1, 1}};
		TestSymbol weak{"optional", 0, 0, 105, {weakAux(3, 1)}};
		TestSymbol fallback{"missingFallback", 0, 0, 2, {}};
		assertSuccess(linkObject(workspace, makeObject({text}, {TestSymbol{"main", 0, 1, 2, {}}, weak, fallback})));
		const auto image = workspace.read(L"linked.exe");
		Assert::AreEqual<std::uint64_t>(0, get64(image, get32(image, sectionHeader(image, ".text") + 20)), L"unresolved weak external defaults to zero");
	}

	TEST_METHOD(ResponseFileAndLinkerOptionsAreHonored) {
		Workspace workspace;
		workspace.write(L"input.obj", makeEntryObject());
		workspace.writeText(L"link.rsp", L"/OUT:\"linked.exe\" /ENTRY:main /MACHINE:X64 /SUBSYSTEM:WINDOWS,6.2 /BASE:0x180000000 /NXCOMPAT:NO /INCREMENTAL:NO /Nologo input.obj");
		assertSuccess(runLinker(workspace, {L"@" + workspace.file(L"link.rsp").wstring()}));
		const auto image = workspace.read(L"linked.exe");
		const auto optional = optionalHeader(image);
		Assert::AreEqual<std::uint64_t>(0x180000000, get64(image, optional + 24), L"/BASE");
		Assert::AreEqual<std::uint16_t>(2, get16(image, optional + 68), L"/SUBSYSTEM:WINDOWS");
		Assert::AreEqual<std::uint16_t>(6, get16(image, optional + 48), L"subsystem major version");
		Assert::AreEqual<std::uint16_t>(2, get16(image, optional + 50), L"subsystem minor version");
		Assert::IsTrue((get16(image, optional + 70) & 0x100) == 0, L"/NXCOMPAT:NO");
	}

	TEST_METHOD(GnuShortOptionsAndMapOptionAreAccepted) {
		Workspace workspace;
		workspace.write(L"input.obj", makeEntryObject());
		const auto output = workspace.file(L"short.exe").wstring();
		const auto result = runLinker(workspace, {L"-o" + output, L"-emain", L"/MAP", workspace.file(L"input.obj").wstring()});
		assertSuccess(result);
		Assert::IsTrue(fs::exists(workspace.file(L"short.map")), L"/MAP creates a map beside the executable");
	}

	TEST_METHOD(LtcgAndLlvmBitcodeInputsAreExplicitlyUnsupported) {
		Workspace rawWorkspace;
		const auto rawBitcode = Bytes{'B', 'C', 0xc0, 0xde, 0x00, 0x00, 0x00, 0x00};
		rawWorkspace.write(L"input.obj", rawBitcode);
		auto args = outputArgs(rawWorkspace);
		args.push_back(rawWorkspace.file(L"input.obj").wstring());
		assertFailureContains(runLinker(rawWorkspace, args), "LLVM bitcode/LTCG input is not supported");
		assertNoOutput(rawWorkspace);

		Workspace wrappedWorkspace;
		const auto wrappedBitcode = Bytes{0xde, 0xc0, 0x17, 0x0b, 0x00, 0x00, 0x00, 0x00};
		wrappedWorkspace.write(L"input.obj", wrappedBitcode);
		auto wrappedArgs = outputArgs(wrappedWorkspace);
		wrappedArgs.push_back(wrappedWorkspace.file(L"input.obj").wstring());
		assertFailureContains(runLinker(wrappedWorkspace, wrappedArgs), "LLVM bitcode/LTCG input is not supported");

		Workspace sectionWorkspace;
		const auto sectionObject = makeObject({TestSection{".llvmbc", rawBitcode, 0x40000040}, TestSection{".text", {0xc3}}},
			{TestSymbol{"main", 0, 2, 2, {}}});
		assertFailureContains(linkObject(sectionWorkspace, sectionObject), "LLVM bitcode/LTCG section '.llvmbc' is not supported");

		Workspace archiveWorkspace;
		archiveWorkspace.write(L"input.lib", makeArchive({{"module.obj", rawBitcode}}));
		assertFailureContains(runLinker(archiveWorkspace, {L"/OUT:" + archiveWorkspace.file(L"linked.exe").wstring(), archiveWorkspace.file(L"input.lib").wstring()}),
			"input.lib(module.obj): LLVM bitcode/LTCG input is not supported");
	}

	TEST_METHOD(MalformedTruncatedObjectReportsErrorAndWritesNoImage) {
		Workspace workspace;
		workspace.write(L"input.obj", Bytes{0x64, 0x86, 0x01});
		auto args = outputArgs(workspace);
		args.push_back(workspace.file(L"input.obj").wstring());
		const auto result = runLinker(workspace, args);
		assertFailureContains(result, "truncated");
		assertNoOutput(workspace);
	}

	TEST_METHOD(NonAmd64ObjectIsRejected) {
		Workspace workspace;
		const auto result = linkObject(workspace, makeObject({TestSection{".text", {0xc3}}}, {TestSymbol{"main", 0, 1, 2, {}}}, 0x14c));
		assertFailureContains(result, "expected AMD64 COFF object");
		assertNoOutput(workspace);
	}

	TEST_METHOD(UnresolvedExternalIsReported) {
		Workspace workspace;
		TestSection text{".text", Bytes(4, 0)};
		text.relocations = {{0, 1, 4}};
		const auto result = linkObject(workspace, makeObject({text}, {TestSymbol{"main", 0, 1, 2, {}}, TestSymbol{"notFound", 0, 0, 2, {}}}));
		assertFailureContains(result, "notFound");
		assertNoOutput(workspace);
	}

	TEST_METHOD(UnsupportedAmd64RelocationIsReported) {
		Workspace workspace;
		TestSection text{".text", Bytes(8, 0)};
		text.relocations = {{0, 0, 10}};
		const auto result = linkObject(workspace, makeObject({text}, {TestSymbol{"main", 0, 1, 2, {}}}));
		assertFailureContains(result, "unsupported AMD64 relocation type");
	}

	TEST_METHOD(RelocationOutsideInitializedDataIsRejected) {
		Workspace workspace;
		TestSection text{".text", {0xc3}};
		text.relocations = {{1, 0, 4}};
		const auto result = linkObject(workspace, makeObject({text}, {TestSymbol{"main", 0, 1, 2, {}}}));
		assertFailureContains(result, "relocation offset is outside its section");
	}

	TEST_METHOD(Addr32OverflowIsRejected) {
		Workspace workspace;
		TestSection data{".data", Bytes(4, 0), 0xc0000040};
		data.relocations = {{0, 0, 2}};
		const auto object = makeObject({data, TestSection{".text", {0xc3}}}, {TestSymbol{"main", 0, 2, 2, {}}});
		const auto result = linkObject(workspace, object);
		assertFailureContains(result, "does not fit its 32-bit relocation type");
	}

	TEST_METHOD(DuplicateStrongDefinitionsAreRejected) {
		Workspace workspace;
		workspace.write(L"first.obj", makeEntryObject());
		workspace.write(L"second.obj", makeEntryObject());
		auto args = outputArgs(workspace);
		args.push_back(workspace.file(L"first.obj").wstring());
		args.push_back(workspace.file(L"second.obj").wstring());
		assertFailureContains(runLinker(workspace, args), "duplicate definition");
	}

	TEST_METHOD(ComdatNoDuplicatesAndSizeMismatchAreRejected) {
		Workspace workspace;
		workspace.write(L"entry.obj", makeEntryObject());
		workspace.write(L"first.obj", makeComdatObject("duplicate", {0x11}, 1));
		workspace.write(L"second.obj", makeComdatObject("duplicate", {0x11}, 1));
		auto args = outputArgs(workspace);
		for (const auto& file : {L"entry.obj", L"first.obj", L"second.obj"}) args.push_back(workspace.file(file).wstring());
		assertFailureContains(runLinker(workspace, args), "duplicate NODUPLICATES COMDAT");
		Workspace sizeWorkspace;
		sizeWorkspace.write(L"entry.obj", makeEntryObject());
		sizeWorkspace.write(L"first.obj", makeComdatObject("sameSize", {0x11}, 3));
		sizeWorkspace.write(L"second.obj", makeComdatObject("sameSize", {0x11, 0x22}, 3));
		auto sizeArgs = outputArgs(sizeWorkspace);
		for (const auto& file : {L"entry.obj", L"first.obj", L"second.obj"}) sizeArgs.push_back(sizeWorkspace.file(file).wstring());
		assertFailureContains(runLinker(sizeWorkspace, sizeArgs), "SAME_SIZE COMDAT mismatch");
	}

	TEST_METHOD(ComdatExactMatchMismatchIsRejected) {
		Workspace workspace;
		workspace.write(L"entry.obj", makeEntryObject());
		workspace.write(L"first.obj", makeComdatObject("exact", {0x11}, 4));
		workspace.write(L"second.obj", makeComdatObject("exact", {0x22}, 4));
		auto args = outputArgs(workspace);
		for (const auto& file : {L"entry.obj", L"first.obj", L"second.obj"}) args.push_back(workspace.file(file).wstring());
		assertFailureContains(runLinker(workspace, args), "EXACT_MATCH COMDAT mismatch");
	}

	TEST_METHOD(InvalidWeakSearchModeIsRejected) {
		Workspace workspace;
		TestSymbol weak{"weakRef", 0, 0, 105, {weakAux(3, 4)}};
		TestSymbol fallback{"fallback", 0, 0, 2, {}};
		const auto result = linkObject(workspace, makeObject({TestSection{".text", {0xc3}}}, {TestSymbol{"main", 0, 1, 2, {}}, weak, fallback}));
		assertFailureContains(result, "unsupported weak-external search mode");
	}

	TEST_METHOD(MalformedArchiveAndMalformedImportObjectAreRejected) {
		Workspace workspace;
		workspace.write(L"bad.lib", Bytes{'n', 'o', 'p', 'e'});
		auto args = outputArgs(workspace);
		args.push_back(workspace.file(L"bad.lib").wstring());
		assertFailureContains(runLinker(workspace, args), "truncated");
		Workspace importWorkspace;
		TestSection text{".text", {0xff, 0x25, 0, 0, 0, 0}};
		text.relocations = {{2, 1, 4}};
		importWorkspace.write(L"input.obj", makeObject({text}, {TestSymbol{"main", 0, 1, 2, {}}, TestSymbol{"__imp_bad", 0, 0, 2, {}}}));
		auto badImport = makeImportObject("bad", "BAD.dll", "bad", 0, 4);
		put16(badImport, 6, 0x14c);
		importWorkspace.write(L"bad.lib", makeArchive({{"bad.obj", badImport}}));
		auto importArgs = outputArgs(importWorkspace);
		importArgs.push_back(importWorkspace.file(L"input.obj").wstring());
		importArgs.push_back(importWorkspace.file(L"bad.lib").wstring());
		assertFailureContains(runLinker(importWorkspace, importArgs), "import library member is not AMD64");
	}

	TEST_METHOD(MalformedAndUnsupportedCoffDirectivesAreReported) {
		Workspace workspace;
		const auto unknown = makeObject({TestSection{".text", {0xc3}}, makeDirectiveSection("-not-a-linker-option:value")},
			{TestSymbol{"main", 0, 1, 2, {}}});
		const auto unknownResult = linkObject(workspace, unknown);
		assertFailureContains(unknownResult, "unsupported .drectve option");
		assertNoOutput(workspace);

		Workspace alignmentWorkspace;
		const auto invalidAlignment = makeObject({TestSection{".text", {0xc3}}, makeDirectiveSection("-align:300")},
			{TestSymbol{"main", 0, 1, 2, {}}});
		assertFailureContains(linkObject(alignmentWorkspace, invalidAlignment), ".drectve /ALIGN must be a power of two");

		Workspace quoteWorkspace;
		const auto malformedQuote = makeObject({TestSection{".text", {0xc3}}, makeDirectiveSection("-defaultlib:\"unterminated")},
			{TestSymbol{"main", 0, 1, 2, {}}});
		assertFailureContains(linkObject(quoteWorkspace, malformedQuote), "unterminated quote in .drectve");

		Workspace exportWorkspace;
		const auto malformedExport = makeObject({TestSection{".text", {0xc3}}, makeDirectiveSection("-export:api,@1,@2")},
			{TestSymbol{"main", 0, 1, 2, {}}, TestSymbol{"api", 0, 1, 2, {}}});
		assertFailureContains(linkObject(exportWorkspace, malformedExport), "specifies more than one ordinal");
	}

	TEST_METHOD(CommandLineAndResponseFileErrorsAreReported) {
		Workspace workspace;
		workspace.write(L"input.obj", makeEntryObject());
		assertFailureContains(runLinker(workspace, {L"/UNKNOWN", workspace.file(L"input.obj").wstring()}), "unsupported option");
		assertFailureContains(runLinker(workspace, {L"/BASE:0x12345", workspace.file(L"input.obj").wstring()}), "64-KiB-aligned");
		assertFailureContains(runLinker(workspace, {L"/ALIGN:300", workspace.file(L"input.obj").wstring()}), "/ALIGN must be a power of two");
		assertFailureContains(runLinker(workspace, {L"/SUBSYSTEM:EFI", workspace.file(L"input.obj").wstring()}), "unsupported subsystem");
		assertFailureContains(runLinker(workspace, {L"/MACHINE:X86", workspace.file(L"input.obj").wstring()}), "only supports /MACHINE:X64");
		assertFailureContains(runLinker(workspace, {L"/INCREMENTAL", workspace.file(L"input.obj").wstring()}), "INCREMENTAL:NO");
		assertFailureContains(runLinker(workspace, {L"@missing.rsp"}), "cannot open response file");
		workspace.writeText(L"bad.rsp", L"/OUT:\"unterminated");
		assertFailureContains(runLinker(workspace, {L"@" + workspace.file(L"bad.rsp").wstring()}), "unterminated quote");
	}

	TEST_METHOD(MissingInputsAndMissingFilesAreReported) {
		Workspace workspace;
		assertFailureContains(runLinker(workspace, {}), "no input object files or libraries");
		assertFailureContains(runLinker(workspace, {L"missing.obj"}), "cannot open");
	}
};
}
